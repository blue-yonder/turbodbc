#include "cpp_odbc/error.h"
#include "cpp_odbc/level2/diagnostic_record.h"
#include <sstream>

using cpp_odbc::error;

namespace {
    std::string make_message(cpp_odbc::level2::diagnostic_record const & record)
    {
        std::ostringstream message;
        message << "ODBC error\n"
                << "state: " << record.odbc_status_code << "\n"
                << "native error code: " << record.native_error_code << "\n"
                << "message: " << record.message;
        return message.str();
    }
}

error::error(cpp_odbc::level2::diagnostic_record const & record) :
    std::runtime_error(make_message(record))
{
}

error::error(std::string const & message) :
    std::runtime_error(message)
{
}


error::~error() throw()
{}
