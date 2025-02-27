#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(void) {
    printf("my_son.exe is running...\n");
    sleep(5);  // Simulate work for 10 seconds before exiting
    printf("my_son.exe is exiting...\n");
    return 0;
}
