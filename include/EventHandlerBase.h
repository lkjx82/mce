#ifndef __MCE_EVENT_HANDLER_BASE_H__
#define __MCE_EVENT_HANDLER_BASE_H__

#include "c_list.h"

class ReactorBase;

class EventHandlerBase
{
public:
//	C_List_Node				_node;
	enum Event
	{
		EVENT_NULL    	= 0x0000,
		EVENT_READ		= 0x0001,
		EVENT_WRITE		= 0x0002,
		EVENT_SIG       = 0x0004,
		EVENT_ES   	    = 0x0008, // 异常
	};

	EventHandlerBase (ReactorBase * r) : _r(r), _mask (EVENT_NULL)
	{
	}

	virtual ~EventHandlerBase ()
	{
	}

	virtual int open (void * /*arg*/ = 0) {return 0;}

	virtual int handleInput (int fd = -1) 				= 0;

	virtual int handleOutput (int fd = -1)				= 0;

	virtual int handleClose (int fd = -1, int closeMask = EVENT_NULL)		= 0;

	virtual int handleSig (int fd = -1, int sig = 0)  {return 0;};

	virtual int getHandle () = 0;

	inline ReactorBase * reactor ()
	{
		return _r;
	}

	inline void reactor (ReactorBase * r)
	{
		_r = r;
	}

	inline int mask ()
	{
		return _mask;
	}

	inline void mask (int mask)
	{
		_mask = mask;
	}

protected:
	ReactorBase * _r;
	int					_mask;
};



#endif // __MCE_EVENT_HANDLER_BASE_H__
