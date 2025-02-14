#pragma once

#include <turbodbc/result_sets/result_set.h>
#include <turbodbc/column.h>
#include <turbodbc/configuration.h>

#include <cpp_odbc/statement.h>
#include <memory>


namespace turbodbc { namespace result_sets {

/**
 * @brief This class implements result_set by associating buffers with a
 *        real ODBC statement.
 */
class bound_result_set : public turbodbc::result_sets::result_set {
public:
    /**
     * @brief Prepare and bind buffers suitable of holding buffered_rows to
     *        the given statement.
     */
    bound_result_set(std::shared_ptr<cpp_odbc::statement const> statement,
                     turbodbc::options const & options);

    /**
     * @brief Move constructor
     */
    bound_result_set(bound_result_set && other);

    virtual ~bound_result_set();

    /**
     * @brief Rebind the internal buffers to the statement.
     */
    void rebind();
private:
    std::size_t do_fetch_next_batch() final;
    std::vector<column_info> do_get_column_info() const final;
    std::vector<std::reference_wrapper<cpp_odbc::multi_value_buffer const>> do_get_buffers() const final;

    std::shared_ptr<cpp_odbc::statement const> statement_;
    std::vector<column> columns_;
    std::size_t rows_fetched_;
};


} }
