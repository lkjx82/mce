// ----------------------------------------------------------------------------

#include "../include/ReactorBase.h"
#include "../include/AsynConnector.h"
#include "../include/NotifyHandlerBase.h"

// ----------------------------------------------------------------------------

#include "../include/TimeHandler.h"

// ----------------------------------------------------------------------------

class AsynConnectTimer: public TimeHandler
{
public:

	AsynConnectTimer()
	{
	}

	virtual int handleTimeout(const TimeValue & tv, void * arg)
	{
		return -1;
	}
};

// ----------------------------------------------------------------------------

class AsynConnectNofity: public NotifyHandlerBase
{
public:
	AsynConnectNofity(EventHandlerBase * h, bool connected, ReactorBase * r)
			: NotifyHandlerBase(r), _h(h), _connected(connected)
	{
	}

	virtual int handleNotify(int fd)
	{
		if (_connected && (0 == _h->open(0))) {
			return -1;
		}

		_h->handleClose(_h->getHandle(), 0);
		return -1;
	}

	virtual int handleClose(int fd, int flag)
	{
		NotifyHandlerBase::handleClose(fd, flag);
		delete this;
		return 0;
	}

	EventHandlerBase * _h;
	bool _connected;
};

// ----------------------------------------------------------------------------

AsynConnector::AsynConnector(ReactorBase * r)
		: EventHandlerBase(r), _sh(0), _sockOptSave(0),
//		  _connSucc (false),
		_connFd(-1)
{
}

// ----------------------------------------------------------------------------

int AsynConnector::getHandle()
{
	return _connFd;
}

// ----------------------------------------------------------------------------

bool AsynConnector::connect(SocketIOHandlerBase * sh, const InetAddr & addr,
		int timeout)
{
	_sh = sh;
	_connFd = _sh->peer().fd();

	// 设置成非阻塞
	_sockOptSave = fcntl(_sh->peer().fd(), F_GETFL);
	fcntl(_sh->peer().fd(), F_SETFL, _sockOptSave | O_NONBLOCK);

	int ret = _sh->peer().connect(addr);

	// 直接连接成功了。
	if (0 == ret) {
		this->_asynNotifyRlt(true);
		this->handleClose(-1, 0);
		return true;
	}

	// 连接彻底失败
	if (-1 == ret && EINPROGRESS != errno) {
		this->_asynNotifyRlt(false);
		this->handleClose(-1, 0);
		return false;
	}

	// 没有超时值，就用 Reactor 检测
//	if (0 == timeout) {
	if (0
			!= this->reactor()->registerHandler(this,
					EventHandlerBase::EVENT_READ
							| EventHandlerBase::EVENT_WRITE)) {
		this->_asynNotifyRlt(false);
		this->handleClose(-1, 0);
		return false;
	}
//	}

//	// 有超时值， 用Timeout检测？
//	else {
//
//	}

	return true;
}

// ----------------------------------------------------------------------------

int AsynConnector::handleInput(int fd)
{
	this->reactor()->removeHandler(this, 0);

	this->_asynNotifyRlt(_checkErrorOK(fd));
	return -1;
}

// ----------------------------------------------------------------------------

int AsynConnector::handleOutput(int fd)
{
	this->reactor()->removeHandler(this, 0);

	this->_asynNotifyRlt(_checkErrorOK(fd));
	return -1;
}

// ----------------------------------------------------------------------------

int AsynConnector::handleClose(int /*fd*/, int /*closeMask*/)
{
	delete this;
	return 0;
}

// ----------------------------------------------------------------------------

bool AsynConnector::_checkErrorOK(int fd)
{
	// 事件包含错误
	int n = 0;
	int len = sizeof(int);
	if (::getsockopt(fd, SOL_SOCKET, SO_ERROR, (char *) &n, (socklen_t *) &len)
			< 0 || (0 != n)) {
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------

bool AsynConnector::_asynNotifyRlt(bool connected)
{
	fcntl(_sh->peer().fd(), F_SETFL, _sockOptSave);

	AsynConnectNofity * cn = new AsynConnectNofity(_sh, connected,
			this->reactor());
	cn->open(0);
	cn->notify();

	return true;
}

// ----------------------------------------------------------------------------

bool AsynConnector::isComplete()
{
	int fd = _sh->peer().fd();

	// 检测
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET(fd, &rset);
	FD_SET(fd, &wset);

	// 阻塞 timeout
	int ret = ::select(fd + 1, &rset, &wset, 0, 0);

	// 错误或者超时
	if (0 >= ret) {
		_sh->handleClose(fd, 0);
		return false;
	}

	// 检测到事件
	if (FD_ISSET (fd, &rset) || FD_ISSET (fd, &wset)) {
		// 事件包含错误
		if (!_checkErrorOK(fd)) {
			if (EINPROGRESS == errno) {
				return false;
			}
		}
		return true;
	}
	else {
		return false;
	}
}

// ----------------------------------------------------------------------------
