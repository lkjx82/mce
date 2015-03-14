#ifndef __MCE_TIME_HANDLER_H__
#define __MCE_TIME_HANDLER_H__

#include "TimeValue.h"

class TimeHandler
{
public:
	TimeHandler()
			: _pos(-1), _arg(0)
	{
	}

	virtual ~TimeHandler()
	{
	}

	virtual int handleTimeout(const TimeValue & tv, void * arg) = 0;

	inline bool operator >(const TimeHandler & tn)
	{
		return this->_elapseTime > tn._elapseTime;
	}

	inline int pos()
	{
		return _pos;
	}

	inline void pos(int p)
	{
		_pos = p;
	}

	inline TimeValue & elapseTime ()
	{
		return _elapseTime;
	}

	inline void * arg ()
	{
		return _arg;
	}

	inline void arg (void * a)
	{
		_arg = a;
	}

	TimeValue & interval ()
	{
		return _interval;
	}

protected:

	int _pos;
	void *_arg;
	TimeValue _elapseTime;
	TimeValue _interval;
};


#endif // __MCE_TIME_HANDLER_H__

