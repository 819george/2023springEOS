#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
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

int passivesock(const char* service, const char* transport, int qlen)
{
    // Store service entry return from getservbyname()
    struct servent *pse;        
    // Service-end socket
    struct sockaddr_in sin;   
    // Service-end socket descriptor and service type
    int s, type;              
    memset(&sin, 0, sizeof(sin));
    // TCP/IP suite
    sin.sin_family = AF_INET; 
    // Use any local IP, need translate to internet byte order
    sin.sin_addr.s_addr = INADDR_ANY; 
    // Get port number
    // service is service name
    if ((pse = getservbyname(service, transport)))
        // sin.sin_port = htons(ntohs((unsigned short)pse->s_port) + portbase);
         sin.sin_port = htons(ntohs((unsigned short)pse->s_port));
    // service is port number
    else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
        exit(printf("can't get \"%s\" service entry\n", service));
        
    // Tranport type
    if (strcmp(transport, "udp") == 0 || strcmp(transport, "UDP") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, 0);
    if (s < 0)
        exit(printf("can't create socket: %s \n", strerror(errno)));

    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
    // Bind socket to service-end address
    if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
        exit(printf("can't bind to %s port: %s \n", service, strerror(errno)));
    // For TCP socket, convert it to passive mode
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        exit(printf("can't listen on %s port: %s \n", service, strerror(errno)));
    return s;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
            fprintf(stderr, "Usage: %s <port>\n", argv[0]);
            exit(1);
    }
    int n; 
    char locations[9][30] = {
        " Baseball Stadium ",
        " Big City ",
        " Neiwan Old Street ",
        " NYCU ",
        " Smangus ",
        " 17 km of Splendid Coastline ",
        " 100 Tastes ",
        " Science Park ",
        " City God Temple "
    };
    char l[9][30] = {
        "Baseball Stadium ",
        "Big City ",
        "Neiwan Old Street ",
        "NYCU ",
        "Smangus ",
        "17 km of Splendid Coastline ",
        "100 Tastes ",
        "Science Park ",
        "City God Temple "
    };
    char ll[9][30] = {
        " Baseball Stadium",
        " Big City",
        " Neiwan Old Street",
        " NYCU",
        " Smangus",
        " 17 km of Splendid Coastline",
        " 100 Tastes",
        " Science Park",
        " City God Temple"
    };
    char people[3][6] = {
        "Child",
        "Adult",
        "Elder"
    };
    int loc_p[9][3] = { 0 };
    char buffer[2048], s[1024];
    char wait[]={"Please wait a few seconds...\n"};
    int exit_count=0;

    int listenfd, connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen;

    clilen = sizeof(cliaddr);
    listenfd = passivesock(argv[1], "tcp", 1);  // port tcp accept-number
    printf("Listening on port %s...\n", argv[1]);
    
    char menu[] = "1. Search\n2. Report\n3. Exit\n";
    while(1){
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);

        memset(loc_p, 0, sizeof(loc_p));
        n = read(connfd, buffer, sizeof(buffer)-1);
        if (strcmp(buffer, "Menu") == 0) {
            n = write(connfd, menu, strlen(menu)+1);
            while (1) {
                n = read(connfd, buffer, sizeof(buffer) - 1);
                buffer[strcspn(buffer, "\n")] = 0; 
                char *tok = strtok(buffer, "|");
                //search
                if (strcmp(tok, "Search") == 0) {
                    tok = strtok(0, "|");

                    if(!tok){
                        sprintf(s, "1. Baseball Stadium : %d\n2. Big City : %d\n3. Neiwan Old Street : %d\n4. NYCU : %d\n5. Smangus : %d\n6. 17 km of Splendid Coastline : %d\n7. 100 Tastes : %d\n8. Science Park : %d\n9. City God Temple : %d\n",loc_p[0][0]+loc_p[0][1]+loc_p[0][2],loc_p[1][0]+loc_p[1][1]+loc_p[1][2],loc_p[2][0]+loc_p[2][1]+loc_p[2][2],loc_p[3][0]+loc_p[3][1]+loc_p[3][2],loc_p[4][0]+loc_p[4][1]+loc_p[4][2],loc_p[5][0]+loc_p[5][1]+loc_p[5][2],loc_p[6][0]+loc_p[6][1]+loc_p[6][2],loc_p[7][0]+loc_p[7][1]+loc_p[7][2],loc_p[8][0]+loc_p[8][1]+loc_p[8][2]);
                        write(connfd, s, strlen(s)+1);
                    }
                }
                //search place
                else if (strcmp(tok, "Search ") == 0){
                    char *p = strtok(0,"|");
                    //printf("-%s-", p);
                    int i=-1,loc=-1;
                    // find location number
                    for(i=0;i<9;i++){
                        if(strcmp(ll[i],p)==0){
                            loc=i;
                            break;
                        }
                    }
                    sprintf(s, "%s- Child : %d Adult : %d Elder : %d\n", l[i], loc_p[loc][0], loc_p[loc][1], loc_p[loc][2]);
                    printf("%s",s);
                    write(connfd, s, strlen(s)+1);
                } 
                //menu
                else if (strcmp(tok, "Menu") == 0) {
                    n = write(connfd, menu, strlen(menu)+1);
                    break;
                }
                //report
                else if (strcmp(tok, "Report ") == 0) {
                    int max_loc=-1,count=0;
                    char ret[1024];
                    memset(ret,0,sizeof(ret));

                    write(connfd, wait, strlen(wait)+1);
                    while(tok!=NULL){
                        count++;
                        char *p = strtok(0,"|");
                        if(!p) break;
                        int i,loc=-1,pt=-1;
                        char *a;
                        printf(" %s",p);
                        for(i=0;i<9;i++){
                            if(strcmp(locations[i], p)==0){
                                loc=i;
                                if(max_loc<loc) max_loc=loc;
                                break;
                            }
                            if(strcmp(l[i], p)==0){
                                loc=i;
                                if(max_loc<loc) max_loc=loc;
                                break;
                            }
                            if(strcmp(ll[i], p)==0){
                                loc=i;
                                if(max_loc<loc) max_loc=loc;
                                break;
                            }
                        }
                        char *t=strtok(0," ");
                        for(i=0;i<3;i++){
                            if(strcmp(people[i],t)==0){
                                pt=i;
                                a=strtok(NULL," ");
                                break;
                            }
                        }
                        loc_p[loc][pt]+=atoi(a);
                        sprintf(ret+strlen(ret), "%s| %s %s\n",l[loc],t,a);
                        printf("%s| %s %s sleep:%d loc:%d\n",p ,t ,a ,max_loc+1, loc);
                    }
                    sleep(max_loc+1);
                    write(connfd, ret, strlen(ret)+1);
                    memset(ret,0,sizeof(ret));
                } else if (strcmp(tok, "Exit") == 0) {
                    close(connfd);
                    break;
                }
                else
                    printf("-NULL-\n");
            }
        }
    }
    return 0;
}
