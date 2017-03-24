#pragma once

#include <turbodbc/configuration.h>
#include <turbodbc/command.h>
#include <turbodbc/column_info.h>
#include <turbodbc/result_sets/result_set.h>

#include <cpp_odbc/connection.h>

#include <memory>


namespace turbodbc {

/**
 * TODO: Cursor needs proper unit tests
 */
class cursor {
public:
    cursor(std::shared_ptr<cpp_odbc::connection const> connection,
           turbodbc::configuration configuration);

    void prepare(std::string const &sql);

    void execute();

    void add_parameter_set(std::vector <nullable_field> const &parameter_set);

    int64_t get_row_count();

    std::shared_ptr <result_sets::result_set> get_result_set() const;

    std::shared_ptr<cpp_odbc::connection const> get_connection() const;

    std::shared_ptr <turbodbc::command> get_command();

    ~cursor();

private:
    std::shared_ptr<cpp_odbc::connection const> connection_;
    turbodbc::configuration configuration_;
    std::shared_ptr <turbodbc::command> command_;
    std::shared_ptr <result_sets::result_set> results_;
};

}
