#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

// הגדרת המבנה של הסינגלטון
typedef struct {
    int value; // שדה לדוגמה
    // שדות נוספים כפי הצורך
} Singleton;

// הפונקציה שמחזירה את האובייקט הסינגלטון
Singleton* get_singleton() {
    static Singleton* instance = NULL; // משתנה סטטי לשמירת המופע
    if (instance == NULL) {
        instance = (Singleton*) malloc(sizeof(Singleton));
        if (instance == NULL) {
            perror("Memory allocation failed");
            exit(1);
        }
        // אתחול השדות במידת הצורך
        instance->value = 0;
    }
    return instance;
}

int main() {
    Singleton* s1 = get_singleton();
    Singleton* s2 = get_singleton();
    if (s1 == s2) {
        printf("Singleton works, both are the same instance\n");
    }
    return 0;
}
