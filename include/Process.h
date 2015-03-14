#ifndef __MCE_PROCESS_H__
#define __MCE_PROCESS_H__

#include <sys/unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

class Process
{
public:
	pid_t exec (const char * file, char * argv[])
	{
		return ::execv (file, argv);
	}

	bool wait ()
	{
		return false;
	}

};


#endif // __MCE_PROCESS_H__
