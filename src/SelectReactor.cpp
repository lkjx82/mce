#include "../include/SelectReactor.h"
#include "../include/TimeQueue.h"
#include <list>
#include <assert.h>

// ----------------------------------------------------------------------------

SelectReactor::SelectReactor(int fdMax, TimeQueue * tq)
		: _tq(tq), _tqDel(false), _maxFdCnt (fdMax), _fds (0)
{
	_maxFd = 0;
	_maxFdCnt = (0 == _maxFdCnt) ? 65535 : _maxFdCnt;
	_fds = (int *) malloc (sizeof (int) * _maxFdCnt);
	if (!_fds) {
		abort ();
		return;
	}

	if (!_tq) {
		_tq = new TimeQueue;
		if (!_tq) {
			free (_fds);
			abort();
		}
		_tqDel = true;
	}
}

// ----------------------------------------------------------------------------

SelectReactor::~SelectReactor()
{
	HandlerMap::iterator it = _shs.begin();
	for (; _shs.end() != it; ++it) {
		if (it->second) {
			it->second->handleClose();
		}
		else {
			printf("error\n~");
		}
	}

	_shs.clear();

	if (_tqDel) {
		delete _tq;
		_tqDel = false;
	}
	_tq = 0;

	free (_fds);
	_fds = 0;

//	printf("~SocketSelectReactor\n");
}

// ----------------------------------------------------------------------------

int SelectReactor::registerHandler(EventHandlerBase * handler, int mask)
{
	if (_shs.end() != _shs.find(handler->getHandle())) {
		printf("register failed fd: %d\n", handler->getHandle());
		return -1;
	}

	if (_shs.insert(std::make_pair(handler->getHandle(), handler)).second) {
//		printf("register success fd: %d\n", handler->getHandle());
		_maxFd = _maxFd > handler->getHandle() ? _maxFd : handler->getHandle();
		handler->mask(mask);
		return 0;
	}

	return -1;
}

// ----------------------------------------------------------------------------

int SelectReactor::removeHandler(EventHandlerBase * handler, int callClose)
{
//	printf("removeHandler %u\n", handler->getHandle());

	HandlerMap::iterator it = _shs.find(handler->getHandle());
	if (_shs.end() != it) {
//		it->second->reactor (0);
		it->second->mask (EventHandlerBase::EVENT_NULL);
		if (callClose) {
			it->second->handleClose(it->second->getHandle());
		}

		_shs.erase(it);

		this->_calcMaxFd();

		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int SelectReactor::cancelWakeup(EventHandlerBase * handler, int mask)
{
	HandlerMap::iterator it = _shs.find(handler->getHandle());
	if (_shs.end() != it) {
		it->second->mask(it->second->mask() & (~mask));
		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int SelectReactor::scheduleWakeup(EventHandlerBase * handler, int mask)
{
	HandlerMap::iterator it = _shs.find(handler->getHandle());
	if (_shs.end() != it) {
		it->second->mask(it->second->mask() | mask);
		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int SelectReactor::tick(const TimeValue & timeout)
{
	if (_shs.empty()) {
		_tq->elapse();
		return 0;
	}

	TimeValue tv;
	TimeValue soon;
	tv.now();

	_tq->elapse(tv);

	if (_tq->getSoonTime(soon)) {
		tv = soon - tv;
		tv = tv < timeout ? tv : timeout;
	}
	else {
		tv = timeout;
	}

	fd_set ws, rs, es;
	FD_ZERO(&ws);
	FD_ZERO(&rs);
	FD_ZERO(&es);

	// ------------------------

	int nFd = 0;

	HandlerMap::iterator it = _shs.begin();
	for (; _shs.end() != it; ++it) {
		if (it->second->mask() & EventHandlerBase::EVENT_READ) {
			FD_SET(it->first, &rs);
		}

		if (it->second->mask() & EventHandlerBase::EVENT_WRITE) {
			FD_SET(it->first, &ws);
		}

		FD_SET(it->first, &es);
		// 记录 fd 到 fds
		_fds[nFd++] = it->first;
	}

	int rlt = ::select(_maxFd + 1, &rs, &ws, &es, &((TimeValue &) tv).tv());
	// error
	if (rlt < 0) {
		printf("sock error: %s\n", strerror(errno));
		return 0;
	}

	// timeout
	if (0 == rlt) {
		return 0;
	}

	// 是否需要重新计算 maxFd

	bool reCalcFd = false;

	for (int i = 0; i < nFd; i++) {

		int curFd = _fds[i];
		bool delCurFd = false;
		int closeFlag = 0;

		HandlerMap::iterator it = _shs.find(curFd);
		if (_shs.end() == it) {
			assert(0);
			continue;
		}

		EventHandlerBase * eh = it->second;

		// process read
		if ((eh->mask() & EventHandlerBase::EVENT_READ) && FD_ISSET (curFd, &rs)) {
			if (eh->handleInput(curFd) < 0) {
				delCurFd = true;
				closeFlag |= EventHandlerBase::EVENT_READ;
			}
		}

		// process write
		if ((eh->mask() & EventHandlerBase::EVENT_WRITE) && FD_ISSET (curFd, &ws)) {
			if (eh->handleOutput(curFd) < 0) {
				delCurFd = true;
				closeFlag |= EventHandlerBase::EVENT_WRITE;
			}
		}

		// es
		if ((eh->mask() & EventHandlerBase::EVENT_ES) && FD_ISSET (curFd, &es)) {
			closeFlag |= EventHandlerBase::EVENT_ES;
			delCurFd = true;
			abort();
		}

		if (delCurFd) {
			_shs.erase(curFd);
			eh->handleClose(curFd, closeFlag);
			reCalcFd = true;
		}
	}

	if (reCalcFd) {
		_calcMaxFd();
	}

	return 0;

	// ---------------------

//	HandlerMap::iterator it = _shs.begin();
//	for (; _shs.end() != it; ++it) {
//		if (it->second->mask() & EventHandlerBase::EVENT_READ) {
//			FD_SET(it->first, &rs);
//		}
//
//		if (it->second->mask() & EventHandlerBase::EVENT_WRITE) {
//			FD_SET(it->first, &ws);
//		}
//
//		FD_SET(it->first, &es);
//	}
//
//	switch (::select(_maxFd + 1, &rs, &ws, &es, &((TimeValue &) tv).tv()))
//	{
//	case -1:
//		printf("sock error: %s\n", strerror(errno));
//		break;
//
//		// timeout
//	case 0:
////          printf ("socket %d\n", it->first);
//		return 0;
//		break;
//
//	default: {
//		HandlerMap::iterator it = _shs.begin();
//
//		bool reCalcFd = false;
////		std::list <EventHandlerBase *> eraseList;
//
//		while (_shs.end() != it) {
//
//			// process read
//			if (FD_ISSET (it->first, &rs)) {
//				if (it->second->handleInput(it->first) < 0) {
//
//					EventHandlerBase * eb = it->second;
//					HandlerMap::iterator itErase = it;
//					++it;
//					_shs.erase(itErase);
//
//					eb->handleClose(eb->getHandle(),
//							EventHandlerBase::EVENT_READ);
//
//					reCalcFd = true;
//					continue;
//				}
//			}
//
//			// process write
//			if (FD_ISSET (it->first, &ws)) {
//				if (it->second->handleOutput(it->first) < 0) {
//
//					EventHandlerBase * eb = it->second;
//					HandlerMap::iterator itErase = it;
//					++it;
//					_shs.erase(itErase);
//
//					eb->handleClose(eb->getHandle(),
//							EventHandlerBase::EVENT_WRITE);
//
//					reCalcFd = true;
//					continue;
//				}
//			}
//
//			if (FD_ISSET (it->first, &es)) {
//
//				EventHandlerBase * eb = it->second;
//				HandlerMap::iterator itErase = it;
//				++it;
//				_shs.erase(itErase);
//
//				eb->handleClose(eb->getHandle(), EventHandlerBase::EVENT_NULL);
//
//				reCalcFd = true;
//				abort();
//				continue;
//			}
//
//			// no read and no write
//			//printf("have no read and write event but call by select\n");
//			++it;
//		}
//		if (reCalcFd) {
//			_calcMaxFd();
//		}
//	}
//		break;
//	}
//	return 0;
}

// ----------------------------------------------------------------------------

int SelectReactor::tick(long timeout)
{
	if (_shs.empty()) {
		_tq->elapse();
		return 0;
	}

	TimeValue tv;
	tv.setMesc(timeout);
	return this->tick(tv);
}

// ----------------------------------------------------------------------------

void SelectReactor::_calcMaxFd()
{
	HandlerMap::iterator it = _shs.begin();
	int maxFd = 0;
	if (_shs.end() != it) {
		maxFd = it->first;
	}

	for (; _shs.end() != it; ++it) {
		_maxFd = maxFd > it->first ? maxFd : it->first;
	}
}

// ----------------------------------------------------------------------------

int SelectReactor::scheduleTime(TimeHandler * th, const void * arg,
		const TimeValue & delay, const TimeValue & interval)
{
	return _tq->schedule(th, arg, delay, interval) ? 0 : -1;
}

// ----------------------------------------------------------------------------

bool SelectReactor::cancelTime(TimeHandler * th)
{
	return _tq->cancel(th);
}

// ----------------------------------------------------------------------------
