#define _XOPEN_SOURCE_EXTENDED
#include <ncurses.h>
#include <stdlib.h>
#include <time.h>
#include <locale.h>
#include <math.h>
#include "defs.h"
#include "map.h"
#include "player.h"
#include "gun.h"
#include "render.h"
#include "title.h"
#include "entity.h"
#include "network.h"
#include "client.h"

static void init_colors(void)
{
    start_color();
    use_default_colors();

    // { pair id, fg, bg }  — use -1 for terminal default bg
    static const short PAL[][3] = {
        // ── 3D view ───────────────────────────────────────────────────────
        { CP_WALL1,    94,           -1  },
        { CP_WALL2,    51,           -1  },  // bright cyan
        { CP_WALL3,    46,           -1  },  // bright green
        { CP_WALL4,    226,          -1  },  // bright yellow
        { CP_XWALL,    58,           -1  },  // olive — dark side face
        { CP_FLOOR,    240,          -1  },
        { CP_CEIL,     17,           -1  },  // dark navy
        { CP_HUD,      COLOR_BLACK,  COLOR_WHITE },

        //22
        // ── minimap radar ─────────────────────────────────────────────────
        { CP_MAP_BDR,   64,          -1  },  // olive fg, dark green bg
        { CP_MAP_P,     124,         232 },  // red on near-black
        { CP_MAP_EMPTY, 232,         232 },  // near-black solid floor
        { CP_WALL1_M,   82,          -1  },  // mid green solid
        { CP_WALL2_M,   34,          -1  },  // green solid
        { CP_WALL3_M,   40,          -1  },  // bright green solid
        { CP_WALL4_M,   28,          -1  },  // lime solid

        // ── gun ───────────────────────────────────────────────────────────
        { CP_GUN,      238,          -1  },
        { CP_FLASH,    226,          -1  },
        { HAND_CLR,    124,          -1  },
        { HAND_CLR_S,  131,          -1  },
        { GUN_BDR,     255,          -1  },
        { MUZ_1,       230,          -1  },
        { MUZ_2,       217,          -1  },
        { MUZ_3,       88,           -1  },
        { GUN_BODY,    64,           -1  },  // olive green  — barrel
        { GUN_TRIM,    22,           -1  },  // dark green   — frame
        { GUN_DIRT,    58,           -1  },  // khaki        — worn detail

        // ── title ─────────────────────────────────────────────────────────
        { TITLE1,      93,           -1  },
        { TITLE2,      90,           -1  },
        { TITLE3,      202,          -1  },
        { TITLE4,      208,          -1  },
        { TITLEBG,     0,            -1  },

        // entity
        { CP_ENTITY_FAR, 160,       160  },  // red
        { CP_ENTITY_R, 160,         160  },  // red
        { CP_ENTITY_D, 88,          88 },  // dark red/brown shadow
        { CP_ENTITY_O, 124,         124  },  // orange highlight
        { CP_ENTITY_W, 255,         255  },  // white eye
    };

    for (int i = 0; i < (int)(sizeof PAL / sizeof PAL[0]); i++)
        init_pair(PAL[i][0], PAL[i][1], PAL[i][2]);
}

static void on_server_update(ClientUpdate u)
{
    // hook into your entity system here
    // printf("entity %d => %.2f %.2f %.2f hp=%d\n",
        //    u.id, u.x, u.y, u.angle, u.health);
    entity_upsert(u.id, u.x, u.y, u.angle, u.health);
}

int main(void)
{
    setlocale(LC_ALL, "");
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    nodelay(stdscr, TRUE);
    curs_set(0);
    init_colors();

    Player player = { 8.0, 8.0, 0.0 };
    map_find_spawn(&player.x, &player.y);

    entities_init(player.x + 1.0, player.y);
    client_connect("127.0.0.1", NETWORK_PORT);


    // show_title_screen();
    // flushinp(); 

    int show_map = 1;

    struct timespec ts = { 0, 16000000L };  // ~60 fps

    while (1) {
        client_recv_updates(on_server_update);
        int ch = getch();
        if (ch == 'q' || ch == 'Q') break;
        if (ch == 'm' || ch == 'M') show_map = !show_map;
        
        if (ch == 'k' || ch == 'K') {
            player.angle -= ROT_SPD;
            if (player.angle < 0)        player.angle += 2*M_PI;
            client_send_position(player.x, player.y, player.angle, 0);
        }
        if (ch == 'l' || ch == 'L') {
            player.angle += ROT_SPD;
            if (player.angle >= 2*M_PI)  player.angle -= 2*M_PI;
            client_send_position(player.x, player.y, player.angle, 0);
        }

        double nx = player.x, ny = player.y;
        if (ch == KEY_UP   || ch == 'w' || ch == 'W') {
            nx += cos(player.angle) * MOVE_SPD;
            ny += sin(player.angle) * MOVE_SPD;
            client_send_position(nx, ny, player.angle, 0);
        }
        if (ch == KEY_DOWN || ch == 's' || ch == 'S') {
            nx -= cos(player.angle) * MOVE_SPD;
            ny -= sin(player.angle) * MOVE_SPD;
            client_send_position(nx, ny, player.angle, 0);
        }
        if (ch == 'a' || ch == 'A' || ch == ',') {
            nx += cos(player.angle - M_PI / 2) * MOVE_SPD;
            ny += sin(player.angle - M_PI / 2) * MOVE_SPD;
            client_send_position(nx, ny, player.angle, 0);
        }
        if (ch == 'd' || ch == 'D' || ch == '.') {
            nx += cos(player.angle + M_PI / 2) * MOVE_SPD;
            ny += sin(player.angle + M_PI / 2) * MOVE_SPD;
            client_send_position(nx, ny, player.angle, 0);
        }

        if (ch == ' ' && gun_frame == 0) {
            gun_frame = 1;
            gun_timer = 10;
        }
        if (gun_timer > 0) {
            gun_timer--;
            if (gun_timer == 0) gun_frame = 0;
        }
        // double guns
        if (ch == 'e' || ch == 'E'){
            if (gun_status == 0){
                gun_status = 1;
            }
            else{
                gun_status = 0;
            }
        }

        entities_update(&player, ch);

        double margin = 0.2;
        if (!map_solid((int)(nx + margin), (int)(player.y)) &&
            !map_solid((int)(nx - margin), (int)(player.y))) player.x = nx;
        if (!map_solid((int)(player.x), (int)(ny + margin)) &&
            !map_solid((int)(player.x), (int)(ny - margin))) player.y = ny;

        render(&player, show_map);
        nanosleep(&ts, NULL);
    }

    client_disconnect();
    endwin();
    printf("YO THANKS FOR PLAYING\n");
    return 0;
}