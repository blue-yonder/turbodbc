#include <turbodbc/field_translators/date_translator.h>

#include <chrono>

#include <sql.h>


namespace turbodbc { namespace field_translators {

date_translator::date_translator() = default;

date_translator::~date_translator() = default;

field date_translator::do_make_field(char const * data_pointer) const
{
    auto const sql_date = reinterpret_cast<SQL_DATE_STRUCT const *>(data_pointer);
    std::chrono::year_month_day ymd{
        std::chrono::year{sql_date->year},
        std::chrono::month{sql_date->month},
        std::chrono::day{sql_date->day}
    };
    return {ymd};
}


} }
