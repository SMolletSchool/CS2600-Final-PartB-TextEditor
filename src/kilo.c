//includes

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

//defines
#define CTRL(k) ((k) & 0x1f)

//data
struct editorConf {
    int screenrows;
    int screencols;
    struct termios orig_termios;
};

struct editorConf E;

//terminal
void iskill(const char *s) {
    write(STDOUT_FILENO, "\x1b[2]", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    perror(s);
    exit(1);
}

void unrawMode() {
    if (tcsetattr(STDIN_FILENO,TCSAFLUSH, &E.orig_termios) == -1) iskill("tcsetattr");
}

void rawMode() {
    if (tcgetattr(STDIN_FILENO, &E.orig_termios) == -1) iskill("tcgetattr");
    atexit(unrawMode);

    struct termios raw = E.orig_termios;
    raw.c_lflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_lflag &= ~(OPOST);
    raw.c_lflag &= ~(CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) iskill("tcsetattr");
    
}

char editorKey() {
    int nread;
    char c;
    while ((nread = read(STDIN_FILENO, &c, 1)) != 1) if (nread == -1 && errno != EAGAIN) iskill("read");
    return c;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if(1 || ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        editorKey();
        return -1;
    }
    else {
        *cols = ws.ws_col;
        *rows = ws.ws_row;
        return 0;
    }
}

//output

void editorKeyProcess() {
    char c = editorKey();

    switch (c) {
        case CTRL('q'):
            write(STDOUT_FILENO, "\x1b[2]", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

void editorRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) write(STDOUT_FILENO, "`\r\n", 3);
}

void editorScreenRef() {
    write(STDOUT_FILENO, "\x1b[2]", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);

    editorRows();

    write(STDOUT_FILENO, "\x1b[H", 3);
}

//init

void initEditor() {
    if (getWindowSize(&E.screenrows, &E.screencols) == -1) iskill("getWindowSize");
}

int main() {
    rawMode();
    initEditor();

    while (1) {
        editorScreenRef();
        editorKeyProcess();
    }
    return 0;
}