
#ifndef __MCE_STDIN_HANDLER_H__
#define __MCE_STDIN_HANDLER_H__

#include <stdlib.h>
#include <stdio.h>
#include "EventHandlerBase.h"

class StdinHandler: public EventHandlerBase
{
public:
	StdinHandler(ReactorBase * sr)
			: EventHandlerBase(sr)
	{
	}

	virtual int open(void * a = 0)
	{
		EventHandlerBase::open(a);
		return this->reactor()->registerHandler(this,
				EventHandlerBase::EVENT_READ);
	}

	virtual int getHandle()
	{
		return STDIN_FILENO;
	}

//	virtual int handleInput(int ) = 0;

	virtual int handleOutput(int /*fd*/= -1)	{	return 0;	}

//	// fd 在Reactor 可能会用到，但是这里已经释放了。需要改进
//	virtual int handleClose(int /*fd*/= -1, int /*closeMask*/= EVENT_NULL)
//	{
//		delete this;
//		return 0;
//	}
};


#endif // __MCE_STDIN_HANDLER_H__
