#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <sys/select.h>
#include <sys/time.h>
#include <unistd.h>
#include <sys/types.h>

#define SERV_PORT 8000
#define MAX_SIZE 1024*50

#if 0
static int vssf(char *buffer, char *fmt, ...)
{
va_list argptr;
int cnt;
fflush(stdin);
va_start(argptr, fmt);
cnt = vsscanf(buffer, fmt, argptr);
va_end(argptr);
return(cnt);
}
#endif

int main(int argc, char *argv[])
{
	int sockfd;
	struct sockaddr_in servaddr;
	//---------------START-------------------------
	int n;
	int pkg=0;	
	socklen_t address_size;
	char buf[MAX_SIZE];
	int a,b,c;

	//daemon(0, 0);
	//------------------END------------------------
	sockfd = socket(AF_INET,SOCK_DGRAM,0);

	bzero(&servaddr,sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	if(bind(sockfd,(struct sockaddr *)&servaddr,sizeof(servaddr)) == -1) {
		perror("bind error");
		exit(-1);
	}

	//---------------START-------------------------
	address_size = sizeof(servaddr);
	while(1) {
		printf("pkg=%d\n", pkg);
		memset(buf,0,MAX_SIZE);
		n = recvfrom(sockfd, buf, MAX_SIZE, 0, (struct sockaddr *)&servaddr, &address_size);
		if (-1 == n) {
			perror("call to recvfrom.\n");
			exit(1);
		}

		printf("recv [%d] byte, buf:{%s}\n", n, buf);
		sscanf(buf, "%d,%d,%d", &a, &b, &c);
		printf("Count %d+%d+%d=%d\n", a, b, c, a+b+c);
		sprintf(buf, "SUM=%d", a+b+c);
		sendto(sockfd, buf, (int)strlen(buf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

		pkg++;
	}

	//------------------END------------------------

	return 0;

}
