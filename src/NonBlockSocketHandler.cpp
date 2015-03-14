#include "../include/NonBlockSocketHandler.h"

NonBlockSocketHandler::NonBlockSocketHandler(ReactorBase * r)
		: SocketIOHandlerBase(r), _mbRecv(0), _mqSend(0)
{
}

int NonBlockSocketHandler::open(void * arg)
{
	SocketIOHandlerBase::open(arg);

	if (0 != this->peer().setOption(SocketIO::SOCKOPT_NONBLOCK, 1)) {
		assert(0);
		return -1;
	}

	if (_mbRecv) {
		_mbRecv = _mbRecv->release();
	}
	_mbRecv = MessageBlock::create(MsgHead::MAX_PACKET_LEN, 0,
			MessageBlock::MB_DATA, 0);
	if (!_mbRecv) {
		return -1;
	}

	if (_mqSend) {
		delete _mqSend;
		_mqSend = 0;
	}
	_mqSend = new MQ(false);
	if (!_mqSend) {
		_mbRecv = _mbRecv->release();
		return -1;
	}
	return 0;
}

// TODO 非阻塞connect，connect 函数 不应该调用 handleClose
int NonBlockSocketHandler::connect(const InetAddr & remoteAddr)
{
	if (0 != peer().open()) {
		return -1;
	}

	else if (0 != peer().connect(remoteAddr)) {
		return -1;
	}

	else if (0 != this->open()) {
		return -1;
	}

	this->reactor()->registerHandler (this, EventHandlerBase::EVENT_READ);

//	else if (0
//			!= this->reactor()->registerHandler(this,
//					SocketIOHandlerBase::EVENT_READ)) {
////		handleClose();
//		return -1;
//	}

	return 0;
}

int NonBlockSocketHandler::handleInput(int /*fd*/)
{
	do {
//		printf ("NonBlockSocketHandler::handleInput\n");
		ssize_t recvSize = peer().recv(_mbRecv->wrPtr(), _mbRecv->space(),
				MSG_DONTWAIT);
		assert(0 != _mbRecv->space());

		if (0 < recvSize) {
			_mbRecv->wrPtr((size_t) recvSize);
			if (0 != _procRecvData(_mbRecv)) {
				return -1;
			}
		}
		else if (0 > recvSize) {
			if (EWOULDBLOCK == errno || EINTR == errno) {
//				printf("[error] fd: %d, recv -1: %s\n", this->peer().fd(),
//						strerror(errno));
				return 0;
			}
			else {
//				printf("[error] fd: %d, recv -1: %s\n", this->peer().fd(),
//						strerror(errno));
				return -1;
			}
		}
		// recv = 0
		else {
			return -1;
		}
	} while (1);
	return 0;
}

int NonBlockSocketHandler::_procRecvData(MessageBlock *mb)
{
	MsgHead * mh = (MsgHead *) mb->rdPtr();

	if (mh->_size > MsgHead::MAX_PACKET_LEN) {
		printf("header to long\n");
		return -1;
	}

	while (mb->length() > 0) {
		if (mh->_size <=  mb->length()) {
			mb->rdPtr( mh->_size);
			if (0 != _procMessage(mh, mh->_size)) {
				return -1;
			}
			mh = (MsgHead *) mb->rdPtr();
		}
		else if (mh->_size > MsgHead::MAX_PACKET_LEN) {
			printf("header to long 2 len: %d\n", mh->_size);
			return -1;
		}
		else {
			break;
		}
	}

// 移动数据
	if (mb->rdPtr() != mb->basePtr()) {
		size_t len = mb->length();
		memmove(mb->basePtr(), mb->rdPtr(), len);
		mb->reset();
		mb->wrPtr(len);
	}
	return 0;
}

int NonBlockSocketHandler::_procMessage(void * buf, int size)
{
	printf("NonBlockSocketHandler::_procMessage msgLen = %d, msg: %s\n", size,
			(char *) buf);
	return 0;
}

int NonBlockSocketHandler::send(MessageBlock * mb)
{
// send queue not empty, write data to send queue
	if (0 != _mqSend->size()) {
		_mqSend->putq(mb);
//		this->reactor()->scheduleWakeup(this, SocketIOHandlerBase::EVENT_WRITE);
		return 0;
	}

// send queue is empty, send data by socket
	do {
		assert (mb);
		assert (0 != mb->length());
		ssize_t sentSize = peer().send(mb->rdPtr(), mb->length(), MSG_NOSIGNAL);

		if (sentSize == (ssize_t) mb->length()) {
			mb = mb->release();
			return 0;
		}
		else {
			// 半包，EWOULDBLOCK, error了
			if (0 < sentSize) {
				mb->rdPtr((size_t) sentSize);
				printf("send error %s !!!~~ \n", strerror(errno));
				continue;
			}
			else if (0 > sentSize) {
				_mqSend->ungetq(mb);
				if (EWOULDBLOCK == errno || EINTR == errno) {
//					printf("send EWOULDBLOCK\n");
					this->reactor()->scheduleWakeup(this,
							SocketIOHandlerBase::EVENT_WRITE);
					return 0;
				}
				return -1;
			}
			// sentSize = 0
			else {
				_mqSend->ungetq(mb);
				return -1;
			}
		}
	} while (1);
	return 0;
}

int NonBlockSocketHandler::sendAsyn(MessageBlock * mb)
{
	_mqSend->putq(mb);
	return this->reactor()->scheduleWakeup(this,
			SocketIOHandlerBase::EVENT_WRITE);
}

int NonBlockSocketHandler::handleOutput(int /*fd*/)
{
//	printf("handleOutput ()\n");
	// 不应该运行到这里
	if (0 == _mqSend->size()) {
		this->reactor()->cancelWakeup(this, SocketIOHandlerBase::EVENT_WRITE);
		assert(0);
		return 0;
	}

	MessageBlock * mbSend = 0;
	while (_mqSend->size()) {

		mbSend = _mqSend->getq();

		do {
			assert (mbSend);
			assert (0 != mbSend->length());

			ssize_t sentSize = peer().send(mbSend->rdPtr(), mbSend->length(),
					MSG_NOSIGNAL);

			if (sentSize == (ssize_t) mbSend->length()) {
				mbSend = mbSend->release();
				break;
			}
			// 不全包
			else {
				if (0 < sentSize) {
					mbSend->rdPtr((size_t) sentSize);
					continue;
				}
				// error or EAGAIN
				else if (0 > sentSize) {
					_mqSend->ungetq(mbSend);

					if (EWOULDBLOCK == errno || EINTR == errno) {
//						printf("send EWOULDBLOCK\n");
						return 0;
					}
					this->reactor()->cancelWakeup(this,
							SocketIOHandlerBase::EVENT_WRITE);
					return -1;
				}
				// sentSize == 0
				else {
					_mqSend->ungetq(mbSend);
					return -1;
				}
			}
		} while (1);
	}

	return this->reactor()->cancelWakeup(this, SocketIOHandlerBase::EVENT_WRITE);
}

int NonBlockSocketHandler::handleClose(int, int /*closeMask*/)
{
	this->peer().close();
	if (_mqSend) {
		delete _mqSend;
		_mqSend = 0;
	}
	if (_mbRecv) {
		_mbRecv = _mbRecv->release();
	}
//	printf("NonBlockSocketHandler::handleClose ()\n");
	return 0;
}
