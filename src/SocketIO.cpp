#include "../include/SocketIO.h"

int SocketIO::open(SocketIO::SocketType type)
{
    _socket = ::socket(PF_INET, SOCKTYPE_TCP == type ? SOCK_STREAM : SOCK_DGRAM,
            0);
    //SOCKTYPE_TCP == type ? IPPROTO_TCP : IPPROTO_UDP);
    if (-1 == _socket) {
        return -1;
    }
    if (this->setOption(SOCKOPT_REUSEADDR, 1) != 0) {
        return -1;
    }

    this->_type = type;

    return 0;
}

int SocketIO::accept(SocketIO & sockIO, InetAddr & addr)
{
    sockaddr_in addrIn;
    unsigned int addrLen = sizeof(struct sockaddr_in);
    int f = ::accept(_socket, (struct sockaddr *) &addrIn, &addrLen);
    if (-1 == f) {
        return -1;
    }
    sockIO.fd(f);
    sockIO.type(SOCKTYPE_TCP);
    addr.set(&addrIn, addrLen);
    return 0;
}

int SocketIO::accept(InetAddr & addr)
{
    sockaddr_in addrIn;
    unsigned int addrLen = sizeof(struct sockaddr_in);
    int fd = ::accept(_socket, (struct sockaddr *) &addrIn, &addrLen);
    addr.set(&addrIn, addrLen);
    return fd;
}

ssize_t SocketIO::recvFrom(void * buf, size_t len, InetAddr * addr, int flags)
{
    struct sockaddr_in addrIn;
    socklen_t addrSize = sizeof(struct sockaddr_in);
    ssize_t ret = ::recvfrom(_socket, buf, len, flags,
            (struct sockaddr *) &addrIn, &addrSize);
    if (ret >= 0 && addr) {
        addr->set(&addrIn, addrSize);
    }
    return ret;
}

int SocketIO::setOption(SocketIO::SocketOption opt, int val)
{
    int ret = -1;
    switch (opt)
    {
        case SOCKOPT_NONBLOCK:
            if (val) {
                ret = fcntl (_socket, F_SETFL, O_NONBLOCK | fcntl (_socket, F_GETFL, 0));
            }
            else {
                ret = fcntl (_socket, F_SETFL, ~O_NONBLOCK & fcntl (_socket, F_GETFL, 0));
            }
//#ifdef HAS_FCNTL
//#else
//            ret = ioctl(_socket, FIONBIO, &val);
//#endif // HAS_FCNTL
            break;

        case SOCKOPT_BROADCAST:
            ret = setsockopt(_socket, SOL_SOCKET, SO_BROADCAST, (char *) &val,
                    sizeof(int));
            break;

        case SOCKOPT_RCVBUF:
            ret = setsockopt(_socket, SOL_SOCKET, SO_RCVBUF, (char *) &val,
                    sizeof(int));
            break;

        case SOCKOPT_SNDBUF:
            ret = setsockopt(_socket, SOL_SOCKET, SO_SNDBUF, (char *) &val,
                    sizeof(int));
            break;
        case SOCKOPT_REUSEADDR:
            ret = setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, (char *) &val,
                    sizeof(int));
            break;

        case SOCKOPT_NODELAY:
            ret = setsockopt(_socket, IPPROTO_TCP, TCP_NODELAY, (char *) &val,
                    sizeof(int));
            break;

    }
    return ret;
}
