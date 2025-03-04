#include <turbodbc/errors.h>

namespace turbodbc {

interface_error::interface_error(std::string const & message) :
    logic_error(message.c_str())
{}

interface_error::~interface_error() noexcept = default;

}