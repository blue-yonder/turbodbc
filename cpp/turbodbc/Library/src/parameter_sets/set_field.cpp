#include <turbodbc/parameter_sets/set_field.h>

#include <boost/variant/static_visitor.hpp>
#include <boost/variant/apply_visitor.hpp>


namespace turbodbc {

namespace {

	std::size_t const size_not_important = 0;

	class is_suitable_for : public boost::static_visitor<bool> {
	public:
		is_suitable_for(parameter const & param) :
				parameter_(param)
		{}

		bool operator()(bool const &) const {
			return parameter_.is_suitable_for(type_code::boolean, size_not_important);
		}

		bool operator()(int64_t const &) const {
			return parameter_.is_suitable_for(type_code::integer, size_not_important);
		}

		bool operator()(double const &) const {
			return parameter_.is_suitable_for(type_code::floating_point, size_not_important);
		}

		bool operator()(boost::posix_time::ptime const &) const {
			return parameter_.is_suitable_for(type_code::timestamp, size_not_important);
		}

		bool operator()(boost::gregorian::date const &) const {
			return parameter_.is_suitable_for(type_code::date, size_not_important);
		}

		bool operator()(std::string const & value) const {
			return parameter_.is_suitable_for(type_code::string, value.size());
		}

	private:
		parameter const & parameter_;
	};


	class set_field_for : public boost::static_visitor<> {
	public:
		set_field_for(cpp_odbc::writable_buffer_element & destination) :
				destination_(destination)
		{}

		void operator()(bool const & value)
		{
			*destination_.data_pointer = (value ? 1 : 0);
			destination_.indicator = 1;
		}

		void operator()(int64_t const & value)
		{
			*reinterpret_cast<intptr_t *>(destination_.data_pointer) = value;
			destination_.indicator = sizeof(intptr_t);
		}

		void operator()(double const & value)
		{
			*reinterpret_cast<double *>(destination_.data_pointer) = boost::get<double>(value);
			destination_.indicator = sizeof(double);
		}

		void operator()(boost::posix_time::ptime const & value)
		{
			auto const & date = value.date();
			auto const & time = value.time_of_day();
			auto destination = reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(destination_.data_pointer);

			destination->year = date.year();
			destination->month = date.month();
			destination->day = date.day();
			destination->hour = time.hours();
			destination->minute = time.minutes();
			destination->second = time.seconds();
			// map posix_time microsecond precision to SQL nanosecond precision
			destination->fraction = time.fractional_seconds() * 1000;

			destination_.indicator = sizeof(SQL_TIMESTAMP_STRUCT);
		}

		void operator()(boost::gregorian::date const & value)
		{
			auto destination = reinterpret_cast<SQL_DATE_STRUCT *>(destination_.data_pointer);

			destination->year = value.year();
			destination->month = value.month();
			destination->day = value.day();

			destination_.indicator = sizeof(SQL_DATE_STRUCT);
		}

		void operator()(std::string const & value)
		{
			auto const length_with_null_termination = value.size() + 1;
			std::memcpy(destination_.data_pointer, value.c_str(), length_with_null_termination);
			destination_.indicator = value.size();
		}

	private:
		cpp_odbc::writable_buffer_element & destination_;
	};

}


bool parameter_is_suitable_for(parameter const & param, field const & value)
{
	return boost::apply_visitor(is_suitable_for(param), value);
}

void set_field(field const & value, cpp_odbc::writable_buffer_element & destination)
{
	set_field_for visitor(destination);
	boost::apply_visitor(visitor, value);
}

void set_null(cpp_odbc::writable_buffer_element & destination)
{
	destination.indicator = SQL_NULL_DATA;
}

}