#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

// Initialize two mutexes
pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

// Thread function for thread 1
void* thread1_func(void *arg) {
    while (1) {
        // Lock the first mutex
        pthread_mutex_lock(&lock1);
        printf("Thread 1: Acquired lock1\n");

        // Attempt to lock the second mutex using trylock
        if (pthread_mutex_trylock(&lock2) == 0) {
            printf("Thread 1: Acquired lock2\n");
            // Critical section: both locks are acquired successfully
            // Perform the shared operation here

            // Release the locks
            pthread_mutex_unlock(&lock2);
            pthread_mutex_unlock(&lock1);
            break;  // Exit the loop as the operation is complete
        } else {
            // If unable to acquire lock2, release lock1 and wait before retrying
            printf("Thread 1: Could not acquire lock2, releasing lock1\n");
            pthread_mutex_unlock(&lock1);
            usleep(1000);  // Sleep for 1 microsecond (adjust as needed)
        }
    }
    return NULL;
}

// Thread function for thread 2
void* thread2_func(void *arg) {
    while (1) {
        // Lock the second mutex first
        pthread_mutex_lock(&lock2);
        printf("Thread 2: Acquired lock2\n");

        // Attempt to lock the first mutex using trylock
        if (pthread_mutex_trylock(&lock1) == 0) {
            printf("Thread 2: Acquired lock1\n");
            // Critical section: both locks are acquired successfully
            // Perform the shared operation here

            // Release the locks
            pthread_mutex_unlock(&lock1);
            pthread_mutex_unlock(&lock2);
            break;  // Exit the loop as the operation is complete
        } else {
            // If unable to acquire lock1, release lock2 and wait before retrying
            printf("Thread 2: Could not acquire lock1, releasing lock2\n");
            pthread_mutex_unlock(&lock2);
            usleep(1000);  // Sleep briefly before retrying
        }
    }
    return NULL;
}

int main() {
    pthread_t t1, t2;

    // Create two threads
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    // Wait for both threads to finish execution
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
