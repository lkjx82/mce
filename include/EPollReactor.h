#ifndef __MCE_SOCKET_EPOLL_REACTOR_H__
#define __MCE_SOCKET_EPOLL_REACTOR_H__

#include "EventHandlerBase.h"
#include "ReactorBase.h"
#include <unordered_map>

class TimeQueue;

class EPollReactor: public ReactorBase
{
public:
	EPollReactor(int fdMax = 65535, TimeQueue * tq = 0);

    virtual ~EPollReactor();

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

    HandlerMap _shs;
    volatile int _fdMax;
    struct epoll_event *_evts;
    int _epFd;
    TimeQueue * _tq;
    bool 		_tqDel;
};


#endif // __MCE_SOCKET_EPOLL_REACTOR_H__
