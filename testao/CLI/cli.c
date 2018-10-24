#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>

//#include "externs.h"

#define BUFSIZE 1024

//#define fprintf(err, ...) YLOG(__VA_ARGS__)
//#define printf(...) YLOG(__VA_ARGS__)

static int sockfd;

//================================
int main(int argc, char *argv[])
{
    struct sockaddr_in serveraddr;
    struct hostent *server;
    char buf[BUFSIZE];
    int nread,i;

    printf("-->%s:%d", __FUNCTION__, __LINE__);

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    server = gethostbyname("11.1.1.77");

    /* build the server's Internet address */
    bzero((char *) &serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serveraddr.sin_addr.s_addr, server->h_length);
    serveraddr.sin_port = htons(23333);

    if (connect(sockfd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) < 0)
        printf("ERROR connecting");

    if(argc==2) {
    if(strcmp(argv[1], "P")==0)
            snprintf(buf, 7, "%s", "^play^");
    if(strcmp(argv[1], "S")==0)
            snprintf(buf, 7, "%s", "^stop^");
    if(strcmp(argv[1], "K")==0)
            snprintf(buf, 7, "%s", "^paus^");
    } else {
        snprintf(buf, 7, "%s", "^stat^");
    }

    printf("write %d bytes\n", write(sockfd, buf, strlen(buf)));
    nread = read(sockfd, buf, 128);
    buf[nread]=0;
    printf("read %d bytes{%s}\n", nread, buf);
/*
    for (i=0; i<nread; i++) {
      putchar(buf[i]);
    }
    putchar('\n');
*/

    close(sockfd);

	return 0;
}

