#ifndef __MCE_SINGLETON_H__
#define __MCE_SINGLETON_H__


template <typename T>
class Singleton
{
public:
    static T * inst ()
    {
        if (!_t) {
            _t = new T;
        }
        return _t;
    }

    static T * inst (T * t)
    {
    	if (_t) {
    		delete _t;
    	}
    	_t = t;
    	return _t;
    }

    static void release ()
    {
        if (_t) {
            delete _t;
            _t = 0;
        }
    }

protected:
    static T * _t;
};

template <typename T>
T * Singleton<T>::_t = 0;




template <typename T>
class SingletonPtr
{
public:
    static T * inst ()
    {
        return _t;
    }

    static T * inst (T * t)
    {
//    	if (_t) {
//    		delete _t;
//    	}
    	_t = t;
    	return _t;
    }

    static void release ()
    {
        if (_t) {
            delete _t;
            _t = 0;
        }
    }

protected:
    static T * _t;
};

template <typename T>
T * SingletonPtr<T>::_t = 0;



#endif // __MCE_SINGLETON_H__
