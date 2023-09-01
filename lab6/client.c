#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h> // Network related functions, e.g. gethostbyname()
#include <netinet/in.h> // struct sockaddr_in 
#include <sys/socket.h> // system socket define
#include <sys/types.h> // system types
#include <sys/wait.h>

#define errexit(format,arg...) exit(printf(format,##arg))

#define BUFSIZE 1024

int connectsock(const char *host, const char *service, const char *transport)
{
    // struct hostent *phe;    // 主機條目
    struct servent *pse;    // 服務條目
    struct sockaddr_in sin; // Internet端點地址
    int s, type;            // 套接字描述符和套接字類型

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;

    // 查找服務條目
    if ((pse = getservbyname(service, transport)))
        sin.sin_port = pse->s_port;
    else if ((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        return -1;

    // 使用協議創建套接字
    if (strcmp(transport, "udp") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    if ((s = socket(PF_INET, type, 0)) < 0)
        return -1;

    // 連接到服務器
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
        return -1;

    return s;
}

int main(int argc, char *argv[])
{
    int connfd;/* socket descriptor*/
    int n,times;

    if (argc != 6) {
        fprintf(stderr, "Usage: %s <ip> <port> <deposit/withdraw> <amount> <times>\n", argv[0]);
        exit(1);
    }

    times=atoi(argv[5]);
    char wrt[10]="-";
    while(times--){
        connfd = connectsock(argv[1], argv[2], "tcp");
        printf("Connected to %s:%s.\n", argv[1], argv[2]);
        memset(wrt, 0, 10);
        if(strcmp(argv[3], "deposit") == 0)
            n = write(connfd, argv[4], strlen(argv[4]));
        else if(strcmp(argv[3], "withdraw") == 0){
            wrt[0]='-';
            strcat(wrt,argv[4]);

            n = write(connfd, wrt, strlen(wrt));
            //printf("%s\n", wrt);
        }
        if (n < 0) {
            perror("Error writing to socket");
            exit(1);
        }
        close(connfd);
    }
    return 0;
}

