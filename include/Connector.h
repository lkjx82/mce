#ifndef __MCE_CONNECTOR_H__
#define __MCE_CONNECTOR_H__

// ----------------------------------------------------------------------------

#include "SocketIOHandlerBase.h"
#include "EventHandlerBase.h"
#include "AsynConnector.h"

// ----------------------------------------------------------------------------

template<typename HandlerT>
class Connector  : public EventHandlerBase
{
public:
	Connector(ReactorBase *sr)
			: EventHandlerBase(sr)
	{
	}

	virtual ~Connector ()
	{
	}

	// asyn 为false， timeout 为连接超时时间，asyn 为 true， timeout 无效。
	bool connect (const InetAddr & addr, int timeout = 0, bool asyn = false);

	bool connect (SocketIOHandlerBase * sh, const InetAddr & addr, int timeout = 0, bool asyn = false);

	virtual bool makeHandler(SocketIOHandlerBase *& sh);

	virtual int handleInput(int /*fd*/)
	{
		return 0;
	}

	virtual int handleOutput(int /*fd*/)
	{
		return 0;
	}

	virtual int handleClose(int /*fd*/, int /*closeMask*/)
	{
		return 0;
	}

	int getHandle ()
	{
		return -1;
	}

protected:
	// 执行异步连接
	bool _connect_asyn_i(SocketIOHandlerBase * sh, const InetAddr & addr,
			int timeout);
	// 执行非阻塞同步连接，如果timeout为0，则为阻塞连接
	bool _connect_synch_i(SocketIOHandlerBase * sh, const InetAddr & addr,
			int timeout);
};

// ----------------------------------------------------------------------------

template<typename HandlerT>
bool Connector<HandlerT>::connect(const InetAddr & addr, int timeout, bool asyn)
{
	SocketIOHandlerBase * sh = 0;
	if (!makeHandler(sh)) {
		return false;
	}

	if (0 != sh->peer().open(SocketIO::SOCKTYPE_TCP)) {
		sh->handleClose(sh->peer().fd(), 0);
		return false;
	}

	return asyn ? _connect_asyn_i(sh, addr, timeout) : _connect_synch_i(sh, addr, timeout);
}

// ----------------------------------------------------------------------------

template<typename HandlerT>
bool Connector<HandlerT>::connect(SocketIOHandlerBase * sh, const InetAddr & addr, int timeout, bool asyn)
{
	if (!sh && !makeHandler(sh)) {
		return false;
	}

	if (0 != sh->peer().open(SocketIO::SOCKTYPE_TCP)) {
		sh->handleClose(sh->peer().fd(), 0);
		return false;
	}

	return asyn ? _connect_asyn_i(sh, addr, timeout) : _connect_synch_i(sh, addr, timeout);
}

// ----------------------------------------------------------------------------

template<typename HandlerT>
bool Connector<HandlerT>::makeHandler(SocketIOHandlerBase *& sh)
{
	sh = new HandlerT (this->reactor());
	if (sh) {
		return true;
	}
	return false;
}

// ----------------------------------------------------------------------------
// 异步连接
template<typename HandlerT>
bool Connector<HandlerT>::_connect_asyn_i(SocketIOHandlerBase * sh,
		const InetAddr & addr, int timeout)
{
	// 建立 AsynConnector，并且把 AsynConnector 注册到 Reactor 中去
	AsynConnector * ac = new AsynConnector(this->reactor());
	return ac->connect (sh, addr, timeout);
}

// ----------------------------------------------------------------------------
// 同步连接
template<typename HandlerT>
bool Connector<HandlerT>::_connect_synch_i(SocketIOHandlerBase * sh,
		const InetAddr & addr, int timeout)
{
	int sockOptSave = fcntl(sh->peer().fd(), F_GETFL);

	// 未设置超时， timeout = 0
	if (0 == timeout) {
		// 使用阻塞模式
		fcntl(sh->peer().fd(), F_SETFL, sockOptSave & ~O_NONBLOCK);

		if (0 == sh->peer().connect (addr) && 0 == sh->open()) {
			fcntl(sh->peer().fd(), F_SETFL, sockOptSave);
			return true;
		}
		else {
			fcntl(sh->peer().fd(), F_SETFL, sockOptSave);
			sh->handleClose(sh->peer().fd(), 0);
			return false;
		}
	}

	// 设置了超时，开启非阻塞模式
	if (0 != fcntl(sh->peer().fd(), F_SETFL, sockOptSave | O_NONBLOCK)) {
		sh->handleClose (sh->peer().fd(), 0);
		return false;
	}

	int ret = sh->peer().connect(addr);

	// 直接连接成功了。
	if (0 == ret) {
		fcntl(sh->peer().fd(), F_SETFL, sockOptSave);
		if (0 != sh->open()) {
			sh->handleClose(sh->peer().fd(), 0);
			return false;
		}
		return true;
	}

	// 连接失败
	if (-1 == ret && EINPROGRESS != errno) {
		fcntl(sh->peer().fd(), F_SETFL, sockOptSave);
		sh->handleClose (sh->peer().fd(), 0);
		return false;
	}

	// 连接中....
	fd_set rset, wset;
	FD_ZERO(&rset);
	FD_ZERO(&wset);
	FD_SET (sh->peer ().fd (), &rset);
	FD_SET (sh->peer ().fd (), &wset);

	struct timeval tv;
	tv.tv_sec = timeout;
	tv.tv_usec = 0;

	// 阻塞 timeout
	ret = ::select(sh->peer().fd() + 1, &rset, &wset, 0, timeout ? &tv : 0);

	// 恢复 老的 socket 选项
	fcntl(sh->peer().fd(), F_SETFL, sockOptSave);

	// 错误或者超时
	if (ret <= 0) {
		sh->handleClose(sh->peer().fd(), 0);
		return false;
	}

	// 检测到事件
	if (FD_ISSET (sh->peer ().fd (), &rset)
			|| FD_ISSET (sh->peer ().fd (), &wset)) {

		// 事件包含错误
		int n = 0;
		int len = sizeof(int);
		if (::getsockopt(sh->peer().fd(), SOL_SOCKET, SO_ERROR, (char *) &n,
				(socklen_t *) &len) < 0 || (0 != n)) {
			sh->handleClose(sh->peer().fd(), 0);
			return false;
		}

		// 没有错误，都成功了
		if (sh->open() < 0) {
			sh->handleClose(sh->peer().fd(), 0);
			return false;
		}
		return true;
	}

	return false;
}

// ----------------------------------------------------------------------------

#include "../src/Connector.inl"

// ----------------------------------------------------------------------------

#endif // __MCE_CONNECTOR_H__
