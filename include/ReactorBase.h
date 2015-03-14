#ifndef __MCE_SOCKET_REACTOR_BASE_H__
#define __MCE_SOCKET_REACTOR_BASE_H__

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "EventHandlerBase.h"
#include "TimeValue.h"

class EventHandlerBase;

class TimeHandler;

class ReactorBase
{
public:
	virtual ~ReactorBase ()
    {
    }

	virtual int registerHandler (EventHandlerBase * handler, int mask) = 0;

	virtual int removeHandler (EventHandlerBase * handler, int callClose = 1) = 0;

	virtual int cancelWakeup (EventHandlerBase * handler, int mask) = 0;

	virtual int scheduleWakeup (EventHandlerBase * handler, int mask) = 0;

	virtual int scheduleTime (TimeHandler * th, const void * arg, const TimeValue & delay,
			const TimeValue & interval = TimeValue::zero) = 0;

	virtual bool cancelTime(TimeHandler * th) = 0;

	virtual int tick (long timeout) = 0;

	virtual int tick (const TimeValue & timeout) = 0;
};


#endif // __MCE_SOCKETREACTORBASE_H__

