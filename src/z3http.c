#include "z3http.h"
#include "threadpool.h"

int len_index;

void * work(int i)
{
	int len;
	char buf[BUFSIZE];

	len = read(events[i].data.fd, buf, BUFSIZE);
	if(len == -1 && (errno == EAGAIN || errno == EWOULDBLOCK))
	{
		;
	}
	if(buf[0] == 'q')
	{
		close(events[i].data.fd);
		printf("client %d is closed\n", events[i].data.fd);
		return (void *)-1;
	}

	switch(check_request(buf))
	{
		case 0:
			puts("case 0");
			break;
		case 1:
		{
			dprintf(events[i].data.fd, "HTTP/1.0 200 OK\r\nMIME-Version: 1.0\r\nServer: z3_http/0.1\r\nContent-Type: text/html\r\nContent-Length: %d\r\n\r\n", len_index);
			write(events[i].data.fd, buf_index, len_index);
			if(events[i].data.fd > 4)
			{
				close(events[i].data.fd);
				printf("in case1: events[%d].data.fd is closed\n", events[i].data.fd);
			}
			break;
		}
		case 2:
			break;
		case 3:
			break;
		default:
			puts("case default");
			break;
	}

	return (void *)0;
}

int check_request(char *buf)
{
//	char *get = "G";
	char *get = "GET / HTTP/1.0\r\n";
	int len = strlen(get);
	int i;
	for(i=0; i<len; i++)
	{
		if(get[i] != buf[i])
		{
			break;
		}
	}
	if(i == len)
	{
		return 1;
	}

	return 0;
}


void index_init(void)
{
	int indexfd = open("index.html", O_RDONLY);
	if(indexfd == -1)
	{
		perror("open()");
		exit(1);
	}
	len_index = lseek(indexfd, 0, SEEK_END) - 1;
	lseek(indexfd, 0, SEEK_SET);
	buf_index = malloc(len_index);
	if(-1 == (read(indexfd, buf_index, len_index)))
	{
		perror("read()");
		exit(1);
	}

	close(indexfd);
}
