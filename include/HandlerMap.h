#ifndef __MCE_HANDLER_MAP_H__
#define __MCE_HANDLER_MAP_H__

#include <list>
#include <stdio.h>
#include <stdlib.h>
#include "SocketIOHandlerBase.h"

#include "c_list.h"

class HandlerEntiy
{
	SocketIOHandlerBase *_sh;
	C_List_Node _node;
};

class HandlerMap
{
public:
	struct iterator
	{
		iterator(C_List_Node * n)
		{
			_node = n;
		}

		iterator()
				: _node(0)
		{
		}

		iterator & operator ++()
		{
			list_node_next(_node);
			return *this;
		}

		iterator & operator --()
		{
			list_node_prev(_node);
			return *this;
		}

		C_List_Node * _node;
	};

	iterator begin()
	{
		C_List_Node * n = list_begin (&_list);
		return iterator(n);
	}

	iterator end()
	{
		C_List_Node * n = list_end (&_list);
		return iterator(n);
	}

	HandlerMap()
			: _handlers(0), _size(0)
	{
	}

	virtual ~HandlerMap()
	{
		free(_handlers);
		_handlers = 0;
		_size = 0;
//        _list.clear ();
		list_clear(&_list);
	}

	bool create(size_t maxFd)
	{
		_handlers = (SocketIOHandlerBase **) malloc(
				sizeof(SocketIOHandlerBase *) * maxFd);
		if (_handlers) {
			memset(_handlers, 0, sizeof(SocketIOHandlerBase *) * maxFd);
			_size = maxFd;
			list_clear(&_list);
			return true;
		}
		return false;
	}

	int insert(int fd, SocketIOHandlerBase * h)
	{
		if ((size_t) fd > _size || _handlers[fd]) {
			return -1;
		}
		_handlers[fd] = h;

//		list_push_back(&_list, list_node_of (h, SocketIOHandlerBase, _node));
		return 0;
	}

	int erase(int fd, SocketIOHandlerBase ** b)
	{
		if ((size_t) fd > _size || !_handlers[fd]) {
			return -1;
		}
		*b = _handlers[fd];

		_handlers[fd] = 0;

//		list_erase(list_node_of (*b, SocketIOHandlerBase, _node) );
//        _list.erase (*b);
		return 0;
	}

	SocketIOHandlerBase * get(int fd)
	{
		if (_size <= (size_t) fd) {
			return 0;
		}
		return _handlers[fd];
	}

protected:
//    typedef std::list<SocketIOHandlerBase *> HandlerList;
	//    HandlerList _list;
	C_List _list;

	SocketIOHandlerBase **_handlers;
	size_t _size;
};

#endif // __MCE_HANDLER_MAP_H__
