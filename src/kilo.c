#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

struct termios orig_termios;

void iskill(const char *s) {
    perror(s);
    exit(1);
}

void unrawMode() {
    tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios);
}

void rawMode() {
    tcgetattr(STDIN_FILENO, &orig_termios);
    atexit(unrawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_lflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
    
}

int main() {
    rawMode();
    while (1) {
        char c = '\0';
        read(STDIN_FILENO, &c, 1);
        if (iscntrl(c)) printf("%d\r\n", c);
        else printf("%d ('%c')\r\n", c, c);
        if (c == 'q') break;
    }

    return 0;
}