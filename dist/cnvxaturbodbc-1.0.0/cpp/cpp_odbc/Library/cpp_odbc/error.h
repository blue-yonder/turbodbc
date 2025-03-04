#pragma once

#include <stdexcept>

namespace cpp_odbc { namespace level2 {
	struct diagnostic_record;
} }

namespace cpp_odbc {

	/**
	 * @brief cpp_odbc classes will translate errors from the underlying unixODBC to this exception.
	 */
	class error : public std::runtime_error{

	public:
		/**
		 * @brief Construct an error directly from a message
		 */
		error(std::string const & message);

		/**
		 * @brief Construct an error from a diagnostic_record
		 */
		error(level2::diagnostic_record const & record);

		virtual ~error() throw();

	};
}
