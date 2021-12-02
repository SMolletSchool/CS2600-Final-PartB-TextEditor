//includes

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <termios.h>

//defines
#define CTRL_KEY(k) ((k) & 0x1f)

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

int getCursorPos(int *rows, int *cols) {
    char buf[32];
    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1) break;
        if (buf[i] == 'R') break;
        i++;
    }
    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[') return -1;
    if (sscanf(&buf[2], "%d:%d", rows, cols) != 2) return -1;

    return 0;
}

int getWindowSize(int *rows, int *cols) {
    struct winsize ws;

    if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12) return -1;
        return getCursorPos(rows,cols);
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
        case CTRL_KEY('q'):
            write(STDOUT_FILENO, "\x1b[2]", 4);
            write(STDOUT_FILENO, "\x1b[H", 3);
            exit(0);
            break;
    }
}

void editorRows() {
    int y;
    for (y = 0; y < E.screenrows; y++) {
        write(STDOUT_FILENO, "`", 1);

        if (y < E.screenrows - 1) {
            write(STDOUT_FILENO, "\r\n", 2);
        }
    }
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