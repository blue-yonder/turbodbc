/**
 *  @file handles.cpp
 *  @date 13.03.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 11:59:59 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21206 $
 *
 */

#include "cpp_odbc/level2/handles.h"
#ifdef _WIN32
#include <windows.h>
#endif
#include "sql.h"

namespace cpp_odbc { namespace level2 {

signed short int connection_handle::type() const
{
	return SQL_HANDLE_DBC;
}

bool connection_handle::operator==(connection_handle const & other) const
{
	return handle == other.handle;
}

bool connection_handle::operator!=(connection_handle const & other) const
{
	return handle != other.handle;
}

signed short int environment_handle::type() const
{
	return SQL_HANDLE_ENV;
}

bool environment_handle::operator==(environment_handle const & other) const
{
	return handle == other.handle;
}

bool environment_handle::operator!=(environment_handle const & other) const
{
	return handle != other.handle;
}

signed short int statement_handle::type() const
{
	return SQL_HANDLE_STMT;
}

bool statement_handle::operator==(statement_handle const & other) const
{
	return handle == other.handle;
}

bool statement_handle::operator!=(statement_handle const & other) const
{
	return handle != other.handle;
}

} }
