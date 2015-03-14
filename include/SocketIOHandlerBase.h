#ifndef __MCE_SOCKET_IO_HANDLER_BASE_H__
#define __MCE_SOCKET_IO_HANDLER_BASE_H__

#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "EventHandlerBase.h"

#include "SocketIO.h"
class ReactorBase;

class SocketIOHandlerBase : public EventHandlerBase
{
public:
//	C_List_Node				_node;
//
//	enum SocketEvent
//	{
//		EVENT_NULL    	= 0x0000,
//		EVENT_READ		= 0x0001,
//		EVENT_WRITE		= 0x0002,
//	};

	SocketIOHandlerBase (ReactorBase * r) : EventHandlerBase (r)  /* _sr(sr), _mask (EVENT_NULL)*/
	{
	}

	virtual ~SocketIOHandlerBase ()
	{
	}

//	virtual int open () {return 0;}
//
//	virtual int handleInput (int fd = -1) 				= 0;
//
//	virtual int handleOutput (int fd = -1)				= 0;
//
//	virtual int handleClose (int fd = -1, int closeMask = EVENT_NULL)		= 0;

	virtual int getHandle () {return this->peer().fd ();};

//	inline ReactorBase * reactor ()
//
//	inline void reactor (ReactorBase * sr)
//
//	inline int mask ()
//
//	inline void mask (int mask)

	inline SocketIO & peer ()
	{
		return _socketIO;
	}

protected:
	SocketIO			_socketIO;
};


#endif // __MCE_SOCKET_IO_HANDLER_BASE_H__
