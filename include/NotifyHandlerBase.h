#ifndef __MCE_NOTIFY_HANDLER_BASE_H__
#define __MCE_NOTIFY_HANDLER_BASE_H__

#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "EventHandlerBase.h"

class NotifyHandlerBase: public EventHandlerBase
{
public:
	NotifyHandlerBase(ReactorBase * r)
			: EventHandlerBase(r), _send(-1), _recv(-1)
	{
	}

	virtual int open(void * arg = 0)
	{
		EventHandlerBase::open(arg);

		int sockfd[2];
		if ((::socketpair(AF_UNIX, SOCK_STREAM, 0, sockfd)) == -1) {
			return -1;
		}

		_send = sockfd[0];
		_recv = sockfd[1];

		if (0 != fcntl(_recv, F_SETFL, O_NONBLOCK | fcntl(_recv, F_GETFL, 0))) {
			::close(_send);
			::close(_recv);
			_send = -1;
			_recv = -1;
			return -1;
		}
		return this->reactor()->registerHandler(this,
				EventHandlerBase::EVENT_READ);
	}

	void notify()
	{
		long val = 1;
		::send(_send, &val, sizeof (long), 0);
	}

	virtual int getHandle()
	{
		return _recv;
	}

	virtual int handleInput(int fd)
	{
		char buf[256];
		while (::recv(fd, buf, 256, 0) > 0)
			;
		return this->handleNotify(fd);
	}

	virtual int handleOutput(int fd)
	{
		return 0;
	}

	virtual int handleClose(int fd, int mask)
	{
		if (-1 != _send) {
			::close(_send);
			_send = -1;
		}

		if (-1 != _recv) {
			::close(_recv);
			_recv = -1;
		}
		return 0;
	}

	virtual int handleNotify(int fd)
	{
		return 0;
	}

private:
	int _send;
	int _recv;
};

#include "MessageQueue.h"

class AsynchHandlerBase: NotifyHandlerBase
{
public:
	AsynchHandlerBase(ReactorBase * r)
			: NotifyHandlerBase(r)
	{
	}

	void asynchMessage(MessageBlock * mb)
	{
		_tq.putq(mb);
		notify();
	}

	virtual int handleAsynch(MessageBlock * mb)
	{
		mb->release();
		return 0;
	}

	virtual int handleClose(int fd, int mask)
	{
		NotifyHandlerBase::handleClose(fd, mask);
		return 0;
	}

	virtual int handleNotify(int fd)
	{
		while (_tq.size()) {
			MessageBlock * mb = _tq.getq();
			handleAsynch(mb);
		}
		return 0;
	}

protected:
	NoBlockMessageQueue _tq;
};

#endif // __MCE_NOTIFY_HANDLER_BASE_H__
