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
#include <codecvt>

#include <simdutf.h>

namespace {

    std::u16string as_utf16(std::string utf8_encoded) {
        size_t expected_utf16_chars = simdutf::utf16_length_from_utf8(utf8_encoded.data(), utf8_encoded.size());
        std::unique_ptr<char16_t[]> utf16{new char16_t[expected_utf16_chars]};
        size_t utf16_chars = simdutf::convert_utf8_to_utf16le(utf8_encoded.data(), utf8_encoded.size(), utf16.get());
        return std::u16string(utf16.get(), utf16_chars);
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
    reset();
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

bool cursor::more_results() const
{
    return command_->more_results();
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

void cursor::reset()
{
    results_.reset();
    if(command_) {
        command_->finalize();
    }
    command_.reset();
}

}
