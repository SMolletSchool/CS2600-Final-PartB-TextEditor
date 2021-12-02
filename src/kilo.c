//includes

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <termios.h>

//defines
#define CTRL(k) ((k) & 0x1f)

//globals
struct termios orig_termios;


//terminal
void iskill(const char *s) {
    perror(s);
    exit(1);
}

void unrawMode() {
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH, &orig_termios) == -1) iskill("tcsetattr");
}

void rawMode() {
    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) iskill("tcgetattr");
    atexit(unrawMode);

    struct termios raw = orig_termios;
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_lflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) iskill("tcsetattr");
    
}

//init
int main() {
    rawMode();
    while (1) {
        char c = '\0';
        if(read(STDIN_FILENO, &c, 1) == -1 && errno != EAGAIN) iskill("read");
        if (iscntrl(c)) printf("%d\r\n", c);
        else printf("%d ('%c')\r\n", c, c);
        if (c == CTRL('q')) break;
    }

    return 0;
}