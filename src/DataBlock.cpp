#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/DataBlock.h"

DataBlock * DataBlock::create(size_t size, const char * buf)
{
    DataBlock * db = new DataBlock(size, buf);
    if (!db) {
        return 0;
    }
    if (!buf && size) {
        db->_data = (char *) malloc(size);
        if (db->_data) {
            db->_selfDelete = true;
            return db;
        }
        else {
        	assert (0);
            db->release();
            return 0;
        }
    }
    return db;
}

DataBlock::DataBlock(size_t size, const char * buf) :
        _size(size), _selfDelete(false), _data((char *) buf), _nRef(1)
{
}

DataBlock * DataBlock::release()
{
    if (0 == --_nRef) {
        if (_selfDelete) {
            free(_data);
        }
        _data = 0;
        _size = 0;
        _selfDelete = false;
        delete this;
    }
    return 0;
}

DataBlock * DataBlock::clone()
{
    DataBlock * db = DataBlock::create(this->_size, 0);
    if (!db) {
        return 0;
    }

    memcpy(db->dataPtr(), this->_data, this->_size);
    return db;
}

DataBlock * DataBlock::duplicate()
{
    ++ this->_nRef;
    return this;
}
