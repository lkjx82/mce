#ifndef __MCE_TIME_VALUE_H__
#define __MCE_TIME_VALUE_H__

#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <stdint.h>

class TimeValue
{
public:
	TimeValue()
	{
		memset(&_tv, 0, sizeof(struct timeval));
//        this->now ();
	}

	TimeValue(time_t sec, suseconds_t usec)
	{
		this->set(sec, usec);
	}

	inline void set(time_t sec, suseconds_t usec)
	{
		_tv.tv_sec = sec;
		_tv.tv_usec = usec;
	}

	inline void set(const TimeValue & r)
	{
		memcpy(&this->_tv, &r._tv, sizeof(struct timeval));
	}

	inline void setMesc(long mecs)
	{
		_tv.tv_sec = mecs / 1000L;
		_tv.tv_usec = mecs * 1000L % 1000000L;
	}

	inline void setUsec(suseconds_t usec)
	{
		_tv.tv_sec = usec / 1000000L;
		_tv.tv_usec = usec % 1000000L;
//        this->normalize();
	}

	inline void normalize()
	{
		this->_tv.tv_sec += this->_tv.tv_usec / 1000000L;
		this->_tv.tv_usec = this->_tv.tv_usec % 1000000L;
		if (this->_tv.tv_usec < 0) {
			this->_tv.tv_usec += 1000000L;
			this->_tv.tv_sec --;
		}
	}

	inline void now()
	{
		struct timezone tz;
		::gettimeofday(&_tv, &tz);
	}

	inline time_t sec() const
	{
		return _tv.tv_sec;
	}

	inline suseconds_t usec() const
	{
		return _tv.tv_usec;
	}

	inline long msec() const
	{
		return _tv.tv_sec * 1000L + _tv.tv_usec / 1000L;
	}

	inline bool operator <(const TimeValue & r)
	{
		return this->_tv.tv_sec < r._tv.tv_sec ?
				true :
				(this->_tv.tv_sec == r._tv.tv_sec ?
						this->_tv.tv_usec < r._tv.tv_usec : false);
	}

	inline bool operator >(const TimeValue & r)
	{
		return this->_tv.tv_sec > r._tv.tv_sec ?
				true :
				(this->_tv.tv_sec == r._tv.tv_sec ?
						this->_tv.tv_usec > r._tv.tv_usec : false);
	}

	inline bool operator <=(const TimeValue & r)
	{
		return this->_tv.tv_sec < r._tv.tv_sec ?
				true :
				(this->_tv.tv_sec == r._tv.tv_sec ?
						(this->_tv.tv_usec <= r._tv.tv_usec) : false);
	}

	inline bool operator >=(const TimeValue & r)
	{
		return this->_tv.tv_sec > r._tv.tv_sec ?
				true :
				(this->_tv.tv_sec == r._tv.tv_sec ?
						(this->_tv.tv_usec >= r._tv.tv_usec) : false);
	}

	inline bool operator ==(const TimeValue & r)
	{
		return (this->_tv.tv_sec == r._tv.tv_sec)
				&& (this->_tv.tv_usec == r._tv.tv_usec);
	}

	inline bool operator !=(const TimeValue & r)
	{
		return this->_tv.tv_sec != r._tv.tv_sec ?
				true : (this->_tv.tv_usec != r._tv.tv_usec);
	}

	inline static TimeValue getTimeOfDay()
	{
		TimeValue ret;
		struct timezone tz;
		::gettimeofday(&ret._tv, &tz);
		return ret;
	}

	inline TimeValue & operator +=(const TimeValue & r)
	{
		this->_tv.tv_sec += r._tv.tv_sec;
		this->_tv.tv_usec += r._tv.tv_usec;
		this->normalize();
		return *this;
	}

	inline TimeValue operator +(const TimeValue & r)
	{
		TimeValue ret;
		ret._tv.tv_sec = this->_tv.tv_sec + r._tv.tv_sec;
		ret._tv.tv_usec = this->_tv.tv_usec + r._tv.tv_usec;
		ret.normalize();
		return ret;
	}

	inline TimeValue & operator -=(const TimeValue & r)
	{
//		uint64_t mec = this->msec() - r.msec();
//		this->setMesc(mec);
        this->_tv.tv_sec -= r._tv.tv_sec;
        this->_tv.tv_usec -= r._tv.tv_usec;
        this->normalize();
		return *this;
	}

	inline TimeValue operator -(const TimeValue & r)
	{
//		uint64_t mec = this->msec() - r.msec();
		TimeValue ret;
//		ret.setMesc(mec);
        ret._tv.tv_sec = this->_tv.tv_sec - r._tv.tv_sec;
        ret._tv.tv_usec = this->_tv.tv_usec - r._tv.tv_usec;
        ret.normalize();
		return ret;
	}

	inline TimeValue & operator =(const TimeValue & r)
	{
		this->_tv.tv_sec = r._tv.tv_sec;
		this->_tv.tv_usec = r._tv.tv_usec;
		return *this;
	}

	struct timeval & tv()
	{
		return _tv;
	}

	static const TimeValue zero;
protected:
	struct timeval _tv;
};

#endif // __MCE_TIME_VALUE_H__
