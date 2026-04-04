#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <wchar.h>
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "title.h"
#include "defs.h"
#include "player.h"
#include "render.h"
#include "entity.h"
#include "ui.h"

// ── GAME OVER title art ────────────────────────────────────────────────────
static const char *GAMEOVER_ART[] = {
    "@@@@@@@@@  @@   @@@@  @@@@@@@@    @@@@@@  @@@@@@@   @@@     @@@ @@@@@@@@@",
    " @@@@@@@@  @@@@  @@    @@@@@@@   @@@@@@@   @@@@@@@   @@@@ @@@@   @@@@@@@@",
    "   @@       @@@@  @@    @@   @@   @@        @@@ @@@   @@@@ @@@@   @@     ",
    "   @@@@@    @@@@@ @@    @@   @@   @@  @@@   @@@ @@@   @@@@@@@@@   @@@@@  ",
    "   @@@@     @@@@@@@@    @@   @@   @@  @@@   @@@ @@@   @@ @@@ @@   @@@@   ",
    "   @@       @@@@@@@@    @@  @@@   @@   @@   @@@@@@@   @@ @@@ @@   @@     ",
    "   @@       @@@@@@@@    @@ @@@@   @@@ @@@   @@@@@@@   @@@ @ @@@   @@     ",
    " @@@@@@@@   @@@  @@@    @@@@@      @@@@@@   @@@  @@@  @@@   @@@  @@@@@@@@",
    "@@@@@@@@   @@@  @@@   @@@@@@      @@@@@    @@@  @@@  @@@   @@@  @@@@@@@@ ",
    "          @@     @   @@@@@        @@      @@    @@  @@     @@            "        
};

#define GAMEOVER_LINES 10
#define GAMEOVER_WIDTH 73

static const char *SKELETON_ART[] = {
    "                      :::!~!!!!!:.                ",
    "                  .xUHWH!! !!?M88WHX:.            ",
    "                .X*#M@$!!  !X!M$$$$$$WWx:.        ",
    "               :!!!!!!?H! :!$!$$$$$$$$$$8X:       ",
    "              !!~  ~:~!! :~!$!#$$$$$$$$$$8X:      ",
    "             :!~::!H!<   ~.U$X!?R$$$$$$$$MM!      ",
    "             ~!~!!!!~~ .:XW$$$U!!?$$$$$$RMM!      ",
    "               !:~~~ .:!M\"T#$$$$WX??#MRRMMM!      ",
    "               ~?WuxiW*`   `\"#$$$$8!!!!??!!!      ",
    "             :X- M$$$$       `\"T#$T~!8$WUXU~      ",
    "            :%`  ~#$$$m:        ~!~ ?$$$$$$       ",
    "          :!`.-   ~T$$$$8xx.  .xWW- ~\"\"##*\"       ",
    ".....   -~~:<` !    ~?T#$$@@W@*?$$      /`        ",
    "W$@@M!!! .!~~ !!     .:XUW$W!~ `\"~:    :          ",
    "#\"~~`.:x%`!!  !H:   !WM$$$$Ti.: .!WUn+!`          ",
    ":::~:!!`:X~ .: ?H.!u \"$$$B$$$!W:U!T$$M~           ",
    ".~~   :X@!.-~   ?@WTWo(\"*$$$W$TH$! `              ",
    "Wi.~!X$?!-~    : ?$$$B$Wu(\"**$RM!                 ",
    "$R@i.~~ !     :   ~$$$$$B$$en:``                  ",
    "?MXT@Wx.~    :     ~\"##*$$$$M~                    ",
};

static const char *SKELETON_MIRROR_ART[] = {
    "               .:!!!!!~!:::                      ",
    "           .:XHW88M?!! !!HWHUx.                  ",
    "       .:xWW$$$$$$M!X!  !!$@M#*X.                ",
    "      :X8$$$$$$$$$$!$!: !H?!!!!!!:               ",
    "     :X8$$$$$$$$$$#!$!~: !!~:~  ~!!              ",
    "     !MM$$$$$$$$R?!X$U.~   <!H!::~!:             ",
    "     !MMR$$$$$$?!!U$$$WX:. ~~!!!!~!~             ",
    "     !MMMRRM#??XW$$$$#T\"M!:. ~~~:!               ",
    "     !!!??!!!!8$$$$#\"`   `*WixuW?~               ",
    "     ~UXUW$8!~T$#T\"`       $$$$M -X:             ",
    "       $$$$$$? ~!~        :m$$$#~  `%:           ",
    "       \"*##\"\"~ -WWx.  .xx8$$$$T~   -.`!:         ",
    "       `/      $$?*@W@@$$#T?~    ! `<:~~-   .....",
    "         :    :~\"` ~!W$WUX:.     !! ~~!. !!!M@@$W",
    "         `!+nUW!. :.iT$$$$MW!   :H!  !!`%x:.`~~\"#",
    "          ~M$$T!U:W!$$$B$$$\" u!.H? :. ~X:`!!:~:::",
    "             ` !$HT$W$$$*\"(oWTW@?   ~-.!@X:   ~~.",
    "                !MR$**\"(uW$B$$$? :    ~-!?$X!~.iW",
    "                 ``:ne$$B$$$$$~   :     ! ~~.i@R$",
    "                   ~M$$$$*##\"~     :    ~.xW@TXM?"
};

#define SKELETON_LINES 20
#define SKELETON_WIDTH 49


static int cmp_kills(const void *a, const void *b)
{
    // return ((Entity *)b)->kills - ((Entity *)a)->kills;
    return 1;
}

static void draw_skeleton(int rows, int cols)
{
    int x = 3;
    int y = rows - SKELETON_LINES - 4;
 
    for (int i = 0; i < SKELETON_LINES; i++) {
        int draw_row = y + i;
        if (draw_row < 0 || draw_row >= rows - 1) continue;
        const char *art = SKELETON_ART[i];
        int len = (int)strlen(art);
        for (int j = 0; j < len; j++) {
            if (art[j] == ' ') continue;
            attron(COLOR_PAIR(TITLE2) | A_BOLD);
            mvaddch(draw_row, x + j, art[j]);
            attroff(COLOR_PAIR(TITLE2) | A_BOLD);
        }
    }

    x = cols - SKELETON_WIDTH - 3;
    y = rows - SKELETON_LINES - 4;
 
    for (int i = 0; i < SKELETON_LINES; i++) {
        int draw_row = y + i;
        if (draw_row < 0 || draw_row >= rows - 1) continue;
        const char *art = SKELETON_MIRROR_ART[i];
        int len = (int)strlen(art);
        for (int j = 0; j < len; j++) {
            if (art[j] == ' ') continue;
            attron(COLOR_PAIR(TITLE2) | A_BOLD);
            mvaddch(draw_row, x + j, art[j]);
            attroff(COLOR_PAIR(TITLE2) | A_BOLD);
        }
    }
}

static void draw_gameover_title(int rows, int cols)
{
    int drawn_w = GAMEOVER_WIDTH;
    int title_x = (cols - drawn_w) / 2;
    int title_y = 4;

    for (int i = 0; i < GAMEOVER_LINES; i++) {
        const char *art = GAMEOVER_ART[i];
        int art_len = (int)strlen(art);
        int draw_row = title_y + i;
        if (draw_row < 0 || draw_row >= rows - 1) continue;

        for (int j = 0; j < art_len; j ++) {
            int draw_col = title_x + j;
            if (draw_col < 0 || draw_col >= cols) continue;
            if (art[j] == ' ') continue;
            attron(COLOR_PAIR(TITLE3) | A_BOLD);
            mvaddch(draw_row, draw_col, art[j]);
            attroff(COLOR_PAIR(TITLE3) | A_BOLD);
        }
    }
}

static double spin_angle[3] = { 0.0, 0.0, 0.0 };

static void draw_podium(int y0, int cols, Entity *sorted, int n)
{
    static const int slot_rank[3] = { 1, 0, 2 };
    static const int ped_h[3]     = { 2, 4, 1 };
    static const int max_ped      = 4;

    int mid     = cols / 2;
    int gap     = 6;
    int slot_w  = SPR_W + gap;
    int total_w = slot_w * 3 - gap;
    int base_x  = mid - total_w /2;

    int floor_y = y0 + SPR_H + max_ped;

    for (int s = 0; s < 3; s++) {
        int pi = slot_rank[s];
        if (pi >= n) continue;

        int sx   = base_x + s * slot_w;
        int pair = sorted[pi].col;

        int ped_top = floor_y - ped_h[s];
        int sy      = ped_top - SPR_H;

        // ── spinning sprite
        double rel = spin_angle[s];
        int sector = (int)(rel / (M_PI / 4) + 0.5) % 8;
        int dir, mirror;
        switch (sector) {
            case 0: dir = 0; mirror = 0; break;
            case 1: dir = 1; mirror = 0; break;
            case 2: dir = 2; mirror = 0; break;
            case 3: dir = 3; mirror = 0; break;
            case 4: dir = 4; mirror = 0; break;
            case 5: dir = 3; mirror = 1; break;
            case 6: dir = 2; mirror = 1; break;
            case 7: dir = 1; mirror = 1; break;
            default: dir = 0; mirror = 0; break;
        }
        const char **spr = sprite_dir[dir];

        for (int r = 0; r < SPR_H; r++) {
            const char *row = spr[r];
            for (int c = 0; c < SPR_W; c++) {
                int sc = mirror ? (SPR_W - 1 - c) : c;
                if (row[sc] == ' ') continue;
                int spair = entity_color(&sorted[pi], row[sc]);
                attron(COLOR_PAIR(spair) | A_BOLD);
                mvaddch(sy + r, sx + c, ' ');
                attroff(COLOR_PAIR(spair) | A_BOLD);
            }
        }

        // ── pedestal 
        attron(COLOR_PAIR(CP_UI_BDR) | A_BOLD);
        for (int r = 0; r < ped_h[s]; r++)
            mvhline(ped_top + r, sx - 2, ACS_CKBOARD, SPR_W + 4);
        attroff(COLOR_PAIR(CP_UI_BDR) | A_BOLD);

        // ── labels 
        attron(COLOR_PAIR(pair) | A_BOLD);
        // mvprintw(floor_y + 1, SPR_W/2 + sx - 4 , "ID:    %d", sorted[pi].id);
        mvprintw(floor_y , SPR_W/2 + sx - 4, "KILLS: %d", sorted[pi].kills);
        attroff(COLOR_PAIR(pair) | A_BOLD);
    }
}

// ── public entry point ─────────────────────────────────────────────────────
void show_end_screen(Entity *entities, int num_entities)
{
    Entity sorted[MAX_ENTITIES];
    int count = (num_entities < MAX_ENTITIES) ? num_entities : MAX_ENTITIES;
    memcpy(sorted, entities, sizeof(Entity) * count);
    qsort(sorted, count, sizeof(Entity), cmp_kills);

    spin_angle[0] = 0.0;
    spin_angle[1] = 0.0;
    spin_angle[2] = 0.0;

    nodelay(stdscr, TRUE);
    curs_set(0);

    struct timespec ts = { 0, 50000000L };  // 20fps
    int ch = 0;

    while (ch != 'r' && ch != 'R') {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        erase();

        attron(COLOR_PAIR(CP_FLOOR));
        for (int y = 0; y < rows; y++)
            mvhline(y, 0, ' ', cols);
        attroff(COLOR_PAIR(CP_FLOOR));


        int content_y = GAMEOVER_LINES + 5;

        draw_gameover_title(rows, cols);
        draw_skeleton(rows, cols);
        // draw_leaderboard(content_y, cols, sorted, count);
        draw_podium(content_y, cols, sorted, count);

        spin_angle[0] += 0.155*2;
        spin_angle[0] = fmod(spin_angle[0], 2 * M_PI);
        spin_angle[1] += 0.155*3;
        spin_angle[1] = fmod(spin_angle[1], 2 * M_PI);
        spin_angle[2] += 0.155;
        spin_angle[2] = fmod(spin_angle[2], 2 * M_PI);

        attron(COLOR_PAIR(TITLE4) | A_BOLD);
        mvprintw(rows - 6, (cols - 18) / 2, "PRESS R TO RESTART");
        attroff(COLOR_PAIR(TITLE4) | A_BOLD);


        draw_hit_indicator_loop(rows, cols);

        refresh();
        nanosleep(&ts, NULL);

        ch = getch();
        if (ch == ERR) ch = 0;
    }

    cbreak();
    nodelay(stdscr, TRUE);
    erase();
    refresh();
}