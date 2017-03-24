#pragma once

#include <turbodbc/buffer_size.h>
#include <cpp_odbc/connection.h>

namespace turbodbc {

struct options {
    options();
    buffer_size read_buffer_size;
    std::size_t parameter_sets_to_buffer;
    bool use_async_io;
    bool prefer_unicode;
};

struct capabilities {
    capabilities(cpp_odbc::connection const & connection);
    bool supports_describe_parameter;
};

struct configuration {
    configuration(turbodbc::options options, turbodbc::capabilities capabilities);
    turbodbc::options options;
    turbodbc::capabilities capabilities;
};

}