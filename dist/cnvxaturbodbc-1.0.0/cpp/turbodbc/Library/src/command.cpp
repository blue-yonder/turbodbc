#include <turbodbc/command.h>

#include <turbodbc/result_sets/bound_result_set.h>
#include <turbodbc/result_sets/double_buffered_result_set.h>

#include <turbodbc/buffer_size.h>


namespace turbodbc {

command::command(std::shared_ptr<cpp_odbc::statement const> statement,
                 turbodbc::configuration configuration) :
    statement_(statement),
    params_(*statement, configuration),
    configuration_(std::move(configuration))
{
}

command::~command()
{
    results_.reset(); // result may access statement concurrently!
    // statement_->close_cursor();
}

void command::execute()
{
    if (params_.get_parameters().empty()) {
        statement_->execute_prepared();
    }

    std::size_t const columns = statement_->number_of_columns();
    if (columns != 0) {
        if (configuration_.options.use_async_io) {
            results_ = std::make_shared<result_sets::double_buffered_result_set>(statement_, configuration_.options);
        } else {
            results_ = std::make_shared<result_sets::bound_result_set>(statement_, configuration_.options);
        }
    }
}

std::shared_ptr<turbodbc::result_sets::result_set> command::get_results()
{
    return results_;
}

bool command::more_results() const
{
    return statement_->more_results();
}

bound_parameter_set & command::get_parameters()
{
    return params_;
}

int64_t command::get_row_count()
{
    bool const has_result_set = (statement_->number_of_columns() != 0);
    if (has_result_set) {
        return statement_->row_count();
    } else {
        if (params_.number_of_parameters() != 0) {
            return params_.transferred_sets();
        } else {
            return statement_->row_count();
        }
    }
}

void command::finalize()
{
    const_cast<cpp_odbc::statement &>(*statement_).finalize();
}

}
