#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "title.h"
#include "defs.h"
#include "render.h"


static const char *TITLE_ART[] = {
    " @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@   @@@@@@@@@",
    " @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@  @@@@@@@@",
    " @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@  @@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@ @@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@@@@@@@@@@@@",
    " @@@@@@@    @@@@@@@  @@@@@@@    @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@ @@ @@@@@@@",
    " @@@@@@@  @@@@@@@@@  @@@@@@@@@  @@@@@@@  @@@@@@@     @@@@@@  @@@@@@@ @@ @@@@@@@",
    " @@@@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@@@@  @@@@@@@@  @@@@@@@@  @@@@@@@ @@ @@@@@@@",
    " @@@@@@@@@@@@@@@@@@    @@@@@@@@@@@@@@@@  @@@@@@@@@@@@@@@     @@@@@@@    @@@@@@@",
    " @@@@@@@@@@@@@@@          @@@@@@@@@@@@@  @@@@@@@@@@@@@         @@@@@    @@@@@@@",
    " @@@@@@@@@@@@@              @@@@@@@@@      @@@@@@@@@              @@    @@@@@@@",
    " @@@@@@@@@@                    @@@@          @@@@                       @@@@@@@",
    " @@@@@@@@                                                               @@@@@@@",
    " @@@@@                                                                   @@@@@@",
    " @@@      @@@@@@@  @@@@@@@  @@@@@@@  @@   @    @@@@   @@@@@@@  @@@@@@@     @@@@",
    " @        @@@  @@  @@  @@@  @@   @@  @@  @@  @@@  @@  @@@@@@@  @@@  @@        @",
    "          @@@      @@  @@@  @@       @@@@    @@@        @@@    @@@",
    "           @@@@@   @@  @@@  @@       @@@@    @@@@@@@    @@@     @@@@@",
    "              @@@  @@  @@@  @@   @@  @@  @@  @@@        @@@        @@@",
    "          @@   @@  @@  @@@  @@@@@@@  @@  @@  @@@  @@    @@@    @@   @@",
    "            @@@@    @@@@     @@@@    @@  @     @@@@      @       @@@@"
};

static const char *TITLE_CLR[] = {
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", /* blue */
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB",
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 
    "BBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBBB", 
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", /* dark red */
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 
    "DDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDDD", 
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", /* orange */
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", 
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", 
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", 
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", 
    "OOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOOO", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", /* yellow */
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
    "YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY", 
};

#define TITLE_LINES 27
#define TITLE_WIDTH 81

static const char *SUBTITLE = "Faiyad Ahmed Masnoon | Jia Ying Lee | Bryan Yee";
static const char *PROMPT   = "[ PRESS ENTER TO PLAY ]";

static int get_title_color(char c)
{
    switch (c) {
        case 'B': return TITLE1;
        case 'D': return TITLE2;
        case 'O': return TITLE3;
        case 'Y': return TITLE4;
        default:  return TITLE4;
    }
}

void draw_heartbeat_loop(int rows, int cols)
{
    static int tick = 0;
    tick++;

    float intensity = 0.3f + 0.5f * sinf(tick * 0.1f);
    int depth = 5 + (int)(5 * intensity);
    unsigned int seed = 42;

    for (int y = 0; y < rows; y++) {
        for (int x = 0; x < cols; x++) {
            if (y == rows - 1 && x == cols - 1) continue;

            int d_top    = y;
            int d_bottom = rows - 1 - y;
            int d_left   = x;
            int d_right  = cols - 1 - x;
            int d_edge   = d_top < d_bottom ? d_top : d_bottom;
            if (d_left  < d_edge) d_edge = d_left;
            if (d_right < d_edge) d_edge = d_right;
            if (d_edge >= depth) continue;

            seed = seed * 1664525u + (unsigned)(y * 1000 + x);
            int r = seed % 100;
            int threshold = (int)(90 * (1.0f - (float)d_edge / depth));
            if (r > threshold) continue;

            char ch;
            if (d_edge == 0)      ch = '#';
            else if (d_edge <= 2) ch = (r % 3 == 0) ? '*' : '%';
            else if (d_edge <= 5) ch = (r % 2 == 0) ? '.' : ',';
            else                  ch = '.';

            
            int pair = (d_edge <= depth / 3) ? TITLERED2 : TITLERED;
            // int pair = title_colors[(d_edge + tick / 5) % 4];

            attron(COLOR_PAIR(pair) | A_BOLD);
            mvaddch(y, x, ch);
            attroff(COLOR_PAIR(pair) | A_BOLD);
        }
    }
}

void name_printer(char *name, int index, char c){
    int color = get_title_color(c);
    attron(COLOR_PAIR(color));

    mvprintw(index, 0, "%s", name);
    
    attroff(COLOR_PAIR(color));            
};


void show_title_screen(void)
{
    nodelay(stdscr, TRUE);
    curs_set(0);

    int ch = 0;
    

    while (ch != '\n' && ch != '\r' && ch != KEY_ENTER) {
        int rows, cols;
        getmaxyx(stdscr, rows, cols);
        erase();


        /*  dark background  */
        attron(COLOR_PAIR(CP_FLOOR));
        for (int y = 0; y < rows; y++)
            mvhline(y, 0, ' ', cols);
        attroff(COLOR_PAIR(CP_FLOOR));

        draw_heartbeat_loop(rows, cols);

        /*  title art  */
        int art_start = (rows - TITLE_LINES) / 2 - 3;
        if (art_start < 1) art_start = 1;

        for (int i = 0; i < TITLE_LINES; i++) {
            int ox = (cols - TITLE_WIDTH) / 2;
            if (ox < 0) ox = 0;
            const char *art = TITLE_ART[i];
            const char *clr = TITLE_CLR[i];

            for (int j = 0; art[j] != '\0' && clr[j] != '\0'; j++) {
                if (art[j] == ' ') continue;
                int pair = get_title_color(clr[j]);
                attron(COLOR_PAIR(pair) | A_BOLD);
                mvaddch(art_start + i, ox + j, '#');
                attroff(COLOR_PAIR(pair) | A_BOLD);
            }
        }

        int sub_y = art_start + TITLE_LINES + 1;
        int sub_x = (cols - 47) / 2;
        attron(COLOR_PAIR(CP_WALL4) | A_BOLD);
        mvprintw(sub_y, sub_x < 0 ? 0 : sub_x, "%s", SUBTITLE);
        attroff(COLOR_PAIR(CP_WALL4) | A_BOLD);

        
        int pr_x = (cols - 23) / 2;
        int pr_y = sub_y + 2;
        attron(COLOR_PAIR(CP_WALL4) | A_BOLD);
        mvprintw(pr_y, pr_x < 0 ? 0 : pr_x, "%s", PROMPT);
        attroff(COLOR_PAIR(CP_WALL4) | A_BOLD);

        refresh();

        // remove halfdelay(5);
        nanosleep(&(struct timespec){0, 16000000L}, NULL);
        ch = getch();
        if (ch == ERR) ch = 0;
    }

    
    

    cbreak();
    nodelay(stdscr, TRUE);

    // nuke transition
    int rows, cols;
    getmaxyx(stdscr, rows, cols);

    #define MAX_PARTS 1600
    typedef struct {
        float x, y, vx, vy;
        int   pair;
        char  ch;
        int   life;
        int   max_life;
    } Particle;

    static Particle parts[MAX_PARTS];

    float cx = cols / 2.0f;
    float cy = rows * 0.9f; 

    static const char sparks[] = "*oO.+#@%$&!";
    static const int  pairs[]  = {
        MUZ_1, MUZ_1, MUZ_2, MUZ_2,
        MUZ_3, CP_WALL4, CP_WALL1,
        HAND_CLR, CP_MAP_P
    };

    srand((unsigned)time(NULL));

    for (int i = 0; i < MAX_PARTS; i++) {
        float angle = ((float)rand() / RAND_MAX) * 3.14159f;
        float speed = 1.0f + ((float)rand() / RAND_MAX) * 5.0f;

        parts[i].x = cx + ((float)rand()/RAND_MAX - 0.5f) * 6;
        parts[i].y = cy + ((float)rand()/RAND_MAX - 0.5f) * 2;

        parts[i].vx = cosf(angle) * speed * 1.8f;
        parts[i].vy = -fabsf(sinf(angle)) * speed * 1.5f;

        parts[i].pair = pairs[rand() % 9];
        parts[i].ch   = sparks[rand() % 11];

        parts[i].life     = 20 + rand() % 40;
        parts[i].max_life = parts[i].life;
    }

    int total_frames = 70;

    for (int frame = 0; frame < total_frames; frame++) {

        int bg_pair;
        char bg_ch;

        if (frame < 3) {
            bg_pair = CP_UI_BDR;  bg_ch = ' ';
        } else if (frame < 10) {
            bg_pair = MUZ_1;     bg_ch = '#';
        } else if (frame < 20) {
            bg_pair = CP_WALL4;  bg_ch = '%';
        } else if (frame < 35) {
            bg_pair = CP_WALL1;  bg_ch = '.';
        } else if (frame < 50) {
            bg_pair = HAND_CLR;  bg_ch = '.';
        } else {
            bg_pair = CP_FLOOR;  bg_ch = ' ';
        }

        attron(COLOR_PAIR(bg_pair) | A_BOLD);
        for (int y = 0; y < rows; y++)
            mvhline(y, 0, bg_ch, cols);
        attroff(COLOR_PAIR(bg_pair) | A_BOLD);

        if (frame >= 2 && frame < 18) {
            float base_r = (frame - 2) * (cols / 2.0f / 16.0f);

            int sw_pair = (frame < 8) ? MUZ_1 :
                        (frame < 13) ? CP_WALL4 : HAND_CLR;

            attron(COLOR_PAIR(sw_pair) | A_BOLD);

            for (float a = 0; a < 2.0f * 3.14159f; a += 0.04f) {
                float noise = ((rand() % 100) / 100.0f - 0.5f) * 2.5f;
                float r = base_r + noise;

                int sx = (int)(cx + cosf(a) * r);
                int sy = (int)(cy + sinf(a) * r * 0.6f);

                if (sx >= 0 && sx < cols && sy >= 0 && sy < rows - 1)
                    mvaddch(sy, sx, (rand()%2) ? 'O' : '0');
            }

            attroff(COLOR_PAIR(sw_pair) | A_BOLD);
        }

        if (frame >= 4 && frame < 40) {
            float radius = (frame - 4) * (cols / 2.5f / 36.0f);

            for (float a = 0; a < 2.0f * 3.14159f; a += 0.03f) {

                float noise = ((rand()%100)/100.0f - 0.5f) * 4.0f;
                float r = radius + noise;

                int x = (int)(cx + cosf(a) * r);
                int y = (int)(cy + sinf(a) * r * 0.35f);

                if (x < 0 || x >= cols || y < 0 || y >= rows-1) continue;

                int pair =
                    (frame < 10) ? MUZ_2 :
                    (frame < 20) ? CP_WALL4 :
                    (frame < 30) ? CP_WALL1 :
                    (frame < 45) ? HAND_CLR :
                    (rand()%2 ? CP_FLOOR : CP_GUN);

                attron(COLOR_PAIR(pair));

                char ch =
                    (rand()%3==0) ? '@' :
                    (rand()%2==0) ? '#' : '.';

                mvaddch(y, x, ch);

                attroff(COLOR_PAIR(pair));
            }
        }

        if (frame >= 1) {
            float rise = (frame - 5) * 0.6f;

            int base = (int)cy;
            for (int y = base; y > base - rise && y >= 0; y--) { 

                float t = (cy - y) / (float)(rows * 0.5f);
                int half_w = 2 + (int)(t * 6);

                for (int dx = -half_w; dx <= half_w; dx++) {
                    int x = (int)cx + dx;
                    if (x < 0 || x >= cols) continue;

                    float noise =
                        sinf(y * 0.25f + frame * 0.3f) +
                        cosf(x * 0.15f);

                    if (noise > -0.5f) {
                        int pair =
                            (frame < 12) ? MUZ_1 :
                            (frame < 22) ? CP_WALL4 :
                            (frame < 35) ? CP_WALL1 :
                            (frame < 50) ? HAND_CLR :
                            (rand()%2 ? CP_FLOOR : CP_GUN);  

                        attron(COLOR_PAIR(pair));

                        char ch =
                            (noise > 0.5f) ? '@' :
                            (noise > 0.0f) ? '#' : '.';

                        mvaddch(y, x, ch);
                        attroff(COLOR_PAIR(pair));
                    }
                }
            }
        }

        if (frame >= 6) {
            float rise = (frame - 6) * 0.6f;

            for (int y = 0; y < rows; y++) {
                for (int x = 0; x < cols; x++) {

                    float dx = (x - cx) / (cols * 0.25f);
                    float dy = (y - (cy - rise)) / (rows * 0.2f);

                    float dist = dx*dx + dy*dy;

                    float noise = sinf(x * 0.2f + frame * 0.3f) *
                                cosf(y * 0.2f) * 0.3f;

                    if (dist + noise < 1.0f && y < cy) {
                        int pair =
                            (frame < 18) ? MUZ_1 :
                            (frame < 30) ? CP_WALL4 :
                            (frame < 45) ? CP_WALL1 :
                            (frame < 60) ? HAND_CLR :
                            (rand()%2 ? CP_FLOOR : CP_GUN);

                        attron(COLOR_PAIR(pair));
                        mvaddch(y, x, (rand()%3==0) ? '@' : '#');
                        attroff(COLOR_PAIR(pair));
                    }
                }
            }
        }

        for (int i = 0; i < MAX_PARTS; i++) {
            if (parts[i].life <= 0) continue;

            parts[i].vx += ((rand()%100)/100.0f - 0.5f) * 0.05f;

            parts[i].x += parts[i].vx;
            parts[i].y += parts[i].vy;

            parts[i].vy += 0.03f;
            parts[i].vx *= 0.99f;

            parts[i].life--;

            int px = (int)parts[i].x;
            int py = (int)parts[i].y;

            if (px < 0 || px >= cols || py < 0 || py >= rows - 1) continue;

            float ratio = (float)parts[i].life / parts[i].max_life;
            int attr = (ratio > 0.5f) ? A_BOLD : 0;

            attron(COLOR_PAIR(parts[i].pair) | attr);
            mvaddch(py, px, parts[i].ch);
            attroff(COLOR_PAIR(parts[i].pair) | attr);
        }

        if (frame < 10) {
            int sx = (rand() % 5) - 2;
            int sy = (rand() % 3) - 1;

            attron(COLOR_PAIR(MUZ_1) | A_BOLD);
            mvprintw(rows/2 + sy, cols/2 - 4 + sx, "KABOOM!!");
            attroff(COLOR_PAIR(MUZ_1) | A_BOLD);
        }

        refresh();

        struct timespec ts = { 0, 16000000L };
        nanosleep(&ts, NULL);
    }

    #undef MAX_PARTS

    erase();
    refresh();
}