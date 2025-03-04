#pragma once

#include <turbodbc/buffer_size.h>
#include <cpp_odbc/connection.h>

namespace turbodbc {

struct options {
    options();
    buffer_size read_buffer_size;
    std::size_t parameter_sets_to_buffer;
    std::size_t varchar_max_character_limit;
    bool use_async_io;
    bool prefer_unicode;
    bool autocommit;
    bool large_decimals_as_64_bit_types;
    bool limit_varchar_results_to_max;
    bool force_extra_capacity_for_unicode;
    bool fetch_wchar_as_char;
};

struct capabilities {
    capabilities(cpp_odbc::connection const & connection);
    capabilities(bool supports_describe_parameter);
    bool supports_describe_parameter;
};

struct configuration {
    configuration(turbodbc::options options, turbodbc::capabilities capabilities);
    turbodbc::options options;
    turbodbc::capabilities capabilities;
};

}
