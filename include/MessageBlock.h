#ifndef __MCE_MESSAGE_BLOCK_H__
#define __MCE_MESSAGE_BLOCK_H__

#include <string.h>
#include "Mutex.h"
#include "DataBlock.h"

class MessageBlock
{
public:
	enum MessageType
	{
		MB_NONE, MB_PROTO, MB_HANGUP, MB_DATA,
	};

	static MessageBlock * create(DataBlock * db, MessageBlock::MessageType type,
			Mutex * m);

	static MessageBlock * create(size_t size, const char * buf = 0,
			MessageType type = MB_DATA, Mutex * m = 0);

	MessageBlock * release();

	inline MessageType type()
	{
		return _type;
	}

	inline void wrPtr(size_t n)
	{
		_wrPtr += n;
	}

	inline void rdPtr(size_t n)
	{
		_rdPtr += n;
	}

	inline char * wrPtr()
	{
		return _db->dataPtr() + _wrPtr;
	}

	template<typename T>
	inline T * wrPtr()
	{
		return (T *) (_db->dataPtr() + _wrPtr);
	}

	inline char * rdPtr()
	{
		return _db->dataPtr() + _rdPtr;
	}

	template<typename T>
	inline T * rdPtr()
	{
		return (T *) (_db->dataPtr() + _rdPtr);
	}

	inline size_t length()
	{
		return _wrPtr - _rdPtr;
	}

	inline size_t space()
	{
		return this->size() - this->length();
	}

	inline size_t size()
	{
		return _db->size();
	}

	inline void reset()
	{
		_wrPtr = 0;
		_rdPtr = 0;
	}

	inline char * basePtr()
	{
		return _db->dataPtr();
	}

	inline size_t copy(const void * p, size_t size)
	{
		size_t copySize = this->space() > size ? size : this->space();
		memcpy(_db->dataPtr() + _wrPtr, p, copySize);
		this->wrPtr(copySize);
		return copySize;
	}

	inline size_t copy(const char * p)
	{
		size_t size = strlen(p);
		size_t copySize = this->space() > size ? size : this->space();
		memcpy(_db->dataPtr() + _wrPtr, p, copySize);
		this->wrPtr(copySize);
		return copySize;
	}
	MessageBlock * duplicate();

	MessageBlock * clone();

	inline Mutex * mutex()
	{
		return _mutex;
	}

	inline void mutex(Mutex * m)
	{
		_mutex = m;
	}

	inline void cont(MessageBlock * mb)
	{
		_cont = mb;
	}

	inline MessageBlock * cont()
	{
		return _cont;
	}

protected:
	MessageBlock(DataBlock * db, MessageType type, Mutex * m)
			: _type(type), _db(db), _wrPtr(0), _rdPtr(0), _dataPtr(
					db ? db->dataPtr() : 0), _size(db ? db->size() : 0), _mutex(
					0), _cont(0)
	{
	}

	MessageBlock(size_t size, MessageType type, Mutex * m)
			: _type(type), _db(0), _wrPtr(0), _rdPtr(0), _dataPtr(0), _size(
					size), _mutex(m), _cont(0)
	{
	}

	virtual ~MessageBlock()
	{
	}

	MessageType _type;
	DataBlock *_db;
	size_t _wrPtr;
	size_t _rdPtr;
	char *_dataPtr;
	size_t _size;
	Mutex *_mutex;
	MessageBlock *_cont;
};

#endif // __MCE_MESSAGE_BLOCK_H__
