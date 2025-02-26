#include <stdio.h>

int main() {
    int ch;
    while ((ch = getc(stdin)) != EOF) {
        if (putc(ch, stdout) == EOF) {
            return 1;
        }
    }
    return 0;
}
