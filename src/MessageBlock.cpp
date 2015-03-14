#include <string.h>
#include "../include/MessageBlock.h"

MessageBlock * MessageBlock::create(DataBlock * db,
        MessageBlock::MessageType type, Mutex * m)
{
    MessageBlock * mb = new MessageBlock(db, type, m);
    return mb;
}

MessageBlock * MessageBlock::create(size_t size, const char * buf,
        MessageBlock::MessageType type, Mutex * m)
{
    MessageBlock * mb = new MessageBlock(size, type, m);
    if (mb && size) {
        mb->_db = DataBlock::create(size, buf);
        if (mb->_db) {
            mb->_dataPtr = mb->_db->dataPtr();
            return mb;
        }
        return mb->release();

    }
    return mb;
}

MessageBlock * MessageBlock::release()
{
    Mutex * m = _mutex;
    if (m) {
        m->lock();
    }

    if (_db) {
        this->_db = this->_db->release();
    }
    this->_type = MB_NONE;
    this->_wrPtr = 0;
    this->_rdPtr = 0;
    this->_size = 0;
    this->_dataPtr = 0;
    MessageBlock * cont = this->cont ();
    this->cont (0);

    if (m) {
        m->unlock();
    }

    delete this;

    if (cont) {
        cont = cont->release();
    }

    return 0;
}

MessageBlock * MessageBlock::duplicate()
{
    if (_mutex) {
        _mutex->lock();
    }

    MessageBlock * mb = MessageBlock::create(0, this->_type, 0);
    if (!mb) {
        if (_mutex) {
            _mutex->unlock();
        }
        return 0;
    }
    if (_db) {
        mb->_db = _db->duplicate();
    }

    mb->wrPtr(this->_wrPtr);
    mb->rdPtr(this->_rdPtr);
    mb->_size = this->_size;
    mb->_dataPtr = this->_dataPtr;

    if (_mutex) {
        _mutex->unlock();
    }

    return mb;
}

MessageBlock * MessageBlock::clone()
{
    if (_mutex) {
        _mutex->lock();
    }

    MessageBlock * mb = MessageBlock::create(0, this->_type, 0);
    if (!mb) {
        if (_mutex) {
            _mutex->unlock();
        }
        return 0;
    }

    if (_db) {
        mb->_db = _db->clone();
        if (mb->_db) {
            mb->_dataPtr = mb->_db->dataPtr();
        }
        else {
            if (_mutex) {
                _mutex->unlock();
            }
            return mb->release();
        }
    }
    mb->_wrPtr = this->_wrPtr;
    mb->_rdPtr = this->_rdPtr;
    mb->_size = this->_size;

    if (_mutex) {
        _mutex->unlock();
    }
    return mb;
}
