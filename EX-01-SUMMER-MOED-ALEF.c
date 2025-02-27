#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

typedef struct {
    int fd[2]; // fd[0]: for reading (sem_wait), fd[1]: for writing (sem_post)
} sem_t;

void sem_init(sem_t *sem, int num_of_tokens) 
{
    if (pipe(sem->fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_of_tokens; i++) {
        if (write(sem->fd[1], "T", 1) != 1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
}

void sem_wait(sem_t *sem) {
    char token;
    if (read(sem->fd[0], &token, 1) != 1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

void sem_post(sem_t *sem) {
    if (write(sem->fd[1], "T", 1) != 1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    sem_t semaphore;

    sem_init(&semaphore, 3);
    
    // Create first child process
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid1 == 0)
    {
        for (int i = 0; i < 3; i++)
        {
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            
            sleep(1);
            
            printf("Process %d: acquired semaphore, iteration %d\n", getpid(), i + 1);
            fflush(stdout);
            
            sem_post(&semaphore);
            sem_post(&semaphore);
            sem_post(&semaphore);
        }
        exit(EXIT_SUCCESS);
    }
    
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0)
     {
        for (int i = 0; i < 3; i++) {
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            
            sleep(1);
            
            printf("Process %d: acquired semaphore, iteration %d\n", getpid(), i + 1);
            fflush(stdout);
            
            sem_post(&semaphore);
            sem_post(&semaphore);
            sem_post(&semaphore);
        }
        exit(EXIT_SUCCESS);
    }
    wait(NULL);
    wait(NULL);
    return 0;
}




















#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

// Semaphore structure using a pipe
typedef struct {
    int fd[2]; // fd[0]: for reading (sem_wait), fd[1]: for writing (sem_post)
} sem_t;

// Initialize the semaphore by creating a pipe and writing num_of_tokens tokens into it.
void sem_init(sem_t *sem, int num_of_tokens) {
    if (pipe(sem->fd) < 0) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < num_of_tokens; i++) {
        if (write(sem->fd[1], "T", 1) != 1) {
            perror("write");
            exit(EXIT_FAILURE);
        }
    }
}

void sem_wait(sem_t *sem) {
    char token;
    if (read(sem->fd[0], &token, 1) != 1) {
        perror("read");
        exit(EXIT_FAILURE);
    }
}

// sem_post writes one token back to the pipe, effectively releasing the semaphore.
void sem_post(sem_t *sem) {
    if (write(sem->fd[1], "T", 1) != 1) {
        perror("write");
        exit(EXIT_FAILURE);
    }
}

int main(void) {
    sem_t semaphore;
    // Parent initializes the semaphore with 3 tokens (semaphore size = 3)
    sem_init(&semaphore, 3);
    
    // Create first child process
    pid_t pid1 = fork();
    if (pid1 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid1 == 0) {
        // Child process 1: perform 3 iterations
        for (int i = 0; i < 3; i++) {
            // Acquire all 3 tokens (i.e. take the full semaphore)
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            
            // Simulate work: sleep for 1 second
            sleep(1);
            
            // Print the process id and the semaphore iteration number
            printf("Process %d: acquired semaphore, iteration %d\n", getpid(), i + 1);
            fflush(stdout);
            
            // Release the 3 tokens
            sem_post(&semaphore);
            sem_post(&semaphore);
            sem_post(&semaphore);
        }
        exit(EXIT_SUCCESS);
    }
    
    // Create second child process
    pid_t pid2 = fork();
    if (pid2 < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid2 == 0) {
        // Child process 2: perform 3 iterations
        for (int i = 0; i < 3; i++) {
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            sem_wait(&semaphore);
            
            sleep(1);
            
            printf("Process %d: acquired semaphore, iteration %d\n", getpid(), i + 1);
            fflush(stdout);
            
            sem_post(&semaphore);
            sem_post(&semaphore);
            sem_post(&semaphore);
        }
        exit(EXIT_SUCCESS);
    }
    
    // Parent process waits for both children to finish
    wait(NULL);
    wait(NULL);
    
    return 0;
}
