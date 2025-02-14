#include <turbodbc/result_sets/bound_result_set.h>

#include <turbodbc/make_description.h>

#include <turbodbc/buffer_size.h>

#include <sqlext.h>

namespace turbodbc { namespace result_sets {

namespace {

    std::size_t determine_buffered_rows(cpp_odbc::statement const & statement,
                                        turbodbc::options const & options)
    {
        std::size_t const n_columns = statement.number_of_columns();
        std::vector<std::unique_ptr<description const>> descriptions;
        for (std::size_t one_based_index = 1; one_based_index <= n_columns; ++one_based_index) {
            auto const from_db = options.prefer_unicode ? statement.describe_column_wide(one_based_index)
                                                        : statement.describe_column(one_based_index);
            descriptions.push_back(make_description(from_db, options));
        }

        return boost::apply_visitor(turbodbc::determine_rows_to_buffer(descriptions), options.read_buffer_size);
    }

}

bound_result_set::bound_result_set(std::shared_ptr<cpp_odbc::statement const> statement,
                                   turbodbc::options const & options) :
    statement_(statement),
    rows_fetched_(0)
{
    std::size_t const n_columns = statement_->number_of_columns();
    std::size_t rows_to_buffer = determine_buffered_rows(*statement_, options);

    for (std::size_t one_based_index = 1; one_based_index <= n_columns; ++one_based_index) {
        auto const from_db = options.prefer_unicode ? statement->describe_column_wide(one_based_index)
                                                    : statement->describe_column(one_based_index);
        auto column_description = make_description(from_db, options);
        columns_.emplace_back(*statement, one_based_index, rows_to_buffer, std::move(column_description));
    }

    statement_->set_attribute(SQL_ATTR_ROW_ARRAY_SIZE, rows_to_buffer);
    rebind();
}

bound_result_set::~bound_result_set() = default;


bound_result_set::bound_result_set(bound_result_set && other) :
    statement_(std::move(other.statement_)),
    columns_(std::move(other.columns_)),
    rows_fetched_(other.rows_fetched_)
{
    rebind();
}

void bound_result_set::rebind()
{
    for (auto & column : columns_) {
        column.bind();
    }
    statement_->set_attribute(SQL_ATTR_ROWS_FETCHED_PTR, &rows_fetched_);
}


std::size_t bound_result_set::do_fetch_next_batch()
{
    statement_->fetch_next();
    return rows_fetched_;
}


std::vector<column_info> bound_result_set::do_get_column_info() const
{
    std::vector<column_info> infos;
    for (auto const & column : columns_) {
        infos.push_back(column.get_info());
    }
    return infos;
}


std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> bound_result_set::do_get_buffers() const
{
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> buffers;
    for (auto const & column : columns_) {
        buffers.push_back(std::cref(column.get_buffer()));
    }
    return buffers;
}


} }
