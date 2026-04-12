// receiver.c
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>

volatile sig_atomic_t signal_received = 0;

// Signal handler
void handle_signal(int sig) {
    if (sig == SIGUSR1) {
        printf("Received SIGUSR1!\n");
        signal_received = 1;
    }
}

int main() {
    pid_t pid = getpid();
    printf("Receiver PID: %d", pid);

    // Set up the signal handler
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sa.sa_flags = 0; // or SA_RESTART
    sigemptyset(&sa.sa_mask);

    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        exit(EXIT_FAILURE);
    }

    printf("Waiting for signals...\n");

    // Wait for the signal
    while (!signal_received) {
        pause(); // Sleep until a signal arrives
    }

    printf("Exiting receiver.\n");
    return 0;
}
