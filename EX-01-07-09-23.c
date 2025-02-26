
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

void cleanup_mem(void* arg)
{
    free()
}

pthread_cond_t qcond = PTHREAD_COND_INITIALIZER;

create_word(char*s)
{
    pthread_cleanup_push(pthread_mutex_unlock, &qlock);
    if(p == NULL)
    p=(char*)malloc(1000);

    pthread_mutex_lock(&qlock);
    strcpy(p,s);
    pthread_mutex_unlock(&qlock);


    pthread_cleanup_pop(0);

}