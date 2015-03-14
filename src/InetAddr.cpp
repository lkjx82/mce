#include "../include/InetAddr.h"

// -----------------------------------------------------------------------------

InetAddr::InetAddr ()
{
    memset (&_sin, 0, sizeof _sin);
}

// -----------------------------------------------------------------------------

InetAddr::InetAddr (const InetAddr & rt)
{
    memset (&_sin, 0, sizeof _sin);
    memcpy (&this->_sin, &rt._sin, sizeof (struct sockaddr_in));
}

// -----------------------------------------------------------------------------

InetAddr::InetAddr (const sockaddr_in * addr, size_t size)
{
    memset (&_sin, 0, sizeof _sin);
    memcpy (&this->_sin, &addr, size);
}

// -----------------------------------------------------------------------------

InetAddr::InetAddr (unsigned short port, unsigned long host)
{
    this->set (port, host);
}

// -----------------------------------------------------------------------------

InetAddr::InetAddr (unsigned short port, const char * host)
{
    this->set (port, host);
}

// -----------------------------------------------------------------------------

InetAddr::InetAddr (const char * addr)
{
    this->set (addr);
}

// -----------------------------------------------------------------------------

// Inet_Addr::~Inet_Addr ()
// {
// }

// -----------------------------------------------------------------------------

int InetAddr::set (unsigned short port)
{
    memset (&_sin, 0, sizeof (struct sockaddr_in));
    _sin.sin_family = AF_INET;

    _sin.sin_port = htons (port);
    _sin.sin_addr.s_addr = INADDR_ANY;
    return 0;
}

// -----------------------------------------------------------------------------

int InetAddr::set (const char * addr)
{
    //memset (&_sin, 0, sizeof (struct sockaddr_in));
    //_sin.sin_family = AF_INET;
    memset (&_sin, 0, sizeof _sin);
    return fromString (addr);
}

// -----------------------------------------------------------------------------

int InetAddr::set (unsigned short port, unsigned long host)
{
    memset (&_sin, 0, sizeof (struct sockaddr_in));
    _sin.sin_family = AF_INET;

    _sin.sin_port = htons (port);
    _sin.sin_addr.s_addr = host;
    return 0;
}

// -----------------------------------------------------------------------------

//int Inet_Addr::set2 (unsigned short port, char * host)
//{
//    if (0 == host) {
//        return -1;
//    }
//
//    memset (&_sin, 0, sizeof (struct sockaddr_in));
//    _sin.sin_family = AF_INET;
//
//#if defined (WIN32)
//    unsigned long addr = inet_addr (host);
//        
//    if (INADDR_NONE != addr) {
//        _sin.sin_addr.s_addr = addr;
//        _sin.sin_port = htons (port);
//        return 0;
//    }
//    
//#elif defined (LINUX)
//
//    // ip str
//    if (inet_aton (host, &(_sin.sin_addr)) != 0) {
//        _sin.sin_port = htons (port);
//        return 0;
//    }
//
//#endif 
//
//    // hostname str
//    struct hostent * hent = 0;
//    hent = gethostbyname (host);
//    if (0 == hent || AF_INET != hent->h_addrtype) {
//        return -1;
//    }
//
//    memcpy (&_sin.sin_addr.s_addr, &hent->h_addr, hent->h_length);
//    _sin.sin_port = htons (port);
//    return 0;
//}

// -----------------------------------------------------------------------------

int InetAddr::set (unsigned short port, const char * host)
{
    if (0 == host) {
        return -1;
    }

    memset (&_sin, 0, sizeof (struct sockaddr_in));
    _sin.sin_family = AF_INET;

    struct hostent * host_ent = 0;

#if defined (WIN32)

    host_ent = gethostbyname (host);
    if (0 == host_ent || AF_INET != host_ent->h_addrtype) {

        unsigned long addr = inet_addr (host);
        if (INADDR_NONE == addr) {
            return -1;
        }
        _sin.sin_addr.s_addr = addr;
        _sin.sin_port = htons (port);
        return 0;
    }

#endif

#if defined (__linux__)

    // gethostbyname_r ���̰߳�ȫ�� gethostbyname
#ifdef HAS_GETHOSTBYNAME_R
    struct hostent host_data;
    char buff[2048];
    int errnum;

#  if defined (__linux__) || defined (__FreeBSD__)
    gethostbyname_r (host, &host_data, buff, sizeof buff, &host_ent, &errnum);
#  else // ! linux && ! freebsd
    host_ent = gethostbyname_r (host, &host_data, buff, sizeof buff, &errnum);
#  endif // linux, freebsd

#else // ! HAS_GETHOSTBYNAME_R
    host_ent = gethostbyname (host);
#endif // HAS_GETHOSTBYNAME_R

    if (0 == host_ent || AF_INET != host_ent->h_addrtype) {
        // inet_pton�ǱȽ��µģ����inet_addr�ĺ���֧��IPV4��IPV6
#if defined (HAS_INET_PTON)
        if (!inet_pton (AF_INET, host, &_sin.sin_addr.s_addr)) {
#else // ! HAS_INET_PTON
        if (!inet_aton (host, (struct in_addr *) &_sin.sin_addr.s_addr)) {
#endif // HAS_INET_PTON
            return -1;
        }
        return 0;
    }

#endif

    _sin.sin_port = htons (port);
    _sin.sin_addr.s_addr = * ((unsigned long *) host_ent->h_addr_list[0]);
    return 0;
}

// -----------------------------------------------------------------------------

void InetAddr::portNumber (unsigned short port)
{
    _sin.sin_port = htons (port);
}

// -----------------------------------------------------------------------------

unsigned short InetAddr::portNumber ()
{
    return (AF_INET == _sin.sin_family ? ntohs (_sin.sin_port) : 0);
}

// -----------------------------------------------------------------------------

unsigned long InetAddr::ipNumber ()
{
    return (AF_INET == _sin.sin_family ? ntohl (_sin.sin_addr.s_addr) : 0);
}

// -----------------------------------------------------------------------------

void InetAddr::ipNumber (unsigned long ip)
{
    _sin.sin_addr.s_addr = htonl (ip);
}

// -----------------------------------------------------------------------------

int InetAddr::isLoopback ()
{
    // IPv4
    // RFC 3330 defines loopback as any address with 127.x.x.x
    return ((this->ipNumber() & 0XFF000000) == (INADDR_LOOPBACK & 0XFF000000));
}

// -----------------------------------------------------------------------------

int InetAddr::isMulticast ()
{
    // IPv4
    return this->_sin.sin_addr.s_addr >= 0xE0000000 &&  // 224.0.0.0
        this->_sin.sin_addr.s_addr <= 0xEFFFFFFF;   // 239.255.255.255
}

// -----------------------------------------------------------------------------

// get str hostname
int InetAddr::getHostName (char * hostname, size_t len)
{
    if (!hostname || !len) {
        return -1;
    }

    struct hostent * host_ent = 0;

#if defined (WIN32)
    if (INADDR_ANY == this->_sin.sin_addr.s_addr) {
        return ::gethostname (hostname, len);
    }
#endif


#ifdef HAS_GETHOSTBYADDR_R

    struct hostent host_data;
    char buff[2048];
    int errnum;

#  if defined (LINUX) || defined (__FreeBSD__)
    gethostbyaddr_r ((char *) &_sin.sin_addr /*in*/, sizeof (struct in_addr), AF_INET, &host_data, buff, sizeof buff, &host_ent, &errnum);
#  else // ! linux && ! freebsd
    host_ent = gethostbyaddr_r ((char *) &_sin.sin_addr /*in*/, sizeof (struct in_addr), AF_INET, &host_data, buff, sizeof buff, &errnum);
#  endif // linux && freebsd

#else // ! HAS_GETHOSTBYADDR_R
    host_ent = gethostbyaddr ((char *) &_sin.sin_addr/*in*/, sizeof (struct in_addr), AF_INET);
#endif

    if (host_ent) {
        if (::strlen (host_ent->h_name) >= len) {
            ::memcpy (hostname, host_ent->h_name, len - 1);
            hostname[len-1] = '\0';
        }
        else {
            ::strncpy (hostname, host_ent->h_name, len);
        }
        return 0;
    }

    return this->getHostAddr (hostname, len);
}

// -----------------------------------------------------------------------------

// �����ַ���ʽ��ip��ַ
int InetAddr::getHostAddr (char * addr, size_t addr_size)
{
    //char * ch = ::inet_ntoa (_sin.sin_addr);
    //strncpy (addr, ch, addr_size);
    //return ch;

//#if defined (LINUX)
#if defined (HAS_INET_PTON)

    if (inet_ntop (AF_INET, &_sin.sin_addr, addr, addr_size) == 0) {
        return -1;
    }

#else
    char * ch = inet_ntoa (_sin.sin_addr);
    if (!ch) {
        return -1;    
    }
    
    strncpy (addr, ch, addr_size);
#endif

    return 0;
}

// -----------------------------------------------------------------------------

int InetAddr::toString (char * buf, size_t buf_size, int ip_format)
{
    char tmp[MAX_HOSTNAME_LEN + 1];
    memset (tmp, 0, MAX_HOSTNAME_LEN + 1);

    if (!buf || 0 == buf_size) {
        return -1;
    }

    int rlt;

    if (ip_format) {
        rlt = (this->getHostAddr (tmp, MAX_HOSTNAME_LEN) != 0);
    }
    else { // default
        rlt = (this->getHostName (tmp, MAX_HOSTNAME_LEN) != 0);
    }

    size_t total_len = 
        strlen (tmp)
        + 5  // strlen ("65535")
        + 1  // sizeof ":"
        + 1; // sizeof '\0'

    if (buf_size < total_len) {
        return -1;
    }

    sprintf (buf, "%s:%d", tmp, this->portNumber ());
    return rlt;
}

// -----------------------------------------------------------------------------

int InetAddr::fromString (const char * addr_str)
{
    int rlt   = -1;
    char addr_tmp[32] = {0};  
    strncpy (addr_tmp, addr_str, 32);
    char * port_p  = strrchr (addr_tmp, ':');
    char * endp = 0;

    if (!port_p) {   // Assume it's a port number.
        unsigned short port_num = static_cast<unsigned short>(::strtol (addr_tmp, &endp, 10));
        if (! *endp) { // strtol scanned the entire string - all digits
            rlt = this->set (port_num);
        }
        //rlt = this->set (0, addr_tmp);
    }
    else {
        * port_p = '\0';
        ++ port_p; // skip over ':'
        unsigned short port_num = static_cast<unsigned short>(::strtol (port_p, &endp, 10));
        if (! *endp) { // strtol scanned the entire string - all digits
            rlt = this->set (port_num, addr_tmp);
        }
    }
    return rlt;
}

// -----------------------------------------------------------------------------

InetAddr & InetAddr::operator= (const InetAddr & rt)
{
    if (this != & rt) {
        memcpy (&_sin, & rt._sin, sizeof (_sin));
    }
    return * this;
}

// -----------------------------------------------------------------------------

bool InetAddr::operator == (const InetAddr & rt) const
{
    return (memcmp (&this->_sin, &rt._sin, sizeof (this->_sin)) == 0);
}

// -----------------------------------------------------------------------------

bool InetAddr::operator != (const InetAddr & rt) const
{
    return !((*this) == rt);
}

// -----------------------------------------------------------------------------

//const InetAddr InetAddr::addr_any (0, INADDR_ANY);

// -----------------------------------------------------------------------------
