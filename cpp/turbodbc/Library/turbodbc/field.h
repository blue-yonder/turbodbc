#pragma once

#include <variant>
#include <optional>
#include <chrono>
#include <string>

namespace turbodbc {

/**
 * @brief This type represents a single field in a table, i.e., the data associated
 *        with a given row and column.
 */
using date  = std::chrono::year_month_day;
using ptime = std::chrono::system_clock::time_point;

using field = std::variant<
    int64_t,
    std::string,
    bool,
    double,
    date,
    ptime
>;

/**
 * @brief This type represents a single nullable field in a table.
 */
using nullable_field = std::optional<field>;

}
