#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <fcntl.h>

#include "../include/Daemon.h"

int init_daemon(int changedir, int closefd, int redirstdio)
{
	int pid = -1;
	if ((pid = fork ())){
		exit(0); //是父进程，结束父进程
	}

	else if (pid < 0) {
		exit(1); //fork失败，退出
	}

	//是第一子进程，后台继续执行
	setsid(); 		//第一子进程成为新的会话组长和进程组长
	//并与控制终端分离
	if ((pid = fork())) {
		exit(0); 	//是第一子进程，结束第一子进程
	}
	else if (pid < 0) {
		exit(1); 	//fork失败，退出
	}

	//是第二子进程，继续
	//第二子进程不再是会话组长
	//改变工作目录到/tmp
	if (changedir && 0 != chdir("/tmp")) {
		return -1;
	}

	if (redirstdio) {
		redir_stdio ();
	}

	if (closefd) {
		for (int i = STDERR_FILENO + 1; i < NOFILE; ++i) {	//关闭打开的文件描述符
			close(i);
		}
	}
	umask(0); 		//重设文件创建掩模
	return 0;
}

int redir_stdio ()
{
	// 打开空洞文件
	int fd = open ("/dev/null", O_RDWR);
	if (fd > 0) {
		for (int i = STDIN_FILENO; i <= STDERR_FILENO; i ++) {
			if (dup2 (fd, i) < 0) {
				close (i);
			}
		}
		close (fd);
	}
	else {
		close (STDIN_FILENO);
		close (STDOUT_FILENO);
		close (STDERR_FILENO);
		return -1;
	}
	return 0;
}

bool save_pid (const char * pidfile)
{
	FILE *f = fopen(pidfile, "w");
	if (!f) {
		return false;
	}
	char buf[256];
	sprintf(buf, "%d", getpid());
	fputs(buf, f);
	fclose(f);
	f = 0;
	return true;
}

void unlink_pid (const char * pidfile)
{
	unlink (pidfile);
}
