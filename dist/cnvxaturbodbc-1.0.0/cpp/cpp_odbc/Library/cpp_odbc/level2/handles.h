#pragma once
/**
 *  @file handles.h
 *  @date 13.03.2014
 *  @author mkoenig
 *  @brief 
 *
 *  $LastChangedDate: 2014-11-28 11:59:59 +0100 (Fr, 28 Nov 2014) $
 *  $LastChangedBy: mkoenig $
 *  $LastChangedRevision: 21206 $
 *
 */

namespace cpp_odbc { namespace level2 {

/**
 * @brief This struct represents a raw handle to a unixODBC connection
 */
struct connection_handle {
	void * handle;						///< the actual handle for use with unixODBC API functions

	/**
	 * @brief Returns a constants which determines the handle type for use with unixODBC
	 *        API functions
	 */
	signed short int type() const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are equal when their
	 *        handle members are equal.
	 */
	bool operator==(connection_handle const & other) const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are not equal when their
	 *        handle members are not equal.
	 */
	bool operator!=(connection_handle const & other) const;
};

/**
 * @brief This struct represents a raw handle to a unixODBC environment
 */
struct environment_handle {
	void * handle;						///< the actual handle for use with unixODBC API functions

	/**
	 * @brief Returns a constants which determines the handle type for use with unixODBC
	 *        API functions
	 */
	signed short int type() const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are equal when their
	 *        handle members are equal.
	 */
	bool operator==(environment_handle const & other) const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are not equal when their
	 *        handle members are not equal.
	 */
	bool operator!=(environment_handle const & other) const;
};

/**
 * @brief This struct represents a raw handle to a unixODBC statement
 */
struct statement_handle {
	void * handle;						///< the actual handle for use with unixODBC API functions

	/**
	 * @brief Returns a constants which determines the handle type for use with unixODBC
	 *        API functions
	 */
	signed short int type() const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are equal when their
	 *        handle members are equal.
	 */
	bool operator==(statement_handle const & other) const;

	/**
	 * @brief Compares two handles with each other. Two handle structs are not equal when their
	 *        handle members are not equal.
	 */
	bool operator!=(statement_handle const & other) const;
};

} }
