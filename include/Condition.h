#ifndef __MCE_CONDITION_H__
#define __MCE_CONDITION_H__

#include "Mutex.h"

class Condition
{
public:
	virtual ~Condition()
	{
	}

	virtual int wait(Mutex * mutex)
	{
		return 0;
	}

	virtual int wait(Mutex * mutex, unsigned long sec)
	{
		return 0;
	}

	virtual int singal()
	{
		return 0;
	}

	virtual int broadcast()
	{
		return 0;
	}
};

class ThreadCondition: public Condition
{
public:
	ThreadCondition()
	{
		//pthread_condattr_t attr;
		//pthread_condattr_init (&attr);
		//pthread_condattr_getpshared (&attr, int *pshared);
		//pthread_condattr_setpshared (&attr, int pshared);

		pthread_cond_init(&_c, 0);
		//pthread_condattr_destroy (&attr);
	}

	virtual ~ThreadCondition()
	{
		pthread_cond_destroy(&_c);
	}

	virtual int wait(Mutex * mutex)
	{
		return pthread_cond_wait(&_c, (pthread_mutex_t *) mutex->m());
	}

	virtual int wait(Mutex * mutex, unsigned long sec)
	{
		struct timespec abstime;
		abstime.tv_sec = sec + (long) time(0);
		abstime.tv_nsec = 0;
		return pthread_cond_timedwait(&_c, (pthread_mutex_t *) (mutex->m()),
				&abstime);
	}

	virtual int singal()
	{
		return pthread_cond_signal(&_c);
	}

	virtual int broadcast()
	{
		return pthread_cond_broadcast(&_c);
	}

protected:
	pthread_cond_t _c;
};

#endif // __MCE_CONDITION_H__
