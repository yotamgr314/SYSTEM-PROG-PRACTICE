#include <stdio.h>              // Standard I/O functions
#include <stdlib.h>             // Standard library functions (malloc, exit, free)
#include <pthread.h>            // POSIX threads library
#include <signal.h>             // Signal handling functions and definitions
#include <unistd.h>             // UNIX standard functions (fork, pause)
#include <errno.h>              // Error number definitions

// Define a semaphore structure
typedef struct {
    int value, wakeups;         // 'value' holds the current semaphore count, 'wakeups' counts pending wakeups
    pthread_mutex_t mutex;      // Mutex to protect access to the semaphore
    pthread_cond_t cond;        // Condition variable for waiting threads
} sem_t;

// Initialize the semaphore with an initial value
void sem_init(sem_t* semaphore, int value) {
    semaphore->value = value;                             // Set the semaphore's initial value
    semaphore->wakeups = 0;                               // Initialize wakeups count to 0
    semaphore->mutex = PTHREAD_MUTEX_INITIALIZER;         // Initialize the mutex
    semaphore->cond = PTHREAD_COND_INITIALIZER;           // Initialize the condition variable
}

// Wait (decrement) on the semaphore
void sem_wait(sem_t* semaphore) {
    pthread_mutex_lock(&semaphore->mutex);                // Lock the semaphore's mutex
    semaphore->value--;                                   // Decrement the semaphore's value
    if (semaphore->value < 0) {                           // If the value is negative, we must wait
        do {
            pthread_cond_wait(&semaphore->cond, &semaphore->mutex); // Wait on the condition variable
        } while (semaphore->wakeups < 1);                 // Loop until a wakeup signal is received
        semaphore->wakeups--;                           // Decrement the wakeups count after waking up
    }
    pthread_mutex_unlock(&semaphore->mutex);              // Unlock the semaphore's mutex
}

// Signal (increment) the semaphore
void sem_post(sem_t* semaphore) {
    pthread_mutex_lock(&semaphore->mutex);                // Lock the semaphore's mutex
    semaphore->value++;                                   // Increment the semaphore's value
    if (semaphore->value <= 0) {                          // If there are waiting threads (value non-positive)
        semaphore->wakeups++;                           // Increment the wakeups count
        pthread_cond_signal(&semaphore->cond);          // Signal one waiting thread
    }
    pthread_mutex_unlock(&semaphore->mutex);              // Unlock the semaphore's mutex
}

// Global mutex for synchronizing access to the queue
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;      // Initialize a global mutex

// Global condition variable for notifying threads waiting for queue items
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;           // Initialize a global condition variable

// Global semaphore to limit the number of concurrent print operations to 3
sem_t sem;                                              // Declare a semaphore

// Dummy declarations for the queue functions (assumed to be implemented elsewhere)
// Returns non-zero if the queue is empty, zero otherwise
int queue_empty();  

// Pushes a string onto the queue
void queue_push(char* str);  

// Pops a string from the queue; returns a pointer to the string
char* queue_pop();          

// Dummy declaration for the print function
// Sends the string to the printer; returns 0 on success or a non-zero value on failure (with errno set)
int print(char* string);    

// Function to push a string into the queue and signal other threads
void push(char* s) {
    pthread_mutex_lock(&mutex);     // Lock the mutex to protect the queue
    pthread_cleanup_push((void(*)(void*))pthread_mutex_unlock, (void*)&mutex); // Register cleanup handler to unlock mutex if needed
    queue_push(s);                  // Push the string into the queue
    pthread_cleanup_pop(1);         // Pop the cleanup handler and execute it (1 indicates execution)
    pthread_cond_signal(&cond);     // Signal the condition variable to notify waiting threads that an item is available
}

// Function to continuously pop messages from the queue and send them to the printer
void pop() {
    while (1) {                                       // Infinite loop to continuously process queue items
        sem_wait(&sem);                               // Wait on the semaphore (limits to 3 concurrent prints)
        pthread_mutex_lock(&mutex);                   // Lock the mutex to access the queue safely
        while (queue_empty())                         // While the queue is empty
            pthread_cond_wait(&cond, &mutex);         // Wait on the condition variable until an item is pushed
        char* s = queue_pop();                        // Pop a string from the queue
        pthread_mutex_unlock(&mutex);                 // Unlock the mutex after accessing the queue
        if (print(s) != 0) {                          // Attempt to print the string; if print fails
            perror("Print failed");                   // Print an error message to standard error
        }
        free(s);                                      // Free the memory allocated for the string
        sem_post(&sem);                               // Signal the semaphore to indicate print operation is done
    }
}

// Global array to store child process IDs (for the 3 child processes)
pid_t id[3] = {0};                                    // Initialize the array with zeros

// Signal handler for SIGHUP signal to cancel processes reading from the queue
void sig_handler(int num) {
    for (int i = 0; i < 3; i++) {                     // Loop over each child process ID
        kill(id[i], SIGTERM);                         // Send a SIGTERM signal to the child process
    }
}

// Main function: creates processes, pushes messages, and sets up signal handling
int main() {
    signal(SIGHUP, sig_handler);                      // Register the SIGHUP signal handler
    sem_init(&sem, 3);                                // Initialize the semaphore with a value of 3 (max 3 concurrent prints)

    // Push 10 different messages into the queue
    for (int i = 0; i < 10; i++) {
        char* msg = malloc(64);                       // Allocate memory for the message string
        if (msg == NULL) {                            // Check if allocation failed
            perror("malloc failed");                  // Print error message if malloc fails
            exit(EXIT_FAILURE);                       // Exit program with failure status
        }
        snprintf(msg, 64, "hey%d", i);                 // Format the message string with the current index
        push(msg);                                    // Push the formatted message into the queue
    }

    // Create 3 child processes to handle printing (pop from queue)
    for (int i = 0; i < 3; i++) {
        id[i] = fork();                               // Fork a new process and store its PID in the id array
        if (id[i] < 0) {                              // Check for fork error
            perror("fork failed");                    // Print error message if fork fails
            exit(EXIT_FAILURE);                       // Exit program with failure status
        }
        if (id[i] == 0) {                             // Child process block
            pop();                                    // Child process calls pop() (which runs indefinitely)
            exit(0);                                  // Exit child process (unreachable due to infinite loop)
        }
    }

    pause();                                          // Parent process waits indefinitely for a signal
    return 0;                                         // Return 0 (this line is never reached)
}





// SEM.C BUT WITH API - AS ERAN ALLOWS US










/* #include <stdio.h>              // Standard input/output functions
#include <stdlib.h>             // Standard library functions (malloc, exit, free)
#include <pthread.h>            // POSIX threads library for mutexes and condition variables
#include <signal.h>             // Signal handling functions and definitions
#include <unistd.h>             // UNIX standard functions (fork, pause)
#include <errno.h>              // Error number definitions
#include <semaphore.h>          // POSIX semaphore functions

// Dummy declarations for the queue functions (assumed to be implemented elsewhere)
int queue_empty();              // Returns non-zero if the queue is empty, zero otherwise
void queue_push(char* str);     // Pushes a string onto the queue
char* queue_pop();              // Pops a string from the queue and returns it

// Dummy declaration for the print function
// Sends the string to the printer synchronously; returns 0 on success or non-zero on failure (errno is set)
int print(char* string);

// Global mutex for synchronizing access to the shared queue
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;  // Initialize the mutex

// Global condition variable to signal threads that an item is available in the queue
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;       // Initialize the condition variable

// Global semaphore to limit the number of concurrent print operations to 3
sem_t sem;                                          // Declare a semaphore

// Function to push a string into the queue and signal waiting threads
void push(char* s) {
    pthread_mutex_lock(&mutex);                     // Lock the mutex to protect the queue
    queue_push(s);                                  // Push the string into the queue
    pthread_mutex_unlock(&mutex);                   // Unlock the mutex after pushing
    pthread_cond_signal(&cond);                     // Signal the condition variable to notify waiting threads
}

// Function that continuously pops messages from the queue and sends them to the printer
void pop() {
    while (1) {                                     // Infinite loop to process messages continuously
        sem_wait(&sem);                             // Wait on the semaphore (decrement), ensuring max 3 concurrent prints
        pthread_mutex_lock(&mutex);                 // Lock the mutex to safely access the queue
        while (queue_empty()) {                     // While the queue is empty, wait for a signal
            pthread_cond_wait(&cond, &mutex);       // Wait on the condition variable; releases and re-acquires the mutex
        }
        char* s = queue_pop();                      // Pop a string from the queue
        pthread_mutex_unlock(&mutex);               // Unlock the mutex after accessing the queue
        if (print(s) != 0) {                        // Attempt to print the string; if printing fails...
            perror("Print failed");                 // ...print an error message to standard error
        }
        free(s);                                    // Free the memory allocated for the string
        sem_post(&sem);                             // Post (increment) the semaphore to indicate print is finished
    }
}

// Global array to store the child process IDs for the three child processes
pid_t id[3] = {0};                                  // Initialize the array with zeros

// Signal handler for SIGHUP to cancel processes reading from the queue
void sig_handler(int num) {
    for (int i = 0; i < 3; i++) {                   // Loop over each child process
        kill(id[i], SIGTERM);                       // Send a SIGTERM signal to the child process
    }
}

int main() {
    signal(SIGHUP, sig_handler);                    // Register the signal handler for SIGHUP

    // Initialize the semaphore with an initial value of 3, allowing up to 3 concurrent prints.
    // The second parameter is 1 to indicate the semaphore is shared between processes.
    if (sem_init(&sem, 1, 3) == -1) {               
        perror("sem_init failed");                  // Print an error if semaphore initialization fails
        exit(EXIT_FAILURE);                         // Exit the program with a failure status
    }

    // Push 10 different messages into the queue
    for (int i = 0; i < 10; i++) {
        char* msg = malloc(64);                     // Allocate memory for the message string
        if (msg == NULL) {                          // Check if memory allocation failed
            perror("malloc failed");                // Print error if allocation fails
            exit(EXIT_FAILURE);                     // Exit the program with a failure status
        }
        snprintf(msg, 64, "hey%d", i);               // Format the message string with the current index
        push(msg);                                  // Push the formatted message into the queue
    }

    // Create 3 child processes to handle printing by calling pop()
    for (int i = 0; i < 3; i++) {
        id[i] = fork();                             // Fork a new process and store its PID in the id array
        if (id[i] < 0) {                            // Check for fork error
            perror("fork failed");                  // Print error message if fork fails
            exit(EXIT_FAILURE);                     // Exit the program with a failure status
        }
        if (id[i] == 0) {                           // Child process block
            pop();                                  // Call pop() which runs indefinitely processing the queue
            exit(0);                                // Exit the child process (unreachable due to infinite loop)
        }
    }

    pause();                                        // Parent process waits indefinitely for a signal (e.g., SIGHUP)
    return 0;                                       // Return 0 (this line is never reached)
}
 */