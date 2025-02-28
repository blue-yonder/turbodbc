#include <turbodbc/field_translators/timestamp_translator.h>

#include <chrono>

#include <sql.h>


namespace turbodbc { namespace field_translators {

timestamp_translator::timestamp_translator() = default;

timestamp_translator::~timestamp_translator() = default;

field timestamp_translator::do_make_field(char const * data_pointer) const
{
    auto const ts = reinterpret_cast<SQL_TIMESTAMP_STRUCT const *>(data_pointer);
    int64_t const adjusted_fraction = ts->fraction / 1000;

    std::chrono::year_month_day ymd{
        std::chrono::year{ts->year},
        std::chrono::month{ts->month},
        std::chrono::day{ts->day}
    };
    std::chrono::sys_days sd{ymd};

    auto time_of_day = std::chrono::hours(ts->hour)
                     + std::chrono::minutes(ts->minute)
                     + std::chrono::seconds(ts->second)
                     + std::chrono::microseconds(adjusted_fraction);

    std::chrono::system_clock::time_point tp = sd + time_of_day;

    return {tp};
}

} }
