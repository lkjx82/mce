#ifndef __MCE_TIME_QUEUE_H__
#define __MCE_TIME_QUEUE_H__

#include "TimeHandler.h"
#include "MinHeap.h"

class TimeQueue
{
public:
	TimeQueue()
			: _mp(1024)
	{
	}

	bool schedule(TimeHandler * th, const void * arg, const TimeValue & delay,
			const TimeValue & interval = TimeValue::zero)
	{
		TimeValue tv;
		tv.now();
		th->arg ((void *)arg);
		th->interval () = interval;
		th->elapseTime () = tv + delay;
		return _mp.insert(th);
	}

	bool cancel(TimeHandler * th)
	{
		TimeHandler * ret = 0;
//        printf ("cancel pos %d\n", tn->pos ());
		if (_mp.remove(th->pos(), &ret)) {
			th->pos(-1);
			return true;
		}
		else {
//            printf ("erase fail %d\n", tn->pos ());
			return false;
		}
	}

	int elapse(const TimeValue & tv = TimeValue::getTimeOfDay())
	{
		TimeHandler * th = _mp.min();
		while (th) {
			if (th->elapseTime () <= tv) {
				_mp.removeMin();
				th->pos (-1);
				if ((th->handleTimeout(tv, th->arg ()) == 0)
						&& (th->interval () != TimeValue::zero)) {
					th->elapseTime () = th->interval () + tv;
					_mp.insert(th);
				}
				th = _mp.min();
				continue;
			}
			else {
				break;
			}
		}
		return 0;
	}

	inline bool getSoonTime(TimeValue & tv)
	{
		TimeHandler * th = _mp.min();
		if (th) {
			tv = th->elapseTime ();
			return true;
		}
		return false;
	}

protected:
	MinHeap<TimeHandler> _mp;
};

#endif // __MCE_TIME_QUEUE_H__
