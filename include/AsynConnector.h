#ifndef __MCE_ASYN_CONNECTOR_H__
#define __MCE_ASYN_CONNECTOR_H__

// ----------------------------------------------------------------------------

#include "SocketIOHandlerBase.h"
#include "EventHandlerBase.h"

// ----------------------------------------------------------------------------

class AsynConnector: public EventHandlerBase
{
public:
	AsynConnector(ReactorBase * r);

	bool connect (SocketIOHandlerBase * sh, const InetAddr & addr, int timeout);

	int getHandle ();

	int handleInput (int);

	int handleOutput (int /*fd*/);

	virtual int handleClose (int /*fd*/, int /*closeMask*/);

	bool isComplete ();

protected:
	bool _asynNotifyRlt (bool connected);
	bool _checkErrorOK (int fd);
	SocketIOHandlerBase * _sh;
	int _sockOptSave;
	int  _connFd;
};

// ----------------------------------------------------------------------------

#endif // __MCE_ASYN_CONNECTOR_H__

