#include <ncurses.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "title.h"
#include "defs.h"


static const char *SKULL_ART[] = {
    "                            __xxxxxxxxxxxxxxxx___.",
    "                        _gxXXXXXXXXXXXXXXXXXXXXXXXX!x_",
    "                   __x!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!x_",
    "                ,gXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx_",
    "              ,gXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!_",
    "            _!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!.",
    "          gXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXs",
    "        ,!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!.",
    "       g!XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "      iXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "     ,XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx",
    "     !XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx",
    "   ,XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXx",
    "   !XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXi",
    "  dXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "  !XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX",
    "   XXXXXXXXXXXXXXXXXXXf~~~VXXXXXXXXXXXXXXXXXXXXXXXXXXvvvvvvvvXXXXXXXXXXXXXX!",
    "   !XXXXXXXXXXXXXXXf`*******'XXXXXXXXXXXXXXXXXXXXXf`**********'~XXXXXXXXXXP",
    "    vXXXXXXXXXXXX!************!XXXXXXXXXXXXXXXXXX!**************!XXXXXXXXX",
    "     XXXXXXXXXXv`**************'VXXXXXXXXXXXXXXX****************!XXXXXXXX!",
    "     !XXXXXXXXX.*****************YXXXXXXXXXXXXX!****************XXXXXXXXX",
    "      XXXXXXXXX!*****************,XXXXXXXXXXXXXX***************VXXXXXXX!",
    "      'XXXXXXXX!****************,!XXXX~~XXXXXXX***************iXXXXXX~",
    "       'XXXXXXXX***************,XXXXXX   XXXXXXXX!*************xXXXXXX!",
    "        !XXXXXXXxxxxxxx*******xXXXXXXX   'YXXXXXX!**********,xXXXXXXXX",
    "          YXXXXXXXXXXXXXXXXXXXXXXXXXXX`    VXXXXXXX!s.**gxx!XXXXXXXXXP",
    "          XXXXXXXXXXXXXXXXXXXXXXXXXX!      'XXXXXXXXXXXXXXXXXXXXXXXXX!",
    "          XXXXXXXXXXXXXXXXXXXXXXXXXP        'YXXXXXXXXXXXXXXXXXXXXXXX!",
    "          XXXXXXXXXXXXXXXXXXXXXXXX!     i    !XXXXXXXXXXXXXXXXXXXXXXXX",
    "          XXXXXXXXXXXXXXXXXXXXXXXX!     XX   !XXXXXXXXXXXXXXXXXXXXXXXX",
    "          XXXXXXXXXXXXXXXXXXXXXXXXx_   iXX_,_dXXXXXXXXXXXXXXXXXXXXXXXX",
    "          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXP",
    "          XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "           ~vXvvvvXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXf",
    "                    'VXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXvvvvvv~",
    "                      'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX~",
    "                       XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXv`",
    "                       !XXXXXXX~XXXXXXXXXXXXXXXXXXXXXX~",
    "                       '~ XXXXX XXXXXXXXXXXXXXXXXXXX`",
    "                          !XXX` XXXXXXXXXXXXXXXXXXXX",
    "                          '~Vf  YXXXXXXXXXXXXXP YXXX",
    "                                !XXP YXXXfXXXX!  XXX",
    "                                     'XXP 'YXX!",
};


#define SKULL_LINES 53
#define SKULL_WIDTH 79


static const char *JAW_ART[] = {
    "_                                           ",
    "-XX!                                     Xxi",
    " YXX                                     iXX`",
    " !XX!                                    !XX",
    " !XXX                                   !XXX",
    " !XXX  ,_                               XXXV",
    " !XXX !XX                            ,.!XXX!",
    " !XXXi!XP  XX.                  ,_  !XXXXXX!",
    " iXXXx X!  XX! !Xx.  ,.     xs.,XXi !XXXXXXf",
    "  XXXXXXXXXXXXXXXXX! _!XXx  dXXXXXXX.iXXXXXX",
    "  VXXXXXXXXXXXXXXXXXXXXXXXxxXXXXXXXXXXXXXXX!",
    "  YXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXV",
    "   'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX!",
    "   'XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXf",
    "      VXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXf",
    "        VXXXXXXXXXXXXXXXXXXXXXXXXXXXXv`",
    "         ~vXXXXXXXXXXXXXXXXXXXXXXXf`",
    "             ~vXXXXXXXXXXXXXXXXv~",
    "                '~VvXXXXXXXV~~",
    "                      ~~",
};

#define JAW_LINES 20
#define JAW_WIDTH 45

void draw_skull(int rows, int cols, int toggle)
{
    // draw every Nth row and scale x
    int scale_y = 2;  // skip every other row
    int scale_x = 2;  // sample every other col

    int drawn_h = SKULL_LINES / scale_y;
    int drawn_w = SKULL_WIDTH / scale_x;

    int skull_y = (rows - drawn_h) / 2 - 15;
    int skull_x = (cols - drawn_w) / 2;

    for (int i = 0; i < SKULL_LINES; i += scale_y) {
        const char *art = SKULL_ART[i];
        int art_len = (int)strlen(art);

        int draw_row = skull_y + i / scale_y;
        if (draw_row < 0 || draw_row >= rows - 1) continue;

        for (int j = 0; j < art_len; j += scale_x) {
            int draw_col = skull_x + j / scale_x;
            if (draw_col < 0 || draw_col >= cols) continue;

            if (art[j] == " ") continue;

            if (art[j] == '*') {
                if (toggle == 0){
                    continue;
                }
                else{
                    attron(COLOR_PAIR(CP_SKULL_R));
                    mvaddch(draw_row, draw_col, '*');
                    attroff(COLOR_PAIR(CP_SKULL_R));
                }
                
            } else {
                if (toggle == 0){
                    attron(COLOR_PAIR(CP_SKULL_R) | A_BOLD);
                    mvaddch(draw_row, draw_col, art[j]);
                    attroff(COLOR_PAIR(CP_SKULL_R) | A_BOLD);
                    continue;
                    
                }
                attron(COLOR_PAIR(CP_SKULL_W) | A_BOLD);
                mvaddch(draw_row, draw_col, art[j]);
                attroff(COLOR_PAIR(CP_SKULL_W) | A_BOLD);
            }
        }
    }
}


void draw_jaw(int rows, int cols, double offset, int toggle)
{
    int scale_y = 2;
    int scale_x = 2;

    int drawn_h = JAW_LINES / scale_y;
    int drawn_w = JAW_WIDTH / scale_x;

    // position below the skull
    int jaw_y = (rows - drawn_h) / 2 + (SKULL_LINES / scale_y/2) + (int)offset -20;
    int jaw_x = (cols - drawn_w) / 2;

    for (int i = 0; i < JAW_LINES; i += scale_y) {
        const char *art = JAW_ART[i];
        int art_len = (int)strlen(art);

        int draw_row = jaw_y + i / scale_y;
        if (draw_row < 0 || draw_row >= rows - 1) continue;

        for (int j = 0; j < art_len; j += scale_x) {
            if (art[j] == ' ') continue;
            int draw_col = jaw_x + j / scale_x;
            if (draw_col < 0 || draw_col >= cols) continue;

            if (toggle == 0){
                attron(COLOR_PAIR(CP_SKULL_R) | A_BOLD);
                mvaddch(draw_row, draw_col, art[j]);
                attroff(COLOR_PAIR(CP_SKULL_R) | A_BOLD);
                continue;

            }

            attron(COLOR_PAIR(CP_SKULL_W) | A_BOLD);
            mvaddch(draw_row, draw_col, art[j]);
            attroff(COLOR_PAIR(CP_SKULL_W) | A_BOLD);
        }
    }
}


void show_death_screen(void)
{
    nodelay(stdscr, TRUE);  // non-blocking input
    curs_set(0);

    int ch = 0;
    static int jaw_tick = 0;
    struct timespec ts = { 0, 50000000L };  // 50ms = ~20fps
    int toggle = 0;

    while (ch != '\n' && ch != '\r' && ch != KEY_ENTER) {

        if (ch == 'Q' || ch == 'q'){
            // printf("YO THANKS FOR PLAYING\n");
            return;
        }

        if (jaw_tick > 1000){
            jaw_tick = 0;
        }
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        erase();

        attron(COLOR_PAIR(CP_FLOOR));
        for (int y = 0; y < rows; y++)
            mvhline(y, 0, ' ', cols);
        attroff(COLOR_PAIR(CP_FLOOR));

        
        if (jaw_tick % 30 == 0){
            toggle = !toggle;
        }

        draw_skull(rows, cols, toggle);

        jaw_tick++;
        double jaw_offset = 7 + 2.5 + 2.5 * sin(jaw_tick * 0.77);
        draw_jaw(rows, cols, jaw_offset, toggle);

        // find bottom of jaw
        int scale_y = 2;
        int jaw_bottom = (rows - JAW_LINES / scale_y) / 2 + (SKULL_LINES / scale_y / 2)  - 10 + JAW_LINES / scale_y + 2;

        // YOU DIED
        if (jaw_tick % 60 == 0){
            
        }

        attron(COLOR_PAIR(TITLE3) | A_BOLD);
        mvprintw(jaw_bottom, (cols - 4) / 2, "YOU DIED");
        attroff(COLOR_PAIR(TITLE3) | A_BOLD);

        // PRESS R TO RESPAWN
        attron(COLOR_PAIR(CP_UI_TEXT));
        mvprintw(jaw_bottom + 2, (cols - 14) / 2, "PRESS R TO RESPAWN");
        attroff(COLOR_PAIR(CP_UI_TEXT));

        refresh();

        nanosleep(&ts, NULL);  // consistent frame time

        ch = getch();
        if (ch == ERR) ch = 0;
    }

    cbreak();
    nodelay(stdscr, TRUE);
    erase();
    refresh();
}