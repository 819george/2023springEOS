#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
// #include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/time.h>

#define SHM_SIZE 12

typedef struct {
    int guess;
    char result[8];
} data;

int shmid;
data* s;
pid_t pid;
int max,min=0;

void guess(int signo, siginfo_t *info, void *context) {
    s->guess = (min+max) / 2;
    printf("guess: %d\n",s->guess);
    kill(pid, SIGUSR1);
}

void setrange(){
    if (strcmp(s->result, "bingo") == 0) {
        exit(0);
    } else if (strcmp(s->result, "smaller") == 0) {
        max = s->guess;
    } else if (strcmp(s->result, "bigger") == 0) {
        min = s->guess;
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        printf("Usage:%s <key> <upper_bound> <pid>" ,argv[0]);
        return 1;
    }

    key_t key = atoi(argv[1]);
    max = atoi(argv[2]);
    pid = atoi(argv[3]);
    printf("%d\n",pid);

    int shmid = shmget(key, SHM_SIZE, 0666);
    if (shmid == -1) {
        printf("no shm\n");
        return 1;
    }

    s = (data *)shmat(shmid, NULL, 0);
    if (s == (data *)-1) {
        printf("no shm\n");
        return 1;
    }

    struct sigaction sa;
    struct itimerval timer;

    /* Install timer_handler as the signal handler for SIGVTALRM */
    memset (&sa, 0, sizeof (sa));
    sa.sa_handler = &guess;
    sigaction (SIGALRM, &sa, NULL);

    /* Configure the timer to expire after 250 msec */
    timer.it_value.tv_sec = 0;
    timer.it_value.tv_usec = 250000;

    /* Reset the timer back to 250 msec after expired */
    timer.it_interval.tv_sec = 0;
    timer.it_interval.tv_usec = 250000;

    /* Start a virtual timer */
    setitimer (ITIMER_REAL, &timer, NULL);

    struct  sigaction sig;
    memset (&sig, 0, sizeof(sig));
    sig.sa_handler = &setrange;
    sigaction(SIGUSR1, &sig, 0);

    while (1) {
        sleep(1);
    }

    // 分离共享内存
    shmdt(s);

    return 0;
}
