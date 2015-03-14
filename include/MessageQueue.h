#ifndef  __MCE_MESSAGE_QUEUE_H__
#define  __MCE_MESSAGE_QUEUE_H__

#include "MessageBlock.h"
#include "Condition.h"

#include <list>

// ----------------------------------------------------------------------------
template <typename MutexT, typename ConditionT>
class MessageQueueT
{
public:
    MessageQueueT() :
            _mutex(0), _cond(0)
    {
        _mutex = new MutexT();
        _cond = new ConditionT();
    }

    MessageQueueT(bool synch) :
            _mutex(0), _cond(0)
    {
        if (synch) {
            _mutex = new ThreadMutex();
            _cond = new ThreadCondition();
        }
        else {
            _mutex = new MutexT();
            _cond = new ConditionT();
        }
    }

    virtual ~MessageQueueT()
    {
        this->release();
    }

    void release()
    {
        _mutex->lock();
        MBList::iterator it = _list.begin();
        for (; _list.end() != it; ++it) {
            if (*it) {
                *it = (*it)->release();
            }
        }
        _list.clear();
        _mutex->unlock();
        delete _mutex;
        delete _cond;
    }

    inline MessageBlock * getq (int millsec)
    {
        return popFront (millsec);
    }

    MessageBlock * popFront(unsigned long sec)
    {
        _mutex->lock();

        if (0 == _list.size()) {
            this->_cond->wait(_mutex, sec);
        }

        if (0 == _list.size ()) {
            _mutex->unlock ();
            return 0;
        }

        MessageBlock * mb = _list.front();
        _list.pop_front();
        _mutex->unlock();
        return mb;
    }

    MessageBlock * popBack(unsigned long sec)
    {
        _mutex->lock();

        if (0 == _list.size()) {
            this->_cond->wait(_mutex, sec);
        }
        if (0 == _list.size ()) {
            _mutex->unlock ();
            return 0;
        }

        MessageBlock * mb = _list.back();
        _list.pop_back();

        _mutex->unlock();
        return mb;
    }

    inline MessageBlock * getq()
    {
        return this->popFront();
    }

    inline int ungetq(MessageBlock * mb)
    {
        return this->pushFront(mb);
    }

    inline int putq(MessageBlock * mb)
    {
        return this->pushBack(mb);
    }

    inline int pushBack(MessageBlock * mb)
    {
        _mutex->lock();
        _list.push_back(mb);
        _cond->singal();
        _mutex->unlock();
        return 0;
    }

    inline int pushFront(MessageBlock * mb)
    {
        _mutex->lock();
        _list.push_front(mb);
        _cond->singal();
        _mutex->unlock();
        return 0;
    }

    MessageBlock * popBack()
    {
        _mutex->lock();

        while (0 == _list.size()) {
            this->_cond->wait(_mutex);
        }

        MessageBlock * mb = _list.back();
        _list.pop_back();

        _mutex->unlock();
        return mb;
    }

    MessageBlock * popFront()
    {
        _mutex->lock();

        while (0 == _list.size()) {
            this->_cond->wait(_mutex);
        }

        MessageBlock * mb = _list.front();
        _list.pop_front();
        _mutex->unlock();
        return mb;
    }

    inline int size()
    {
        return _list.size();
    }

protected:
    Mutex * _mutex;
    Condition * _cond;
    typedef std::list<MessageBlock *> MBList;
    MBList _list;
};

typedef MessageQueueT<SpinMutex, Condition> NoBlockMessageQueue;
typedef MessageQueueT<ThreadMutex, ThreadCondition> MessageQueue;


#endif // __MCE_MESSAGE_QUEUE_H__
