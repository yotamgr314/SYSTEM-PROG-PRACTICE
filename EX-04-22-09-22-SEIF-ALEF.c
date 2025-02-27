#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>

// --- External library functions ---
// These functions are provided by a library and are not thread-safe:
// queue_is_empty() returns a boolean (true/false) indicating whether the queue is empty.
// queue_push(char*) pushes a string into the queue.
// queue_pop() returns a string from the queue.
// print(char*) sends a string to the printer, returns 0 on success, non-zero on failure (and sets errno).
extern int queue_is_empty(void);
extern void queue_push(char *str);
extern char* queue_pop(void);
extern int print(char *string);

// Global synchronization variables
pthread_mutex_t queue_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t  queue_cond  = PTHREAD_COND_INITIALIZER;

// Semaphore to limit concurrent printer calls to 3.
sem_t printer_sem;

// Function to enqueue a message into the queue and signal waiting threads.
void enqueue_message(char *message) {
    pthread_mutex_lock(&queue_mutex);
    queue_push(message);
    // Signal all threads that an item is now available.
    pthread_cond_broadcast(&queue_cond);
    pthread_mutex_unlock(&queue_mutex);
}

// Consumer function: waits until a message is available in the queue, then processes it.
// The semaphore limits the number of concurrent print() calls to 3.
void *consumer_function(void *arg) {
    while (1) {
        pthread_mutex_lock(&queue_mutex);
        // Wait until the queue is not empty.
        while (queue_is_empty()) {
            pthread_cond_wait(&queue_cond, &queue_mutex);
        }
        // Retrieve a message from the queue.
        char *msg = queue_pop();
        pthread_mutex_unlock(&queue_mutex);
        if (msg != NULL) {
            // Wait on the semaphore before calling print().
            sem_wait(&printer_sem);
            int ret = print(msg);
            if (ret != 0) {
                // On failure, print the error message using errno.
                fprintf(stderr, "Print error: %s\n", strerror(errno));
            }
            sem_post(&printer_sem);
            // Free the message if it was dynamically allocated.
            free(msg);
        }
    }
    return NULL;
}
