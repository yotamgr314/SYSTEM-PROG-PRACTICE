
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <pthread.h>





void cleanup_mutex(void* arg)
{
    pthread_mutex_unlock((pthread_mutex_t*)arg);
}

void cleanup_word(void *arg) {
    if (p != NULL) {
        free(p);
        p = NULL;
    }
}


pthread_cond_t qcond = PTHREAD_COND_INITIALIZER;
pthread_mutex_t qlock = PTHREAD_MUTEX_INITIALIZER;
create_word(char*s)
{
    pthread_mutex_lock(&qlock)
    pthread_cleanup_push(pthread_mutex_unlock, &qlock);
    if(p == NULL)
    p=(char*)malloc(1000);
    pthread_clean_up_push(cleanup_mem, p);
    
    strcpy(p,s);
    pthread_mutex_unlock(&qlock);


    pthread_cleanup_pop(0);
    pthread_cleanup_pop(1);


}