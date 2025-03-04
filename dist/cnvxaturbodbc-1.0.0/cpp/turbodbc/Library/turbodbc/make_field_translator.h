#pragma once

#include <turbodbc/column_info.h>
#include <turbodbc/field_translator.h>
#include <memory>

namespace turbodbc {

/**
 * @brief Create a field translator based on information on a given column
 */
std::unique_ptr<field_translator const> make_field_translator(turbodbc::column_info const & source);


}
