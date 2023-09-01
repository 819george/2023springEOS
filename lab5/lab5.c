#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "sockop.h"

pid_t childpid;
int passivesock(const char *service, const char *transport, int qlen);
void reaper(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0);
}
int main(int argc, char *argv[]){
    int listenfd, connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    listenfd = passivesock(argv[1], "tcp", 5);

    (void) signal(SIGCHLD, reaper);

    printf("Listening on port %s...\n", argv[1]);

    while(1){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0) {
			if (errno == EINTR)
				continue;
			errexit("accept: %s\n", strerror(errno));
		}

        switch (childpid=fork()) {
		case 0:		// child  
            dup2(connfd, STDOUT_FILENO);
            execlp("sl", "sl", "-l", NULL);
            close(connfd);

            //close(listenfd);
			exit(0);
		default:	// parent 
            printf("child pid:%d\n",childpid);
			break;
		case -1:
			errexit("fork: %s\n", strerror(errno));
		}
    }
    return 0;
}