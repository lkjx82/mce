#ifndef __MCE_SOCKETIO_H__
#define __MCE_SOCKETIO_H__

#include <unistd.h>
#include <netinet/tcp.h>
#include <errno.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/ioctl.h>

#include "InetAddr.h"


class SocketIO
{
public:

	enum SocketOption
	{
		SOCKOPT_NONBLOCK = 1,
		SOCKOPT_BROADCAST,
		SOCKOPT_RCVBUF,
		SOCKOPT_SNDBUF,
		SOCKOPT_NODELAY,
		SOCKOPT_REUSEADDR,
	};

	enum
	{
		HOST_ANY = 0,
		HOST_BROADCAST = 0xFFFFFFFF,
		PORT_ANY = 0,
	};

	enum SocketType
	{
		SOCKTYPE_TCP,
		SOCKTYPE_UDP,
	};

	SocketIO ()
	{
		_socket = -1;
		_type   = SOCKTYPE_TCP;
	};

	~SocketIO ()
	{
		if (-1 != _socket) {
			this->close ();
		}
	};

	int open (SocketType type = SOCKTYPE_TCP);

	inline int bind (const InetAddr & addr)
	{
		if (::bind (_socket, (struct sockaddr *)addr.addrPtr (), addr.addrSize ()) == -1) {
			return -1;
		}
		return 0;
	}

	inline int connect (const InetAddr & addr)
	{
		return ::connect (_socket, (struct sockaddr *) addr.addrPtr (), addr.addrSize ());
	}

	inline int listen (int backlog = 5)
	{
		return ::listen (_socket, backlog);
	}

	int accept (SocketIO & sockIO, InetAddr & addr);

	int accept (InetAddr & addr);

	inline void close ()
	{
	    errno = 0;
//        int ret =
        		::close(_socket);
//        printf("[close error]ret = %d fd %d, %s\n", ret, _socket, strerror(errno));

		_socket = -1;
	}

	int setOption (SocketOption opt, int val);

	inline ssize_t recv (void * buf, size_t len, int flags = 0)
	{
		return ::recv (_socket, buf, len, flags);
	}

	inline ssize_t send (const void * buf, size_t len, int flags = 0)
	{
		return ::send (_socket, buf, len, flags);
	}

	ssize_t recvFrom (void * buf, size_t len, InetAddr * addr = 0, int flags = 0);

	inline ssize_t sendTo (void * buf, size_t len, InetAddr & addr, int flags = 0)
	{
		return ::sendto (_socket, buf, len, flags, addr.addrPtr (), addr.addrSize ());
	}

	inline int & fd ()
	{
		return _socket;
	}

	inline void fd (int f)
	{
		_socket = f;
	}

	inline SocketType type ()
	{
		return _type;
	}

	inline void type (SocketType tp)
	{
		_type = tp;
	}

protected:
	int 		_socket;
	SocketType	_type;

};


#endif // __MCE_NETWORK_SOCKETIO_H__

