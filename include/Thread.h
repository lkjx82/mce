#ifndef _MCE_THREAD_H_
#define _MCE_THREAD_H_

// -----------------------------------------------------------------------------

#include <pthread.h>

typedef pthread_t ThreadId;
typedef void *(*ThreadFun)(void *);

// -----------------------------------------------------------------------------
// 取消点函数
// pthread_join(3)
// pthread_cond_wait(3)
// pthread_cond_timedwait(3)
// pthread_testcancel(3)
// sem_wait(3)
// sigwait(3)
// -----------------------------------------------------------------------------

class Thread
{
public:
    enum
    {
        JOIN_ABLE, DETACHED,
    };

    Thread()
    {
    }

    static ThreadId selfId()
    {
        return pthread_self();
    }

    static void threadYaild()
    {
#if defined (WIN32)
        Sleep (0);
#elif defined (LINUX)
        pthread_yaild ();
#endif // WIN32
        //::usleep ();
    }

    // 推出本线程
    static void exit(void * retVal)
    {
        pthread_exit(retVal);
    }

    // 分离一个线程
    static int detach(ThreadId tid)
    {
        return pthread_detach(tid);
    }

    static int join(ThreadId tid, void ** rlt)
    {
        return pthread_join(tid, rlt);
    }

    // 建立一个线程
    static int create(ThreadId * tid, ThreadFun fun, void * arg, int joinAble =
            1)
    {
        pthread_attr_t attr;
        pthread_attr_init(&attr);

        pthread_attr_setscope(&attr,
                joinAble ? PTHREAD_CREATE_JOINABLE : PTHREAD_CREATE_DETACHED);

        pthread_create(tid, 0, fun, arg);
        pthread_attr_destroy(&attr);
        return 0;
    }

    // 在某线程取消点取消一个线程
    static int cancel(ThreadId tid)
    {
        return pthread_cancel(tid);
    }

    // 设置线程取消点
    static void testCancel()
    {
        return pthread_testcancel();
    }

    // PTHREAD_CANCEL_ENABLE
    // PTHREAD_CANCEL_DISABLE
    static int setCancelState(int state, int * oldState)
    {
        return pthread_setcancelstate(state, oldState);
    }

    // PTHREAD_CANCEL_ASYNCHRONOUS 请求到达立即执行
    // PTHREAD_CANCEL_DEFERRED，挂起收到的信号直到下个取消点
    static int setCancelType(int type, int * oldType)
    {
        return pthread_setcanceltype(type, oldType);
    }
    //suspend
    //resume
protected:
    //ThreadId   _tid;
};

// -----------------------------------------------------------------------------

#include <list>

// -----------------------------------------------------------------------------

class ThreadManager
{
public:
    ThreadManager()
    {
    }

    ~ThreadManager()
    {
    }

    void wait (ThreadId tid)
    {
    	void * rlt = 0;
    	Thread::join (tid, &rlt);
    }

    void wait()
    {
        ThreadIdList::iterator it = _list.begin();
        it = _list.begin();

        for (; _list.end() != it; ++it) {
            Thread::join(*it, 0);
        }
        _list.clear();
    }

    void spawn(ThreadFun fun, void * arg, int joinable = 1)
    {
        ThreadId tid;
        if (Thread::create(&tid, fun, arg, joinable) == 0) {
            _list.push_back(tid);
        }
    }

    size_t spawn(size_t n_thread, ThreadFun fun, void * arg, int joinable = 1)
    {
        size_t rlt = 0;
        for (size_t n = 0; n < n_thread; n++) {
            ThreadId tid;
            if (Thread::create(&tid, fun, arg, joinable) == 0) {
                _list.push_back(tid);
                rlt++;
            }
        }
        return rlt;
    }

protected:
    typedef std::list<ThreadId> ThreadIdList;
    ThreadIdList _list;
};

#include "Singleton.h"
typedef Singleton<ThreadManager>	ThreadMgrSgt;

#endif /* _MCE_THREAD_H_ */
