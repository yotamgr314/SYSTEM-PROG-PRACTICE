#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// הגדרת המבנה של הסינגלטון
typedef struct {
    int value; // שדה לדוגמה
    // שדות נוספים כפי הצורך
} Singleton;

pthread_mutex_t singleton_mutex = PTHREAD_MUTEX_INITIALIZER;

// הפונקציה שמחזירה את האובייקט הסינגלטון
Singleton* get_singleton() {
    pthread_mutex_lock(&singleton_mutex);
    static Singleton* instance = NULL; // משתנה סטטי לשמירת המופע
    if (instance == NULL) 
    {
        instance = (Singleton*) malloc(sizeof(Singleton));
        if (instance == NULL) {
            perror("Memory allocation failed");
            exit(1);
        }
        // אתחול השדות במידת הצורך
        instance->value = 0;
    }
    return instance;
    pthread_mutex_unlock(&singleton_mutex);
}

int main() {
    Singleton* s1 = get_singleton();
    Singleton* s2 = get_singleton();
    if (s1 == s2) {
        printf("Singleton works, both are the same instance\n");
    }
    return 0;
}
-