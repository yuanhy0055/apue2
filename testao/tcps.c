#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static int rcport = 23333;

int rclisten() {
	int listensock;
	struct sockaddr_in servaddr;
	int optval = 1;

	//创建TCP套接字
	if ((listensock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		perror("socket error");
		listensock = -1;
	}

	memset(&servaddr, 0, sizeof(servaddr)); //套接字地址变量清0
	servaddr.sin_family = AF_INET;
	servaddr.sin_family = htonl(INADDR_ANY);
	servaddr.sin_port = htons(rcport);

	/* configure the server so that it can be restarted immediately */
	setsockopt(listensock, SOL_SOCKET, SO_REUSEADDR, (const void*) &optval,
			sizeof(int));

	//绑字套接字和套接字地址变量
	if (bind(listensock, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
		perror("bind error");
		listensock = -1;
	}

	//监听套接字  
	if (listen(listensock, 5) < 0) {
		perror("listen error");
		listensock = -1;
	}

	go_back:
	//printf("listensock=%d\n", listensock);
	return listensock;
}
