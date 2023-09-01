#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>

#define SHM_SIZE 12

typedef struct {
    int guess;
    char result[8];
} data;

int shmid;
data* s;
pid_t pid;
int answer;

void my_handler(int signo, siginfo_t *info, void *context)
{
    // Compare guess with the guess number and update result
    if (s->guess < answer) {
        strcpy(s->result, "bigger");
    } else if (s->guess > answer) {
        strcpy(s->result, "smaller");
    } else {
        strcpy(s->result, "bingo");
    }
    printf("guess: %d, %s\n",s->guess,s->result);
    // Notify the Guess program that the judgment is complete
    kill(info->si_pid, SIGUSR1);
}

void cleanup()
{
    // Detach and remove shared memory segment
    shmdt(s);
    shmctl(shmid, IPC_RMID, NULL);
}

int main(int argc, char* argv[])
{
    if (argc < 3) {
        printf("Usage: %s <key> <guess>\n", argv[0]);
        return 1;
    }

    // Get the shared memory key
    key_t key = atoi(argv[1]);
    printf("Game PID: %d\n",pid=getpid());

    // Create or attach to the shared memory segment
    shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
    if (shmid == -1) {
        perror("shmget");
        return 1;
    }

    // Attach to the shared memory segment
    s = (data*)shmat(shmid, NULL, 0);
    if (s == (void*)-1) {
        perror("shmat");
        return 1;
    }

    // Store the guess number and initialize result
    answer = atoi(argv[2]);
    strcpy(s->result, "");

    // Set up SIGUSR1 handler
    struct sigaction sa;
    memset(&sa, 0, sizeof(sa));
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = &my_handler;
    sigaction(SIGUSR1, &sa, NULL); 

    struct timespec req;
    int retval;
    /* set the sleep time to ? sec */
    memset(&req, 0, sizeof(struct timespec));
    req.tv_sec = 30;
    req.tv_nsec = 0;

    do{
        retval = nanosleep(&req, &req);
    } while(retval);

    cleanup();

    return 0;
}
