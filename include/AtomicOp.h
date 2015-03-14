// ----------------------------------------------------------------------------

#include "Mutex.h"

// ----------------------------------------------------------------------------

template<typename T>
class AtomicOp
{
public:
    AtomicOp(const T & t) :
            _t(0)
    {
        Guard g(&_m);
        _t = t;
    }

    T & operator -=(const T & r)
    {
        Guard g(&_m);
        _t -= r;
        return _t;
    }

    T & operator +=(const T & r)
    {
        Guard g(&_m);
        _t += r;
        return _t;
    }

    T & operator ++()
    {
        Guard g(&_m);
        _t++;
        return _t;
    }

    T & operator --()
    {
        Guard g(&_m);
        _t--;
        return _t;
    }

    bool operator ==(const T & r)
    {
        Guard g(&_m);
        return _t == r ? true : false;
    }

    bool operator !=(const T & r)
    {
        Guard g(&_m);
        return _t != r ? true : false;
    }

    T & operator =(const T & r)
    {
        Guard g(&_m);
        _t = r;
        return _t;
    }

    void lock()
    {
        _m.lock();
    }

    void unlock()
    {
        _m.unlock();
    }

protected:
    AtomicOp() :
            _t(0)
    {
    }


    T _t;
    SpinMutex _m;
};

// ----------------------------------------------------------------------------

template<typename T>
inline bool operator ==(T t, const AtomicOp<T> & r)
{
    return r == t;
}

// ----------------------------------------------------------------------------

template<typename T>
inline bool operator !=(T t, const AtomicOp<T> & r)
{
    return r != t;
}

// -----------------------------------------------------------------------------
