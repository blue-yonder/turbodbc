#include <turbodbc/parameter_sets/set_field.h>

#include <variant>
#include <cstring>
#include <chrono>
#include <sql.h>

namespace turbodbc {

namespace {

	std::size_t const size_not_important = 0;

	class is_suitable_for {
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

		bool operator()(std::chrono::system_clock::time_point const &) const {
			return parameter_.is_suitable_for(type_code::timestamp, size_not_important);
		}

		bool operator()(std::chrono::year_month_day const &) const {
			return parameter_.is_suitable_for(type_code::date, size_not_important);
		}

		bool operator()(std::string const & value) const {
			return parameter_.is_suitable_for(type_code::string, value.size());
		}

	private:
		parameter const & parameter_;
	};


	class set_field_for {
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
			*reinterpret_cast<double *>(destination_.data_pointer) = value;
			destination_.indicator = sizeof(double);
		}

		void operator()(std::chrono::system_clock::time_point const & value)
		{
			auto tp = value;
			auto dp = std::chrono::floor<std::chrono::days>(tp);
			std::chrono::year_month_day ymd{dp};
			auto time_of_day = tp - dp;
			auto hrs   = std::chrono::duration_cast<std::chrono::hours>(time_of_day);
			time_of_day -= hrs;
			auto mins  = std::chrono::duration_cast<std::chrono::minutes>(time_of_day);
			time_of_day -= mins;
			auto secs  = std::chrono::duration_cast<std::chrono::seconds>(time_of_day);
			time_of_day -= secs;
			auto micros = std::chrono::duration_cast<std::chrono::microseconds>(time_of_day);

			auto destination = reinterpret_cast<SQL_TIMESTAMP_STRUCT *>(destination_.data_pointer);
			destination->year = static_cast<SQLSMALLINT>(static_cast<int>(ymd.year()));
			destination->month = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.month()));
			destination->day = static_cast<SQLUSMALLINT>(static_cast<unsigned>(ymd.day()));
			destination->hour = static_cast<SQLUSMALLINT>(hrs.count());
			destination->minute = static_cast<SQLUSMALLINT>(mins.count());
			destination->second = static_cast<SQLUSMALLINT>(secs.count());
			destination->fraction = static_cast<SQLUINTEGER>(micros.count() * 1000);
			destination_.indicator = sizeof(SQL_TIMESTAMP_STRUCT);
		}

		void operator()(std::chrono::year_month_day const & value)
		{
			auto destination = reinterpret_cast<SQL_DATE_STRUCT *>(destination_.data_pointer);
			destination->year = static_cast<SQLSMALLINT>(static_cast<int>(value.year()));
			destination->month = static_cast<SQLUSMALLINT>(static_cast<unsigned>(value.month()));
			destination->day = static_cast<SQLUSMALLINT>(static_cast<unsigned>(value.day()));
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
	return std::visit(is_suitable_for(param), value);
}

void set_field(field const & value, cpp_odbc::writable_buffer_element & destination)
{
	set_field_for visitor(destination);
	std::visit(visitor, value);
}

void set_null(cpp_odbc::writable_buffer_element & destination)
{
	destination.indicator = SQL_NULL_DATA;
}

}