#pragma once

#include <stdexcept>
#include <string>

namespace turbodbc {

class interface_error : public std::logic_error {
public:
    interface_error(std::string const & message);
    virtual ~interface_error() noexcept;
};

}