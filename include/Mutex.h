#ifndef __MCE_MUTEX_H__
#define __MCE_MUTEX_H__

#include <pthread.h>

class Mutex
{
public:
    Mutex()
    {
    }
    virtual ~Mutex()
    {
    }

    virtual int lock()
    {
        return 0;
    }

    virtual int lock (unsigned long sec)
    {
        return 0;
    }

    virtual int unlock()
    {
        return 0;
    }

    virtual int trylock ()
    {
        return 0;
    }

    virtual void * m()
    {
        return 0;
    }
};

class ThreadMutex: public Mutex
{
public:
    ThreadMutex()
    {
        pthread_mutex_init(&_m, /*&attr*/0);
    }

    virtual ~ThreadMutex()
    {
        pthread_mutex_destroy(&_m);
    }

    virtual int lock()
    {
        return pthread_mutex_lock(&_m);
    }

    virtual int lock (unsigned long sec)
    {
        struct timespec abstime;
        abstime.tv_sec = sec + (long) time(0);
        abstime.tv_nsec = 0;
        return pthread_mutex_timedlock(&_m, &abstime);
    }

    virtual int trylock ()
    {
        return pthread_mutex_trylock (&_m);
    }

    virtual int unlock()
    {
        return pthread_mutex_unlock(&_m);
    }

    virtual void * m()
    {
        return &this->_m;
    }

protected:
    pthread_mutex_t _m;
};


class SpinMutex: public Mutex
{
public:
    SpinMutex()
    {
        pthread_spin_init (&_m, 0);
    }

    virtual ~SpinMutex()
    {
        pthread_spin_destroy(&_m);
    }

    virtual int lock()
    {
        return pthread_spin_lock (&_m);
    }

    virtual int lock(unsigned long sec)
    {
        time_t b = time(0);
        while (0 != pthread_spin_trylock(&_m)) {
            time_t n = time (0);
            if (n - b >= (long)sec) {
                return -1;
            }
        }
        return 0;
    }

    virtual int trylock()
    {
        return pthread_spin_trylock(&_m);
    }

    virtual int unlock()
    {
        return pthread_spin_unlock(&_m);
    }

    virtual void * m()
    {
        return (void *) &this->_m;
    }

protected:
    pthread_spinlock_t  _m;
};



class Guard
{
public:
    Guard(Mutex *m) :
            _m(m)
    {
        _m->lock();
    }

    ~Guard()
    {
        _m->unlock();
    }

protected:
    Mutex *_m;
};

#endif // __MCE_MUTEX_H__
