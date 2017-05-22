#ifndef Z3HTTP_H_
#define Z3HTTP_H_


#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/epoll.h>
#include <string.h>
#include <signal.h>

#define LISTENQ		1024
#define MAXEVENTS	1024
#define BUFSIZE		512

struct epoll_event *events;
char *buf_index;
//char *buf_index = NULL;		//赋值会报错

void * work(int i);
int check_request(char *buf);
void index_init(void);


#endif
