#ifndef __MCE_DATA_TIME_H__
#define __MCE_DATE_TIME_H__

#include <time.h>
#include "TimeValue.h"
// ----------------------------------------------------------------------------

class DateTime
{
public:

    DateTime(void) :
            _day(0), _month(0), _year(0), _hour(0), _minute(0), _second(0), _microsec(
                    0), _wday(0)
    {
        this->update();
    }

    DateTime(const TimeValue & tv) :
            _day(0), _month(0), _year(0), _hour(0), _minute(0), _second(0), _microsec(
                    0), _wday(0)
    {
        this->update(tv);
    }

    DateTime(long day, long month = 0, long year = 0, long hour = 0,
            long minute = 0, long second = 0, long microsec = 0, long wday = 0) :
            _day(day), _month(month), _year(year), _hour(hour), _minute(minute), _second(
                    second), _microsec(microsec), _wday(wday)

    {
    }

    DateTime (const struct tm * tm_time) : _day (tm_time->tm_mday), _month (tm_time->tm_mon), _year (tm_time->tm_year),
                                    _hour (tm_time->tm_hour), _minute (tm_time->tm_min), _second (tm_time->tm_sec),
                                    _microsec (0), _wday (tm_time->tm_wday)
    {
    }

    inline void update(void)
    {
        TimeValue tv;
        tv.now();
        update(tv);
    }

    void update(const TimeValue & tv)
    {
        time_t time = tv.sec();
        struct tm tm_time;
        localtime_r(&time, &tm_time);
        this->_day = tm_time.tm_mday;
        this->_month = tm_time.tm_mon + 1;    // localtime's months are 0-11
        this->_year = tm_time.tm_year + 1900; // localtime reports years since 1900
        this->_hour = tm_time.tm_hour;
        this->_minute = tm_time.tm_min;
        this->_second = tm_time.tm_sec;
        this->_microsec = tv.msec();
        this->_wday = tm_time.tm_wday;
    }

    inline long day(void) const
    {
        return this->_day;
    }

    inline void day(long day)
    {
        this->_day = day;
    }

    inline long month(void) const
    {
        return this->_month;
    }

    inline void month(long month)
    {
        this->_month = month;
    }

    inline long year(void) const
    {
        return _year;
    }

    inline void year(long year)
    {
        this->_year = year;
    }

    inline long hour(void) const
    {
        return _hour;
    }

    inline void hour(long hour)
    {
        this->_hour = hour;
    }

    inline long minute(void) const
    {
        return this->_minute;
    }

    inline void minute(long minute)
    {
        this->_minute = minute;
    }

    inline long second(void) const
    {
        return _second;
    }

    inline void second(long second)
    {
        this->_second = second;
    }

    inline long microsec(void) const
    {
        return this->_microsec;
    }

    inline void microsec(long microsec)
    {
        this->_microsec = microsec;
    }

    inline long weekday(void) const
    {
        return this->_wday;
    }

    inline void weekday(long wday)
    {
        this->_wday = wday;
    }

protected:
    long _day;
    long _month;
    long _year;
    long _hour;
    long _minute;
    long _second;
    long _microsec;
    long _wday;
};

#endif //__MCE_DATE_TIME_H__
