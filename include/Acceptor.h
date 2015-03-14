#ifndef __MCE_ACCEPTOR_H__
#define __MCE_ACCEPTOR_H__

#include "SocketIOHandlerBase.h"
#include <assert.h>

template<typename HandlerT>
class Acceptor: public SocketIOHandlerBase
{
public:
	Acceptor(ReactorBase *sr)
			: SocketIOHandlerBase(sr)
	{
	}

	bool open(const InetAddr & addr, int backlog = SOMAXCONN)
	{
		if (0 != this->peer().open(SocketIO::SOCKTYPE_TCP)) {
			return false;
		}

		if (0 != this->peer ().setOption (SocketIO::SOCKOPT_REUSEADDR, 1)) {
			return false;
		}

		if (0 != this->peer().bind(addr)) {
			this->peer().close();
			return false;
		}

		if (0 != this->peer().listen(backlog)) {
			this->peer().close();
			return false;
		}

		if (0 != this->peer().setOption(SocketIO::SOCKOPT_NONBLOCK, 1)) {
			this->peer().close();
			return false;
		}

		if (0 != this->reactor()->registerHandler(this,
						SocketIOHandlerBase::EVENT_READ)) {
			this->peer().close();
			return false;
		}
		return true;
	}

	virtual int handleInput(int /*fd*/)
	{
		do {
			InetAddr addrFrom;
			int fdNew = this->peer().accept(addrFrom);
			if (fdNew > 0) {
				SocketIOHandlerBase * sh = 0;

				if (makeHandler(sh)) {
					sh->peer().fd(fdNew);
					sh->peer().type(SocketIO::SOCKTYPE_TCP);
					// 允许连接
					if ((0 == this->acceptHandle(sh, addrFrom)) && (0 == sh->open())) {
						continue;
					}
					// 不允许连接
					else {
						sh->handleClose();
						continue;
					}
				}
				// 建立Handler 失败
				else {
					::close(fdNew);
					continue;
				}
			}
			// 所有的进来的连都处理完
			else if (-1 == fdNew) {
				if (EWOULDBLOCK == errno || EINTR == errno) {
					return 0;
				}
			}
			else {
				printf("accept error %s \n", strerror(errno));
				return 0;
			}
		} while (1);
		return 0;
	}

	virtual bool makeHandler(SocketIOHandlerBase *& sh)
	{
		sh = new HandlerT(this->reactor());
		if (sh) {
			return true;
		}
		return false;
	}

	virtual int acceptHandle(SocketIOHandlerBase * sh, const InetAddr & addr)
	{
		return 0;
	}

	virtual int handleOutput(int /*fd*/)
	{
		return 0;
	}

	virtual int handleClose(int /*fd*/, int /*closeMask*/)
	{
		this->peer ().close ();
		return 0;
	}
};

#endif /* __MCE_ACCEPTOR_H__ */
