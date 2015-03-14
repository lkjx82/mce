#ifndef __MCE_NONBLOCK_SOCKET_HANDLER__H__
#define __MCE_NONBLOCK_SOCKET_HANDLER__H__

#include <assert.h>
#include "SocketIOHandlerBase.h"
#include "ReactorBase.h"
#include "MessageBlock.h"
#include "MessageQueue.h"



#pragma pack(1)
struct MsgHead
{
    enum
    {
        MAX_PACKET_LEN = 40960,
    };

	uint32_t    _size;
	uint32_t 	_opcode;
	uint64_t    _data;
};
#pragma pack()


class NonBlockSocketHandler: public SocketIOHandlerBase
{
public:

    NonBlockSocketHandler(ReactorBase * r);

    // 需要在派生类里调用 父类的 open
    virtual int open (void * /*arg*/ = 0);

    int connect(const InetAddr & remoteAddr);

    int listen (const InetAddr & listenAddr);

    virtual int handleInput(int fd = -1);

    virtual int handleOutput(int fd = -1);

    // 需要在派生类里调用 父类的 handleClose
    virtual int handleClose(int fd = -1, int closeMask = SocketIOHandlerBase::EVENT_NULL);

    int send(MessageBlock * mb);

    int sendAsyn (MessageBlock * mb);

protected:
    typedef NoBlockMessageQueue MQ;
    // 处理接受到的数据，把所有整包都处理以后，重置 MessageBlock
    virtual int _procRecvData(MessageBlock *mb);
	virtual int _procMessage (void * buf, int size);

    MessageBlock * _mbRecv;
    MQ * _mqSend;
};

#endif // __MCE_NONBLOCK_SOCKET_HANDLER__H__
