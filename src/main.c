#include "z3http.h"
#include "threadpool.h"

int main(int argc, char **argv)
{
	if(argc != 2)
	{
		puts("usage: ./a.out [PORT]");
		return 0;
	}

	int listenfd;
	struct sockaddr_in serveraddr;
	
	int s = 0;

	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if(listenfd == -1)
	{
		perror("socket()");
		exit(1);
	}
	memset(&serveraddr, 0, sizeof(serveraddr));
	serveraddr.sin_family = AF_INET;
	serveraddr.sin_addr.s_addr = htonl(INADDR_ANY);
	serveraddr.sin_port = htons(atoi(argv[1]));

	if(-1 == bind(listenfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)))
	{
		perror("bind()");
		exit(1);
	}

	int flags;
	flags = fcntl(listenfd, F_GETFL, 0);
	if(-1 == flags)
	{
		perror("fcntl()");
		close(listenfd);
		exit(2);
	}
	else
		flags |= O_NONBLOCK;
	s = fcntl(listenfd, F_SETFL, flags);
	if(-1 == s) {
		perror("fcntl()");
		close(listenfd);
		exit(2);
	}

	s = listen(listenfd, LISTENQ);
	if(-1 == s)
	{
		perror("listen()");
		close(listenfd);
		exit(2);
	}


	int epollfd;
	struct epoll_event event;
	epollfd = epoll_create1(0);
	if(-1 == s)
	{
		perror("epoll_create1()");
		close(listenfd);
		exit(2);
	}
	event.data.fd = listenfd;
	event.events = EPOLLIN | EPOLLET;
	s = epoll_ctl(epollfd, EPOLL_CTL_ADD, listenfd, &event);
	if(-1 == s)
	{
		perror("epoll_ctl()");
		close(listenfd);
		close(epollfd);
		exit(2);
	}

	events = calloc(MAXEVENTS, sizeof(event));

	int n;
	int i;
	int infd;

	index_init();
	struct threadpool *pool = threadpool_init(8, 32);


	signal(SIGPIPE, SIG_IGN);

	while(1)
	{
		n = epoll_wait(epollfd, events, MAXEVENTS, -1);
		if(n == 0) {
			break;
		}
		printf("there`s %d clients want to do something. ", n);
		for(i=0; i<n; i++) {
			if(listenfd == events[i].data.fd)
			{
				printf("client %d want to connect\n", events[i].data.fd);
				while(1)
				{
					struct sockaddr in_addr;
					socklen_t in_len;
					
					infd = accept(listenfd, &in_addr, &in_len);
					printf("%d\n", infd);
					if(infd == -1) {
						if(errno == EAGAIN || errno == EWOULDBLOCK)
						{
							break;
						}
						else 
						{
							perror("accept()");
							break;
						}
					}
					else
					{
						flags = fcntl(infd, F_GETFL, 0);
						flags |= O_NONBLOCK;
						fcntl(infd, F_SETFL, flags);

						event.data.fd = infd;
					//	event.events = EPOLLIN | EPOLLET;
						event.events = EPOLLIN | EPOLLET | EPOLLONESHOT;
						s = epoll_ctl(epollfd, EPOLL_CTL_ADD, infd, &event);
						if(s == -1)
						{
							perror("epoll_ctl() 2");
							abort ();
						}
						printf("infd%d/envets[%d] is connected\n", infd, infd);
					}
				}//while
			}//if 
			else
			{
				printf("client %d want to get index.html\n", events[i].data.fd);
				threadpool_add_job(pool, work, i);
			}
		}//for
	}//while

	free(events);
	free(buf_index);
	close(listenfd);

	exit(0);
}

