#include "console.h"
#include "input.h"
#include "player.h"
#include "render.h"
#include <ncurses.h>
#include <time.h>

int main(void) {
    initConsole();
    player.angle = 0.0;

    struct timespec lastTime, currentTime;
    clock_gettime(CLOCK_MONOTONIC, &lastTime);

    while (1) {
        // 1. Read keyboard input
        handleInput();

        // 2. Move and rotate the player
        updatePlayer();

        // 3. Clear ncurses' internal buffer
        //    erase() marks everything for redraw - doesn't touch the terminal yet
        erase();

        // 4. Draw the 3D view into the buffer
        renderView();

        // 5. Draw the HUD into the buffer
        drawHUD();

        // 6. Flush the buffer to the terminal
        //    refresh() only redraws characters that actually changed - efficient
        refresh();

        // 7. Cap at ~60 FPS using nanosleep (more precise than Sleep on Linux)
        clock_gettime(CLOCK_MONOTONIC, &currentTime);
        long elapsedNs = (currentTime.tv_sec  - lastTime.tv_sec)  * 1000000000L
                       + (currentTime.tv_nsec - lastTime.tv_nsec);
        long targetNs = 16666666L;  // 1/60 second in nanoseconds

        if (elapsedNs < targetNs) {
            struct timespec sleepTime = {0, targetNs - elapsedNs};
            nanosleep(&sleepTime, NULL);
        }

        clock_gettime(CLOCK_MONOTONIC, &lastTime);
    }

    shutdownConsole();
    return 0;
}