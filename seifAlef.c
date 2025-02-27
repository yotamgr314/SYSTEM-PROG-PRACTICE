// --- Code for Question 1: Thread-Safe Enqueue and Consumer Functions ---
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

// External queue functions (not thread-safe)
extern int queue_is_empty(void);
extern void queue_push(char *str);
extern char* queue_pop(void);

// External printer function:
// Sends a string to the printer and waits for a response.
// Returns 0 on success, non-zero on failure and sets errno.
extern int print(char *string);

// Global synchronization variables
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  queue_cond  = PTHREAD_COND_INITIALIZER;

// Semaphore to limit concurrent printer calls to 3.
sem_t printer_sem;


void init_sync_primitives(void) {
    // Initialize the printer semaphore for threads (pshared = 0) with initial count 3.
    if (sem_init(&printer_sem, 0, 3) != 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }
}


// CONFIRMED BY ERANNNNNNNNNNNN
// Function to enqueue a message into the shared queue and signal waiting threads.
void enqueue_message(char *message) {
    pthread_mutex_lock(&queue_mutex);
    queue_push(message);
    // Signal all waiting threads that a new message is available.

    pthread_mutex_unlock(&queue_mutex); // ERAN SAID TO PUT IT OUTSIDE THE LOCK ! NOT SUCH A BIG MISTAKE BUT NO REASON IT SHALL BE LOCKED.
    pthread_cond_broadcast(&queue_cond); 

}