#pragma once

namespace turbodbc {

/**
 * This enumeration assigns integer values to certain database types
 */
enum class type_code : int {
	boolean = 0,            ///< boolean type
	integer = 10,           ///< integer types
	floating_point = 20,    ///< floating point types
	string = 30,            ///< string types
	unicode = 31,           ///< unicode types
	timestamp = 40,         ///< timestamp types
	date = 41               ///< date type
};

}
