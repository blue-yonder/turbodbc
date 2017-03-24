#include <turbodbc/cursor.h>
#include <turbodbc/make_description.h>

#include <turbodbc/buffer_size.h>

#include <cpp_odbc/statement.h>
#include <cpp_odbc/error.h>

#include <boost/variant/get.hpp>
#include <sqlext.h>
#include <stdexcept>

#include <cstring>
#include <sstream>
//#include <codecvt>
//#include <locale>

#include <boost/locale.hpp>

namespace {

    std::u16string as_utf16(std::string utf8_encoded) {
        // not all compilers support the new C++11 conversion facets
//        static std::wstring_convert<std::codecvt_utf8_utf16<char16_t>, char16_t> converter;
//        return converter.from_bytes(utf8_encoded);
        return boost::locale::conv::utf_to_utf<char16_t>(utf8_encoded);
    }

}

namespace turbodbc {

cursor::cursor(std::shared_ptr<cpp_odbc::connection const> connection,
               turbodbc::configuration configuration) :
    connection_(connection),
    configuration_(std::move(configuration)),
    command_()
{
}

cursor::~cursor() = default;

void cursor::prepare(std::string const & sql)
{
    results_.reset();
    command_.reset();
    auto statement = connection_->make_statement();
    if (configuration_.options.prefer_unicode) {
        statement->prepare(as_utf16(sql));
    } else {
        statement->prepare(sql);
    }
    command_ = std::make_shared<command>(statement, configuration_);
}

void cursor::execute()
{
    command_->execute();
    auto raw_result_set = command_->get_results();
    if (raw_result_set) {
        results_ = raw_result_set;
    }
}

std::shared_ptr<result_sets::result_set> cursor::get_result_set() const
{
    return command_->get_results();
}

int64_t cursor::get_row_count()
{
    return command_->get_row_count();
}

std::shared_ptr<cpp_odbc::connection const> cursor::get_connection() const
{
    return connection_;
}

std::shared_ptr<turbodbc::command> cursor::get_command()
{
    return command_;
}


}
