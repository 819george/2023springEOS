#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h> // Network related functions, e.g. gethostbyname()
#include <netinet/in.h> // struct sockaddr_in 
#include <sys/socket.h> // system socket define
#include <sys/types.h> // system types
#include <sys/wait.h>

#define errexit(format,arg...) exit(printf(format,##arg))
#define SHMSZ 27

#define SEM_MODE 0666 /* rw(owner)-rw(group)-rw(other) permission */
#define SEM_KEY 17

/* P () - returns 0 if OK; -1 if there was a problem */
int P (int s)
{
    struct sembuf sop; /* the operation parameters */
    sop.sem_num = 0; /* access the 1st (and only) sem in the array */
    sop.sem_op = -1; /* wait..*/
    sop.sem_flg = 0; /* no special options needed */
    if (semop (s, &sop, 1) < 0) {
    fprintf(stderr,"P(): semop failed: %s\n",strerror(errno));
    return -1;
    } else {
    return 0;
    }
}
/* V() - returns 0 if OK; -1 if there was a problem */
int V(int s)
{
    struct sembuf sop; /* the operation parameters */
    sop.sem_num = 0; /* the 1st (and only) sem in the array */
    sop.sem_op = 1; /* signal */
    sop.sem_flg = 0; /* no special options needed */
    if (semop(s, &sop, 1) < 0) {
    fprintf(stderr,"V(): semop failed: %s\n",strerror(errno));
    return -1;
    } else {
    return 0;
    }
}

int sem;
pid_t childpid;
int shmid;
int *shm;

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
        errexit("can't get \"%s\" service entry\n", service);
    // Tranport type
    if (strcmp(transport, "udp") == 0 || strcmp(transport, "UDP") == 0)
        type = SOCK_DGRAM;
    else
        type = SOCK_STREAM;

    s = socket(PF_INET, type, 0);
    if (s < 0)
        errexit("can't create socket: %s \n", strerror(errno));
    
    int yes = 1;
    setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));
    
    // Bind socket to service-end address
    if (bind(s, (struct sockaddr*)&sin, sizeof(sin)) < 0)
        errexit("can't bind to %s port: %s \n", service, strerror(errno));
    // For TCP socket, convert it to passive mode
    if (type == SOCK_STREAM && listen(s, qlen) < 0)
        errexit("can't listen on %s port: %s \n", service, strerror(errno));
    return s;
}

void reaper(int sig){
    while (waitpid(-1, NULL, WNOHANG) > 0);
}

void dt(int sig){
    /* remove semaphore */
    if (semctl (sem, 0, IPC_RMID, 0) < 0)
    {
    fprintf (stderr, "unable to remove sem %ld\n", SEM_KEY);
    exit(1);
    }
    printf("Semaphore %ld has been remove\n", SEM_KEY);

    int retval;
    /* Detach the share memory segment */
    shmdt(shm);
    /* Destroy the share memory segment */
    printf("Server destroy the share memory.\n");
    retval = shmctl(shmid, IPC_RMID, NULL);
    if (retval < 0)
    {
    fprintf(stderr, "Server remove share memory failed\n");
    exit(1);
    }
}

int main(int argc, char *argv[]){
    
    char buffer[10];
    int listenfd, connfd;
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        exit(1);
    }
    listenfd = passivesock(argv[1], "tcp", 2000);

    (void) signal(SIGCHLD, reaper);
    (void) signal(SIGINT, dt);

    printf("Listening on port %s...\n", argv[1]);

    /* create semaphore */
    sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
    if (sem < 0)
    {
    fprintf(stderr, "Sem %ld creation failed: %s\n", SEM_KEY,
    strerror(errno));
    exit(-1);
    }
    /* initial semaphore value to 1 (binary semaphore) */
    if ( semctl(sem, 0, SETVAL, 1) < 0 )
    {
    fprintf(stderr, "Unable to initialize Sem: %s\n", strerror(errno));
    exit(0);
    }
    printf("Semaphore %ld has been created & initialized to 1\n", SEM_KEY);

    key_t key;
    int m;
    /* We'll name our shared memory segment "5678" */
    key = 10;
    /* Create the segment */
    if ((shmid = shmget(key, SHMSZ, IPC_CREAT | 0666)) < 0) {
    perror("shmget");
    exit(1);
    }
    /* Now we attach the segment to our data space */
    if ((shm = shmat(shmid, NULL, 0)) == (int *) -1) {
    perror("shmat");
    exit(1);
    }
    *shm=0;

    while(1){
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &clilen);
        if (connfd < 0) {
			if (errno == EINTR)
				continue;
            close(connfd);
			errexit("accept: %s\n", strerror(errno));
		}
        switch (childpid=fork()) {
		case 0:		// child  
            /* clear */
            memset(buffer, 0, 10);
            /* read raw data from file */
            read(connfd, buffer, 10);

            P(sem);
            /**************** Critical Section *****************/
            m=atoi(buffer);
            *shm += m;
            if(m>0) printf("After deposit: %d\n",*shm);
            else printf("After withdraw: %d\n",*shm);
            /**************** Critical Section *****************/
            V(sem);
            close(connfd);
			exit(0);

		default:	// parent 
			break;

		case -1:
			errexit("fork: %s\n", strerror(errno));
		}
    }
    
    return 0;
}