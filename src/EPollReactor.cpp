#include <sys/epoll.h>
#include <unistd.h>
#include <assert.h>
#include "../include/TimeQueue.h"
#include "../include/EPollReactor.h"

// ----------------------------------------------------------------------------

EPollReactor::EPollReactor(int fdMax, TimeQueue * tq)
		: _fdMax(0), _evts(0), _tq(tq), _tqDel(false)
{
	_evts = (struct epoll_event *) ::malloc(
			(fdMax + 1) * sizeof(struct epoll_event));
	if (!_evts) {
		abort();
		return;
	}

	if (!_tq) {
		_tq = new TimeQueue;
		if (!_tq) {
			abort();
		}
		_tqDel = true;
	}

	_epFd = epoll_create(fdMax);
	if (_epFd < 0) {
		::free(_evts);
		_evts = 0;
		if (_tqDel) {
			delete _tq;
		}
		abort();
	}
	_fdMax = fdMax;
}

// ----------------------------------------------------------------------------

EPollReactor::~EPollReactor()
{
	HandlerMap::iterator it = _shs.begin();
	for (; _shs.end() != it; ++it) {
		if (it->second) {
			it->second->handleClose();
		}
		else {
			assert(0);
			printf("error\n~");
		}
	}

	_shs.clear();
	if (_epFd > 0) {
		::close(_epFd);
		_epFd = -1;
	}

	::free(_evts);
	_evts = 0;
	_fdMax = 0;

	if (_tqDel) {
		delete _tq;
		_tqDel = false;
	}
	_tq = 0;
//	printf("~SocketEPollReactor\n");
}

// ----------------------------------------------------------------------------

int EPollReactor::registerHandler(EventHandlerBase * handler, int mask)
{
	if (_shs.end() != _shs.find(handler->getHandle())) {
//		assert(0);
		return -1;
	}

//	printf ("register handler %d\n", handler->getHandler());
	if (_shs.insert(std::make_pair(handler->getHandle(), handler)).second) {
		handler->mask(mask);

		struct epoll_event et = { 0, { 0 } };

		et.data.ptr = handler;
		et.events = EPOLLET;

		if (mask & EventHandlerBase::EVENT_READ) {
			et.events |= EPOLLIN;
		}
		if (mask & EventHandlerBase::EVENT_WRITE) {
			et.events |= EPOLLOUT;
			assert(0);
		}

		if (0 != epoll_ctl(_epFd, EPOLL_CTL_ADD, handler->getHandle(), &et)) {
			assert(0);
			return -1;
		}
		return 0;
	}

	return -1;
}

// ----------------------------------------------------------------------------

int EPollReactor::removeHandler(EventHandlerBase * handler, int callClose)
{
	HandlerMap::iterator it = _shs.find(handler->getHandle());

	if (_shs.end() != it) {

		struct epoll_event et = { 0, { 0 } };

		epoll_ctl(_epFd, EPOLL_CTL_DEL, handler->getHandle(), &et);

		it->second->mask (EventHandlerBase::EVENT_NULL);

//		it->second->reactor (0);
		if (callClose) {
			it->second->handleClose(it->second->getHandle());
		}

		_shs.erase(it);
		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int EPollReactor::cancelWakeup(EventHandlerBase * handler, int mask)
{
	HandlerMap::iterator it = _shs.find(handler->getHandle());
	if (_shs.end() != it) {

		it->second->mask(it->second->mask() & (~mask));

		struct epoll_event et = { 0, { 0 } };
		et.data.ptr = handler;
		et.events = EPOLLET;

//		if (SocketIOHandlerBase::EVENT_NULL == it->second->mask()) {
//			epoll_ctl(_epFd, EPOLL_CTL_DEL, it->first, &et);
//		}
//		else {

		if (it->second->mask() & EventHandlerBase::EVENT_READ) {
			et.events |= EPOLLIN;
		}

		if (it->second->mask() & EventHandlerBase::EVENT_WRITE) {
			printf("!!!!\n");
			assert(0);
			et.events |= EPOLLOUT;
		}

		if (0 != epoll_ctl(_epFd, EPOLL_CTL_MOD, handler->getHandle(), &et)) {
			assert(0);
			return -1;
		}
//		}

		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int EPollReactor::scheduleWakeup(EventHandlerBase * handler, int mask)
{
	HandlerMap::iterator it = _shs.find(handler->getHandle());
	if (_shs.end() != it) {
		it->second->mask(it->second->mask() | mask);

		struct epoll_event et = { 0, { 0 } };
		et.data.ptr = handler;
		et.events = EPOLLET;

		if (it->second->mask() & EventHandlerBase::EVENT_READ) {
			et.events |= EPOLLIN;
		}
		if (it->second->mask() & EventHandlerBase::EVENT_WRITE) {
//			assert(0);
			et.events |= EPOLLOUT;
		}

		if (0 != epoll_ctl(_epFd, EPOLL_CTL_MOD, handler->getHandle(), &et)) {
			return -1;
		}

		return 0;
	}
	return -1;
}

// ----------------------------------------------------------------------------

int EPollReactor::tick(const TimeValue & timeout)
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

	int nFd = epoll_wait(_epFd, _evts, _fdMax, tv.msec());

	if (nFd < 0) {
		if (EINTR == errno) {
			return 0;
		}
		return -1;
	}

	// timeout
	else if (0 == nFd) {
		return 0;
	}

	// > 0
	else {
		for (int i = 0; i < nFd; ++i) {

			EventHandlerBase * eh = (EventHandlerBase *) _evts[i].data.ptr;
			bool delFd = false;
			int closeFlag = 0;
			int fd = eh->getHandle();

			if ((_evts[i].events & EPOLLIN) && (EventHandlerBase::EVENT_READ
					& eh->mask())) {
				if (eh->handleInput(fd) < 0) {
					delFd = true;
					closeFlag |= EventHandlerBase::EVENT_READ;
				}
			}

			if ((_evts[i].events & EPOLLOUT) && (EventHandlerBase::EVENT_WRITE
					& eh->mask())) {
				if (eh->handleOutput(fd) < 0) {
					delFd = true;
					closeFlag |= EventHandlerBase::EVENT_WRITE;
				}
			}

			if (delFd) {
				int f = eh->getHandle();
				eh->handleClose(f, closeFlag);
				_shs.erase(f);
			}
		}
	}
	return 0;
}

// ----------------------------------------------------------------------------

int EPollReactor::tick(long timeout)
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

int EPollReactor::scheduleTime(TimeHandler * th, const void * arg,
		const TimeValue & delay, const TimeValue & interval)
{
	return _tq->schedule(th, arg, delay, interval) ? 0 : -1;
}

// ----------------------------------------------------------------------------

bool EPollReactor::cancelTime(TimeHandler * th)
{
	return _tq->cancel(th);
}

// ----------------------------------------------------------------------------

