#ifndef __MCE_DATA_BLOCK_H__
#define __MCE_DATA_BLOCK_H__

#include "Mutex.h"
#include "AtomicOp.h"

class DataBlock
{
public:
    static DataBlock * create (size_t size, const char * buf);
    DataBlock * release();

    inline char * dataPtr()
    {
        return _data;
    }

    inline size_t size()
    {
        return _size;
    }

    DataBlock * clone();

    DataBlock * duplicate();

protected:
    DataBlock(size_t size, const char * buf);

    virtual ~DataBlock()
    {
    }

    size_t  _size;
    bool    _selfDelete;
    char    *_data;
    AtomicOp<long> _nRef;
};

// -----------------------------------------------------------------------------------

#endif // __MCE_DATA_BLOCK_H__
