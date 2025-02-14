#pragma once

#include <turbodbc/description.h>

#include <vector>
#include <memory>
#include <cstring>
#include <boost/variant.hpp>


namespace turbodbc {

/// Struct representing a buffer size with a fixed number of rows
struct rows {
    rows();
    rows(std::size_t rows_to_buffer_);
    std::size_t value;
};

/// Struct representing a buffer size with a fixed number of Megabytes
struct megabytes {
    megabytes(std::size_t megabytes_to_buffer);
    std::size_t value;
};

/// A type to encode the size of a buffer
using buffer_size = boost::variant<rows, megabytes>;
// buffer_size is default constructible when rows is


/**
 * @brief A visitor used to determine the appropriate number of rows to buffer
 *        based on a buffer size and a description of a result set
 */
class determine_rows_to_buffer
    : public boost::static_visitor<std::size_t>
{
public:
    determine_rows_to_buffer(std::vector<std::unique_ptr<turbodbc::description const>> const& descriptions);
    std::size_t operator()(rows const& r) const;
    std::size_t operator()(megabytes const& m) const;

private:
    std::vector<std::unique_ptr<description const>> const& descriptions_;
};


/**
 * @brief A visitor used to reduce the buffer size by a factor of two.
 */
class halve_buffer_size
    : public boost::static_visitor<buffer_size>
{
public:
    buffer_size operator()(rows const& r) const;
    buffer_size operator()(megabytes const& m) const;
};

}