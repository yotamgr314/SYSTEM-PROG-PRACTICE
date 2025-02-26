#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

pthread_mutex_t lock1 = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t lock2 = PTHREAD_MUTEX_INITIALIZER;

void* thread1_func(void *arg) {
    // חוט 1 נועל את lock1
    pthread_mutex_lock(&lock1);
    printf("Thread 1: Acquired lock1\n");
    sleep(1);  // דיליי על מנת לתת לחוט 2 זמן לנעול את lock2

    // מנסה לנעול את lock2 אך lock2 כבר מוחזק על ידי חוט 2
    printf("Thread 1: Waiting for lock2\n");
    pthread_mutex_lock(&lock2);
    printf("Thread 1: Acquired lock2\n");

    // שחרור המשאבים (לא יגיעו לכאן בדדלוק)
    pthread_mutex_unlock(&lock2);
    pthread_mutex_unlock(&lock1);
    return NULL;
}

void* thread2_func(void *arg) {
    // חוט 2 נועל את lock2
    pthread_mutex_lock(&lock2);
    printf("Thread 2: Acquired lock2\n");
    sleep(1);  // דיליי כדי לתת לחוט 1 זמן לנעול את lock1

    // מנסה לנעול את lock1 אך lock1 מוחזק על ידי חוט 1
    printf("Thread 2: Waiting for lock1\n");
    pthread_mutex_lock(&lock1);
    printf("Thread 2: Acquired lock1\n");

    // שחרור המשאבים (לא יגיעו לכאן בדדלוק)
    pthread_mutex_unlock(&lock1);
    pthread_mutex_unlock(&lock2);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, thread1_func, NULL);
    pthread_create(&t2, NULL, thread2_func, NULL);

    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    return 0;
}
