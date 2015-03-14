#ifndef __MCE_TASK_BASE_H__
#define __MCE_TASK_BASE_H__

#include "MessageQueue.h"
#include "Thread.h"

class TaskBase
{
public:
    TaskBase(ThreadManager * threadMgr) :
            _threadMgr (threadMgr), _arg (0), _prev(0), _next(0), _mq(true)
    {
    }

    virtual ~TaskBase () {}

    virtual int open(void * arg)
    {
        _arg = arg;
        return 0;
    }

    virtual int svc() = 0;

    static void * _threadFun(void * task)
    {
        if (!task) {
            return 0;
        }

        TaskBase * t = (TaskBase *) task;
        t->svc();
        return 0;
    }

    inline int active(size_t n, int joinable = 1)
    {
        return _threadMgr->spawn(n, TaskBase::_threadFun, this,
                joinable) == 0;
    }

    inline int putq(MessageBlock * mb)
    {
        return this->_mq.putq(mb);
    }

    inline MessageBlock * getq()
    {
        return this->_mq.getq();
    }

    inline MessageBlock * getq (int sec)
    {
    	return this->_mq.getq(sec);
    }

    inline TaskBase * prev ()
    {
        return _prev;
    }

    inline void prev (TaskBase * t)
    {
        _prev = t;
    }

    inline TaskBase * next ()
    {
        return _next;
    }

    inline void next (TaskBase * t)
    {
        _next = t;
    }

    inline void wait ()
    {
    	this->_threadMgr->wait ();
    }

protected:
    ThreadManager * _threadMgr;
    void * _arg;
    TaskBase *_prev;
    TaskBase *_next;
    MessageQueue _mq;
    ThreadMutex _mutex;
    ThreadCondition _cond;
};

#endif // __MCE_TASK_BASE_H__
