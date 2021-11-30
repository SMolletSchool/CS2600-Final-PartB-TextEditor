#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void unrawMode() {
    tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios);
}

void rawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(unrawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(ECHO);
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
}

int main() {
    rawMode();
    char c;
    while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q');
    return 0;
}