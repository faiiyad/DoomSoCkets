#include "input.h"
#include "console.h"
#include <ncurses.h>
#include <stdlib.h>

Input input = {0, 0, 0, 0, 0, 0};

void handleInput(void) {
    // Reset all flags at the start of each frame
    input.moveForward  = 0;
    input.moveBackward = 0;
    input.strafeLeft   = 0;
    input.strafeRight  = 0;
    input.turnLeft     = 0;
    input.turnRight    = 0;

    // Drain all pending keypresses from the buffer
    // getch() returns ERR immediately when empty because we set nodelay(TRUE)
    int ch;
    while ((ch = getch()) != ERR) {
        switch (ch) {
            case 'w': case 'W': input.moveForward  = 1; break;
            case 's': case 'S': input.moveBackward = 1; break;
            case 'a': case 'A': input.strafeLeft   = 1; break;
            case 'd': case 'D': input.strafeRight  = 1; break;
            case KEY_LEFT:      input.turnLeft     = 1; break;
            case KEY_RIGHT:     input.turnRight    = 1; break;
            case 'q': case 'Q':
                // Always clean up ncurses before exit or the terminal breaks
                shutdownConsole();
                exit(0);
        }
    }
}