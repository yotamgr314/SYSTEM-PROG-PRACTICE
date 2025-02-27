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
// CONFIRMED BY ERANNNNNNNNNNNN





/* // Consumer function: waits until a message is available in the queue,
// pops the message, and sends it to the printer.
// It limits concurrent printer calls using a semaphore.
void *consumer_function(void *arg) {
    while (!shutdown_flag) {
        pthread_mutex_lock(&queue_mutex);
        // Wait until the queue is not empty or a shutdown signal is set.
        while (queue_is_empty() && !shutdown_flag)
        {
            pthread_cond_wait(&queue_cond, &queue_mutex); // relase the lock. and wait on a condition.
        }
        // IF WE ARE HERE IT MEANS MESSAGE QUEUE IS NOT EMPTY + I GOT A WAKE UP SIGNAL - ENSURING THAT AFTER I WAKE UP ANOTHER THREAD HASENT EMPTIED THE MESSAGE QUEUE.

        // Exit if shutdown is signaled. --------- ERAN HAVENT TALLKED ABOUT IT
        if (shutdown_flag) {
            pthread_mutex_unlock(&queue_mutex);
            break;
        }
        //  ERAN HAVENT TALLKED ABOUT IT



        // Retrieve a message from the queue.
        char *msg = queue_pop(); // OK BY ERAN 
        pthread_mutex_unlock(&queue_mutex); // OK BY ERAN 

        if (msg != NULL) 
        {
            // Wait on the semaphore to ensure at most 3 concurrent print calls.
            sem_wait(&printer_sem); // OK BY ERAN 
            int ret = print(msg);
            if (ret != 0) {
                // Print the error message with the error string from errno.
                fprintf(stderr, "Print error: %s\n", strerror(errno));
            }
            sem_post(&printer_sem); // OK BY ERAN 
            // Free the dynamically allocated message if needed.
            free(msg);
        }
    }
    return NULL;
}
 */
















// --- Updated Code for Question 2: One Producer and Three Consumers ---

#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>
#include <errno.h>

// Declaration of external functions assumed to be implemented in Question 1.
void enqueue_message(char *message);
void *consumer_function(void *arg);

// Global semaphore for limiting printer calls.
// For processes, pshared should be 1.
sem_t printer_sem;

// Helper function to create a word string.
// Returns a dynamically allocated string.
char* create_word(int index) 
{
    char buffer[50];
    snprintf(buffer, sizeof(buffer), "word%d", index);
    return strdup(buffer);
}

int main(void) {
    pid_t producer_pid;
    pid_t consumer_pids[3];
    int i;

    // Initialize the printer semaphore for inter-process sharing (pshared = 1) with an initial count of 3.
    if (sem_init(&printer_sem, 1, 3) < 0) {
        perror("sem_init");
        exit(EXIT_FAILURE);
    }

    // Create the Producer process (child 1).
    producer_pid = fork();
    if (producer_pid < 0) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (producer_pid == 0) {
        // Producer Process: Enqueue 10 messages into the queue.
        for (int j = 1; j <= 10; j++) 
        {
            char *word = create_word(j);
            enqueue_message(word);
        }
        exit(0);
    }

    // Create three Consumer processes (children 2, 3, and 4).
    for (i = 0; i < 3; i++) 
    {
        consumer_pids[i] = fork();

        if (consumer_pids[i] < 0)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (consumer_pids[i] == 0) 
        {
            // Consumer Process: Repeatedly pop messages from the queue and send them to the printer.
            consumer_function(NULL);
            exit(0);
        }
    }

    // Parent process: wait for the Producer process.
    waitpid(producer_pid, NULL, 0);
    // Parent process: wait for all Consumer processes.
    for (i = 0; i < 3; i++)
     {
        waitpid(consumer_pids[i], NULL, 0);
    }

    return 0;
}
