#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>

int main() {
    int pipefd[2];  // Pipe: pipefd[0] for reading, pipefd[1] for writing
    pid_t pid1, pid2;

    if (pipe(pipefd) == -1) {
        perror("pipe failed");
        exit(EXIT_FAILURE);
    }

    // Create the first child process for A
    pid1 = fork();
    if (pid1 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0) {
        // Child process A: Redirects its output to the pipe's write end
        close(pipefd[0]); // Close the read end of the pipe
        dup2(pipefd[1], STDOUT_FILENO); // Redirect stdout to the write end of the pipe
        close(pipefd[1]); // Close the original write end of the pipe

        execlp("./A", "./A", NULL);
        perror("execlp A failed");
        exit(EXIT_FAILURE);
    }

    // Create the second child process for B
    pid2 = fork();
    if (pid2 < 0) {
        perror("fork failed");
        exit(EXIT_FAILURE);
    }

    if (pid2 == 0) {
        // Child process B: Redirects its input from the pipe's read end
        close(pipefd[1]); // Close the write end of the pipe
        dup2(pipefd[0], STDIN_FILENO); // Redirect stdin to the read end of the pipe
        close(pipefd[0]); // Close the original read end of the pipe

        // Open the file b.txt for writing (append mode)
        int fd = open("b.txt", O_WRONLY | O_CREAT | O_APPEND, 0644);
        if (fd < 0) {
            perror("open failed");
            exit(EXIT_FAILURE);
        }

        // Redirect stdout to the file
        dup2(fd, STDOUT_FILENO);
        close(fd); // Close the original file descriptor

        execlp("./B", "./B", NULL);
        perror("execlp B failed");
        exit(EXIT_FAILURE);
    }

    // Parent process closes both ends of the pipe
    close(pipefd[0]);
    close(pipefd[1]);

    // Wait for both child processes to finish
    waitpid(pid1, NULL, 0);
    waitpid(pid2, NULL, 0);

    return 0;
}
