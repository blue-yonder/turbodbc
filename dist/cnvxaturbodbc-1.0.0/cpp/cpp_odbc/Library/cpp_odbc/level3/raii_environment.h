#pragma once

#include "cpp_odbc/environment.h"

#include <memory>

namespace cpp_odbc {
	class environment;
}

namespace cpp_odbc { namespace level2 {
	class api;
	struct environment_handle;
} }

namespace cpp_odbc { namespace level3 {

/**
 * @brief This class represents an initialized ODBC environment.
 *        On destruction, instances automatically free acquired resources.
 */
class raii_environment : public environment {
public:
	/**
	 * @brief Initialize the environment with the given level 2 API instance
	 * @param api All API calls made by raii_environment will be delegated to this instance.
	 */
	raii_environment(std::shared_ptr<level2::api const> api);

	/**
	 * @brief Retrieve the API instance associated with this environment.
	 * @return The associated API instance
	 */
	std::shared_ptr<level2::api const> get_api() const;

	/**
	 * @brief Retrieve the non-raii environment_handle which you can use in level 2 API calls.
	 * @return The non-raii environment_handle
	 */
	level2::environment_handle const & get_handle() const;

	virtual ~raii_environment();

private:
	std::shared_ptr<connection> do_make_connection(std::string const & connection_string) const final;
	void do_set_attribute(SQLINTEGER attribute, intptr_t value) const final;

	struct intern;
	std::unique_ptr<intern> impl_;
};

} }
