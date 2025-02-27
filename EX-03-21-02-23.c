#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <signal.h>

#define MAX_LINE 100
#define FILENAME "data.txt"

int pipe_fd[2];
pid_t child_pid;
int start_line = 0;

// Signal handler for SIGCHLD - triggered when the child process terminates
void handle_sigchld(int sig) {
    int status;
    pid_t pid;

    // Ensure that all terminated child processes are collected
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0)
    {
        if (pid == child_pid) 
        {
            printf("Child %d exited. Restarting...\n", pid);
            
            // Create a new child process to continue from the same line
            child_pid = fork();
            if (child_pid == 0)
            {
                close(pipe_fd[0]); // Close the read end of the pipe

                FILE *file = fopen(FILENAME, "r");
                if (!file) {
                    perror("fopen");
                    exit(1);
                }
                
                char line[MAX_LINE];
                int current_line = 0;
                
                while (fgets(line, MAX_LINE, file)) {
                    if (current_line >= start_line) {
                        write(pipe_fd[1], line, strlen(line) + 1);
                        sleep(1); // Simulate processing delay
                    }
                    current_line++;
                }
                
                fclose(file);
                close(pipe_fd[1]);
                exit(0);
            }
        }
    }
}

int main() {
    if (pipe(pipe_fd) == -1) {
        perror("pipe");
        exit(1);
    }

    // Register the SIGCHLD signal handler using signal()
    signal(SIGCHLD, handle_sigchld);

    // Create the first child process
    child_pid = fork();

if (child_pid == 0) // child process
{
    close(pipe_fd[0]); // close write end.

    FILE *file = fopen(FILENAME, "r");
    if (!file)
    {
        perror("fopen");
        exit(1);
    }

    char line[MAX_LINE];
    int current_line = 0;

    while (fgets(line, MAX_LINE, file)) 
    {
        if (current_line >= start_line)
        {
            write(pipe_fd[1], line, strlen(line) + 1);
            sleep(1);
        }
        current_line++;
    }
    fclose(file);
    close(pipe_fd[1]);
    exit(0);
}

    close(pipe_fd[1]); // Parent closes the write end of the pipe
    FILE *file = fopen(FILENAME, "a");
    if (!file) {
        perror("fopen");
        exit(1);
    }

    char line[MAX_LINE];
    while (1) {
        if (read(pipe_fd[0], line, MAX_LINE) > 0)
        {
            fprintf(file, "My son said: %s", line);
            fflush(file);
            start_line++;
        }
    }

    fclose(file);
    close(pipe_fd[0]);
    return 0;
}
