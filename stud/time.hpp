
#ifndef _STUD_TIME

#include "forward.hpp"
#include <Windows.h>
#include <ctime>
#include <string>
#include "io.hpp"
#include "result.hpp"
#include "utility.hpp"

_STD_API_BEGIN

enum class TimeConvert {
    Local, UTC
};
enum Month : size_t {
    January,
    February,
    March,
    April,
    May,
    June,
    July,
    August,
    September,
    October,
    November,
    December
};

inline static std::string_view month_to_string(Month month, Case c = Case::Capitalized) {
    switch (month) {
    case Month::January:
        return c == Case::Capitalized ? "January" : c == Case::Lower ? "january" : "JANUARY";
    case Month::February:
        return c == Case::Capitalized ? "February" : c == Case::Lower ? "february" : "FEBRUARY";
    case Month::March:
        return c == Case::Capitalized ? "March" : c == Case::Lower ? "march" : "MARCH";
    case Month::April:
        return c == Case::Capitalized ? "April" : c == Case::Lower ? "april" : "APRIL";
    case Month::May:
        return c == Case::Capitalized ? "May" : c == Case::Lower ? "may" : "MAY";
    case Month::June:
        return c == Case::Capitalized ? "June" : c == Case::Lower ? "june" : "JUNE";
    case Month::July:
        return c == Case::Capitalized ? "July" : c == Case::Lower ? "july" : "JULY";
    case Month::August:
        return c == Case::Capitalized ? "August" : c == Case::Lower ? "august" : "AUGUST";
    case Month::September:
        return c == Case::Capitalized ? "September" : c == Case::Lower ? "september" : "SEPTEMBER";
    case Month::October:
        return c == Case::Capitalized ? "October" : c == Case::Lower ? "october" : "OCTOBER";
    case Month::November:
        return c == Case::Capitalized ? "November" : c == Case::Lower ? "november" : "NOVEMBER";
    case Month::December:
        return c == Case::Capitalized ? "December" : c == Case::Lower ? "december" : "DECEMBER";
    default:
        return c == Case::Capitalized ? "Unknown" : c == Case::Lower ? "unknown" : "UNKNOWN";
    }
}

// Sunday is first to be compliant with the <ctime> api
enum Day : size_t {
    Sunday,
    Monday,
    Tuesday,
    Wednesday,
    Thursday,
    Friday,
    Saturday,
};

inline static std::string_view day_to_string(Day day, Case c = Case::Capitalized)
{
    switch (day) {
    case Day::Monday:
        return c == Case::Capitalized ? "Monday" : c == Case::Lower ? "monday" : "MONDAY";
    case Day::Tuesday:
        return c == Case::Capitalized ? "Tuesday" : c == Case::Lower ? "tuesday" : "TUESDAY";
    case Day::Wednesday:
        return c == Case::Capitalized ? "Wednesday" : c == Case::Lower ? "wednesday" : "WEDNESDAY";
    case Day::Thursday:
        return c == Case::Capitalized ? "Thursday" : c == Case::Lower ? "thursday" : "THURSDAY";
    case Day::Friday:
        return c == Case::Capitalized ? "Friday" : c == Case::Lower ? "friday" : "FRIDAY";
    case Day::Saturday:
        return c == Case::Capitalized ? "Saturday" : c == Case::Lower ? "saturday" : "SATURDAY";
    case Day::Sunday:
        return c == Case::Capitalized ? "Sunday" : c == Case::Lower ? "sunday" : "SUNDAY";
    default:
        return c == Case::Capitalized ? "Unknown" : c == Case::Lower ? "unknown" : "UNKNOWN";
    }
}
inline static std::string_view get_postfix_for_date_number(std::size_t number) {
    switch (number) {
    case 1:
        return "st";
    case 2:
        return "nd";
    case 3:
        return "rd";
    case 4:
        return "th";
    case 5:
        return "th";
    case 6:
        return "th";
    case 7:
        return "th";
    case 8:
        return "th";
    case 9:
        return "th";
    case 10:
        return "th";
    case 11:
        return "th";
    case 12:
        return "th";
    case 13:
        return "th";
    case 14:
        return "th";
    case 15:
        return "th";
    case 16:
        return "th";
    case 17:
        return "th";
    case 18:
        return "th";
    case 19:
        return "th";
    case 20:
        return "th";
    case 21:
        return "st";
    case 22:
        return "nd";
    case 23:
        return "rd";
    case 24:
        return "th";
    case 25:
        return "th";
    case 26:
        return "th";
    case 27:
        return "th";
    case 28:
        return "th";
    case 29:
        return "th";
    case 30:
        return "th";
    case 31:
        return "st";
    default:
        return "";
    }
}

struct _DatetimeInternals {
    size_t Year, Month, Day, Millis;
    size_t Seconds, Minutes, Hours;
    size_t DayOfMonth, MonthsSinceJanuary, WeekDay;
    size_t DayOfYear;
    bool IsDaylightSaving;
};

class DateTime {
private:
    _DatetimeInternals _Data;
public:
    inline DateTime(const TimeConvert convert = TimeConvert::Local) {
        // The default constructor gets the NOW time.
        auto tt = std::time(NULL);
        struct tm* timeInfo = convert == TimeConvert::Local ? std::localtime(&tt) : std::gmtime(&tt);
        SYSTEMTIME stime = {};
        GetSystemTime(&stime);

        _Data.Day = stime.wDay;
        _Data.Minutes = stime.wHour;
        _Data.Seconds = stime.wSecond;
        _Data.Millis = stime.wMilliseconds;
        _Data.Year = stime.wYear;

        _Data.Day = timeInfo->tm_mday;
        _Data.DayOfMonth = timeInfo->tm_mday;
        _Data.IsDaylightSaving = static_cast<bool>(timeInfo->tm_isdst);
        _Data.MonthsSinceJanuary = timeInfo->tm_mon;
    }
    inline DateTime(const _DatetimeInternals* data) {
        std::memcpy(&_Data, data, sizeof(_DatetimeInternals));
    }
    // Values that cannot be resolved from a file time are:
    // Day
    // DayOfMonth
    // IsDaylightSaving
    // MonthsSinceJanuary
    inline DateTime(FILETIME* ft) {
        // The default constructor gets the NOW time.
        SYSTEMTIME stime = {};
        // This function is very unlikely to fail.
        FileTimeToSystemTime(ft, &stime);

        _Data.Day = stime.wDay;
        _Data.Minutes = stime.wMinute;
        _Data.Hours = stime.wHour;
        _Data.Seconds = stime.wSecond;
        _Data.Millis = stime.wMilliseconds;
        _Data.Year = stime.wYear;

        _Data.Day = NULL;
        _Data.DayOfMonth = NULL;
        _Data.IsDaylightSaving = NULL;
        _Data.MonthsSinceJanuary = NULL;
    }
    inline DateTime(std::nullptr_t) noexcept {
        _Data.Day = 0;
        _Data.DayOfMonth = 0;
        _Data.DayOfYear = 0;
        _Data.Hours = 0;
        _Data.IsDaylightSaving = false;
        _Data.Millis = 0;
        _Data.Minutes = 0;
        _Data.Month = 0;
        _Data.MonthsSinceJanuary = 0;
        _Data.Seconds = 0;
        _Data.WeekDay = 0;
        _Data.Year = 0;
    }

    inline size_t seconds() const noexcept { return _Data.Seconds; }
    inline size_t minutes() const noexcept { return _Data.Minutes; }
    inline size_t hours() const noexcept { return _Data.Hours; }

    inline size_t month_day() const noexcept { return _Data.DayOfMonth; }
    inline Month month() const noexcept { return static_cast<Month>(_Data.Month); }

    inline Day day() const noexcept { return static_cast<Day>(_Data.WeekDay); }

    inline bool is_daylight_savings() const noexcept { return _Data.IsDaylightSaving; }

    inline size_t year() const noexcept {
        return _Data.Year;
    }

    inline std::string time() const noexcept {
        return stud::format("{:02}:{:02}:{:02}", hours(), minutes(), seconds());
    }
    inline std::string date() const noexcept {
        return stud::format("{:02}/{:02}/{:02}", _Data.DayOfMonth, static_cast<size_t>(month()) + 1, year());
    }
    inline std::string month_string() const noexcept {
        const auto postfix = get_postfix_for_date_number(this->month_day());
        const auto month = month_to_string(this->month());

        return stud::format("{}{} of {}", month_day(), postfix, month);
    }

    inline static DateTime now_local() noexcept {
        return DateTime();
    }
    inline static DateTime now_utc() noexcept {
        return DateTime(TimeConvert::UTC);
    }

    inline static DateTime none() {
        return DateTime(nullptr);
    }
};

_STD_API_END

#define _STUD_TIME
#endif