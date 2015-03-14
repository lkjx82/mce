#ifndef __MCE_DAEMON_H__
#define __MCE_DAEMON_H__

int init_daemon (int changedir, int closefd, int redirstdio = 1);

int redir_stdio ();

bool save_pid (const char * pidfile);

void unlink_pid (const char * pidfile);

#endif // __MCE_DAEMON_H__
