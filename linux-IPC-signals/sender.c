// sender.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <PID>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    pid_t receiver_pid = (pid_t)atoi(argv[1]);
    printf("Sending SIGUSR1 to PID: %d", receiver_pid);

    if (kill(receiver_pid, SIGUSR1) == -1) {
        perror("kill");
        exit(EXIT_FAILURE);
    }

    printf("Signal sent.\n");
    return 0;
}