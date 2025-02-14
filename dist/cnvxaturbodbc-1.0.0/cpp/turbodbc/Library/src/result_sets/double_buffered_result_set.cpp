#include <turbodbc/result_sets/double_buffered_result_set.h>

#include <turbodbc/make_description.h>

#include <turbodbc/buffer_size.h>

#include <boost/variant.hpp>
#include <sqlext.h>

#include <future>

namespace turbodbc { namespace result_sets {

namespace {

    std::size_t const stop_fetching_results = 2;

    void reader_thread(detail::message_queue<std::size_t> & read_requests,
                       detail::message_queue<std::shared_future<std::size_t>> & read_responses,
                       std::array<bound_result_set, 2> & batches)
    {
        std::size_t batch_id = 0;
        do {
            // catch exceptions since uncaught exceptions in threads are deadly
            try {
                batch_id = read_requests.pull();
                if (batch_id != stop_fetching_results) {
                    batches[batch_id].rebind();
                    std::promise<std::size_t> promise;
                    promise.set_value(batches[batch_id].fetch_next_batch());
                    read_responses.push(promise.get_future().share());
                }
            } catch (...) {
                std::promise<std::size_t> promise;
                promise.set_exception(std::current_exception());
                read_responses.push(promise.get_future().share());
            }
        } while (batch_id != stop_fetching_results);

    }

    turbodbc::options options_with_half_read_buffer(turbodbc::options options)
    {
        options.read_buffer_size = boost::apply_visitor(halve_buffer_size(), options.read_buffer_size);
        return options;
    }

}


double_buffered_result_set::double_buffered_result_set(std::shared_ptr<cpp_odbc::statement const> statement,
                                                       turbodbc::options const & options) :
    statement_(statement),
    batches_{{bound_result_set(statement_, options_with_half_read_buffer(options)),
              bound_result_set(statement_, options_with_half_read_buffer(options))}},
    active_reading_batch_(0),
    reader_(reader_thread,
            std::ref(read_requests_),
            std::ref(read_responses_),
            std::ref(batches_))
{
    read_requests_.push(active_reading_batch_);
    active_reading_batch_ = 1;
}

double_buffered_result_set::~double_buffered_result_set()
{
    read_requests_.push(stop_fetching_results);
    reader_.join();
}


std::size_t double_buffered_result_set::do_fetch_next_batch()
{
    read_requests_.push(active_reading_batch_);
    active_reading_batch_ = (active_reading_batch_ == 0) ? 1 : 0;
    return read_responses_.pull().get();
}


std::vector<column_info> double_buffered_result_set::do_get_column_info() const
{
    return batches_[active_reading_batch_].get_column_info();
}


std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> double_buffered_result_set::do_get_buffers() const
{
    return batches_[active_reading_batch_].get_buffers();
}

} }
