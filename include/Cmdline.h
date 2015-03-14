#ifndef __MCE_CMDLINE_H__
#define __MCE_CMDLINE_H__

// ----------------------------------------------------------------------------
// 只支持带参数选项 -xxxx yyyy 和不带参数选项 -xxx
#include <stdlib.h>
#include <errno.h>
#include <map>
#include <string>
#include <list>

// ----------------------------------------------------------------------------

class Cmdline
{
public:
	// 初始化命令行参数
	Cmdline(int argc, char * argv[]);

	inline bool has(const char * arg)
	{
		return _map.end() != _map.find(arg);
	}

	// bool，定义了就是true，没定义就是false
	inline bool getBool(const char * arg, bool deft = false)
	{
		return this->has(arg) ? true : deft;
	}

	long int getInt(const char * arg, long int deft = 0)
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return deft;
		}
		errno = 0;
		long int ret = strtol(it->second.c_str(), 0, 10);
		return 0 == errno ? ret : deft;
	}

	long long getLongLong(const char * arg, long long deft = 0)
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return deft;
		}
		errno = 0;
		long long ret = strtoll(it->second.c_str(), 0, 10);
		return 0 == errno ? ret : deft;
	}

	const char * getStr(const char * arg, const char * deft = 0)
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return deft;
		}
		return it->second.c_str();
	}

	template<typename T>
	T getVal(const char * arg, T deft = 0)
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return deft;
		}
		errno = 0;
		long long ret = strtoll(it->second.c_str(), 0, 10);
		return 0 == errno ? static_cast<T>(ret) : deft;
	}

protected:
	typedef std::map<std::string, std::string> OptMap;
	OptMap _map;
};

// ----------------------------------------------------------------------------

class CmdlineList
{
public:
	typedef std::list<std::string> ArgValList;

	CmdlineList(int argc, char * argv[]);
	~CmdlineList()
	{
	}

	inline bool has(const char * arg)
	{
		return _map.end() != _map.find(arg);
	}

	// bool，定义了就是true，没定义就是false
	inline bool getBool(const char * arg, bool deft = false)
	{
		return this->has(arg) ? true : deft;
	}

	long int getInt(const char * arg, long int deft = 0)
	{
		long int ret;
		return 0 == getVals<long int>(arg, &ret, 1, deft) ? deft : ret;
	}

	unsigned int getUInt (const char * arg, unsigned int deft = 0)
	{
		unsigned int ret;
		return 0 == getVals <unsigned int> (arg, &ret, 1, deft) ? deft : ret;
	}


	long long getLongLong(const char * arg, long long deft = 0)
	{
		long long ret;
		return 0 == getVals<long long>(arg, &ret, 1, deft) ? deft : ret;
	}

	uint8_t getU8 (const char * arg, uint8_t deft = 0)
	{
		uint8_t ret;
		return 0 == getVals<uint8_t>(arg, &ret, 1, deft) ? deft : ret;
	}

	unsigned short getUShort(const char * arg, unsigned short deft = 0)
	{
		unsigned short ret;
		return 0 == getVals<unsigned short>(arg, &ret, 1, deft) ? deft : ret;
	}

	std::string getStr(const char * arg, const char * deft = 0)
	{
		std::string ret;
		return 0 == getVals(arg, &ret, 1, deft) ? deft : ret;
	}

	template<typename T>
	size_t getVals(const char * arg, T * out, size_t inSize = 1,
			const T & deft = 0)
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return 0;
		}

		size_t i = 0;
		ArgValList::iterator itList = it->second.begin();
		for (; (i < inSize) && (it->second.end() != itList); ++itList, ++i) {
			errno = 0;
			long long val = strtoll(itList->c_str(), 0, 10);
			val = (0 == errno) ? static_cast<T>(val) : deft;
			out[i] = val;
		}

		return i;
	}

	size_t getVals(const char * arg, std::string * out, size_t inSize = 1,
			const std::string & deft = "")
	{
		OptMap::iterator it = _map.find(arg);
		if (_map.end() == it || (0 == it->second.size())) {
			return 0;
		}

		size_t i = 0;
		ArgValList::iterator itList = it->second.begin();
		for (; (i < inSize) && (it->second.end() != itList); ++itList, ++i) {
			errno = 0;
			out[i] = *itList;
		}
		return i;
	}

protected:
	typedef std::map<std::string, ArgValList> OptMap;
	OptMap _map;
};

// ----------------------------------------------------------------------------

#endif // __MCE_CMDLINE_H__
