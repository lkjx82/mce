#ifndef __MCE_SOCKET_SELECT_REACTOR_H__
#define __MCE_SOCKET_SELECT_REACTOR_H__

#include "EventHandlerBase.h"
#include "ReactorBase.h"
#include <unordered_map>

class TimeQueue;

class SelectReactor: public ReactorBase
{
public:
    SelectReactor(int fdMax = 65535, TimeQueue * tq = 0);

    virtual ~SelectReactor();

    virtual int registerHandler(EventHandlerBase * handler, int mask);

    virtual int removeHandler(EventHandlerBase * handler, int callClose =
            1);
    virtual int cancelWakeup(EventHandlerBase * handler, int mask);

    virtual int scheduleWakeup(EventHandlerBase * handler, int mask);

	virtual int scheduleTime (TimeHandler * th, const void * arg, const TimeValue & delay,
			const TimeValue & interval = TimeValue::zero);

	virtual bool cancelTime(TimeHandler * th);

    virtual int tick(long timeout);

    virtual int tick (const TimeValue & timeout);

protected:
    typedef std::unordered_map<int, EventHandlerBase *> HandlerMap;
    void _calcMaxFd();

    HandlerMap _shs;
    volatile int _maxFd;
    TimeQueue * _tq;
    bool 		_tqDel;
    int 		_maxFdCnt;
    int			*_fds;
};

#endif // __MCE_SOCKETSELECTREACTOR_H__
