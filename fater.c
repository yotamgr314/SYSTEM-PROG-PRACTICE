#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

pid_t child_pid; // Global variable to track child process

// Signal handler: gets called when the child process dies
void signalHandler(int signalNumber) 
{
    if (signalNumber == SIGCHLD) 
    {
        int status;
        pid_t pid = waitpid(child_pid, &status, WNOHANG); // Non-blocking waitpid for all childrens death.

        if (pid > 0)
        {
            printf("Child process (PID %d) terminated. Restarting...\n", child_pid);

            child_pid = fork();
            if (child_pid == 0) {
                execl("./my_son.exe", "my_son.exe", NULL);
                perror("execl failed");
                exit(EXIT_FAILURE);
            }
        }
}
}

int main(void) {
    // Set up SIGCHLD signal handler using signal()
    signal(SIGCHLD, signalHandler);

    // First child process
    child_pid = fork();

    if (child_pid == 0)
    {
        execl("./my_son.exe", "my_son.exe", NULL);
        perror("execl failed");
        exit(EXIT_FAILURE);

    } else if (child_pid < 0) 
    {
        perror("fork failed");
        exit(EXIT_FAILURE);
    } else
    {
        printf("Child process started with PID: %d\n", child_pid);
    }

    // Parent does other things while child is running
    while (1) {
        printf("Parent is running...\n");
        sleep(5); // Simulate work (Parent is not blocked!)
    }

    return 0;
}
