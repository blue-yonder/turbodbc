#include <turbodbc/make_description.h>

#include <turbodbc/descriptions.h>
#include <sqlext.h>

#include <boost/variant/apply_visitor.hpp>

#include <stdexcept>
#include <sstream>

namespace turbodbc {

namespace {

	SQLULEN const digits_representable_by_long = 18;

	/*
	 * This function returns a buffer size for the given string
	 * which leaves room for future, larger strings.
	 *
	 * The intent is to waste little space for small strings while
	 * keeping the number of required buffer rebinds small.
	 */
	std::size_t size_after_growth_strategy(std::size_t const & size)
	{
		std::size_t const minimum_size = 10;
		if (size < minimum_size) {
			return minimum_size;
		}
		return std::ceil(size * 1.2);
	}

	std::unique_ptr<description const> make_decimal_description(cpp_odbc::column_description const & source)
	{
		if (source.size <= digits_representable_by_long) {
			if (source.decimal_digits == 0) {
				return std::unique_ptr<description>(new integer_description(source.name, source.allows_null_values));
			} else {
				return std::unique_ptr<description>(new floating_point_description(source.name, source.allows_null_values));
			}
		} else {
			// fall back to strings; add two characters for decimal point and sign!
			return std::unique_ptr<description>(new string_description(source.name, source.allows_null_values, source.size + 2));
		}
	}

	using description_ptr = description const *;

	struct description_by_value : public boost::static_visitor<description_ptr> {
		description_ptr operator()(int64_t const &) const
		{
			return new integer_description;
		}

		description_ptr operator()(double const &) const
		{
			return new floating_point_description;
		}

		description_ptr operator()(bool const &) const
		{
			return new boolean_description;
		}

		description_ptr operator()(boost::gregorian::date const &) const
		{
			return new date_description;
		}

		description_ptr operator()(boost::posix_time::ptime const &) const
		{
			return new timestamp_description;
		}

		description_ptr operator()(std::string const & s) const
		{
			auto const target_size = size_after_growth_strategy(s.size());
			return new string_description(target_size);
		}
	};

}


std::unique_ptr<description const> make_description(cpp_odbc::column_description const & source, bool prefer_unicode)
{
	switch (source.data_type) {
		case SQL_CHAR:
		case SQL_VARCHAR:
		case SQL_LONGVARCHAR:
			if (prefer_unicode) {
				return std::unique_ptr<description>(new unicode_description(source.name, source.allows_null_values, source.size));
			} else {
				return std::unique_ptr<description>(new string_description(source.name, source.allows_null_values, source.size));
			}
		case SQL_WVARCHAR:
		case SQL_WLONGVARCHAR:
		case SQL_WCHAR:
			return std::unique_ptr<description>(new unicode_description(source.name, source.allows_null_values, source.size));
		case SQL_INTEGER:
		case SQL_SMALLINT:
		case SQL_BIGINT:
		case SQL_TINYINT:
			return std::unique_ptr<description>(new integer_description(source.name, source.allows_null_values));
		case SQL_REAL:
		case SQL_FLOAT:
		case SQL_DOUBLE:
			return std::unique_ptr<description>(new floating_point_description(source.name, source.allows_null_values));
		case SQL_BIT:
			return std::unique_ptr<description>(new boolean_description(source.name, source.allows_null_values));
		case SQL_NUMERIC:
		case SQL_DECIMAL:
			return make_decimal_description(source);
		case SQL_TYPE_DATE:
			return std::unique_ptr<description>(new date_description(source.name, source.allows_null_values));
		case SQL_TYPE_TIMESTAMP:
			return std::unique_ptr<description>(new timestamp_description(source.name, source.allows_null_values));
		default:
			std::ostringstream message;
			message << "Error! Unsupported type identifier '" << source.data_type << "'";
			throw std::runtime_error(message.str());
	}
}


std::unique_ptr<description const> make_description(field const & value)
{
	return std::unique_ptr<description const>(boost::apply_visitor(description_by_value{}, value));
}

std::unique_ptr<description const> make_description(type_code type, std::size_t size)
{
	switch (type) {
		case type_code::floating_point:
			return std::unique_ptr<description const>(new floating_point_description);
		case type_code::boolean:
			return std::unique_ptr<description const>(new boolean_description);
		case type_code::date:
			return std::unique_ptr<description const>(new date_description);
		case type_code::timestamp:
			return std::unique_ptr<description const>(new timestamp_description);
		case type_code::string:
			return std::unique_ptr<description const>(new string_description(size_after_growth_strategy(size)));
		case type_code::unicode:
			return std::unique_ptr<description const>(new unicode_description(size_after_growth_strategy(size)));
		default:
			return std::unique_ptr<description const>(new integer_description);
	}
}

}
