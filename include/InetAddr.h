#ifndef __MCE_INET_ADDR_H__
#define __MCE_INET_ADDR_H__

// -----------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <dlfcn.h> // 需要libdl.a 和 libdl.so

#define MAX_HOSTNAME_LEN	128

// -----------------------------------------------------------------------------

/*
HAS_INET_PTON: use function inet_ntop for get_host_addr
HAS_GETHOSTBYADDR_R: use gethostbyaddr_r for get_host_name, is thread safe
HAS_GETHOSTBYNAME_R: use gethostbyname_r
*/

// -----------------------------------------------------------------------------

class InetAddr
{
public:

    InetAddr ();

    InetAddr (const InetAddr & rt);

    InetAddr (const sockaddr_in * addr, size_t size);

    // 16 bit port, 32 bit ip
    InetAddr (unsigned short port, unsigned long int host = INADDR_ANY);

    // 16 bit port, string ip
    InetAddr (unsigned short port, const char * host);

    // str port or str host:port
    InetAddr (const char * addr);

    //~Inet_Addr ();
    
    // str port or str host:port, reset sockaddr_in
    int set (const char * addr);

    // 16bit port, reset sockaddr_in 
    int set (unsigned short port);

    // 16 bit port, 32 bit ip, reset sockaddr_in
    int set (unsigned short port, unsigned long int host);

    // 16 bit port, str ip, reset sockaddr_in
    int set (unsigned short port, const char * host);

    inline int set (sockaddr_in * ad, int ad_len)
    {
        memcpy (&this->_sin, ad, ad_len);
        return 0;
    }

    // set 16 bit port, not reset sockaddr_in
    void portNumber (unsigned short port);

    // get 16 bit port 
    unsigned short portNumber ();

    // get 32bit ip
    unsigned long int ipNumber ();

    // set 32bit ip,not reset sockaddr_in
    void ipNumber (unsigned long int ip);

    // get str hostname
    int getHostName (char * hostname, size_t len);

    // get str ip
    int getHostAddr (char * addr, size_t addr_size);
  
    // transform sockaddr_in to str ip:port or hostname:port
    int toString (char * buf, size_t buf_size, int ip_format = 1);

    // set addr from str port or host:port, reset sockaddr_in
    int fromString (const char * addr_str);

    int isLoopback ();

    int isMulticast ();

    InetAddr & operator= (const InetAddr & rt);

    bool operator == (const InetAddr & rt) const;
   
    bool operator != (const InetAddr & rt) const;

    inline const struct sockaddr * addrPtr () const
    {
        return (sockaddr *)& _sin;
    }

    inline int addrSize () const
    {
        return sizeof (struct sockaddr_in);
    }

//    static const InetAddr addr_any;

protected:
    struct sockaddr_in  _sin;
};

// -----------------------------------------------------------------------------

#endif // __MCE_INET_ADDR_H__
