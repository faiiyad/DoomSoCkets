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
#include "ui.h"

static Player player;

static void init_colors(void)
{
    start_color();
    use_default_colors();

    // SOME SYSTEMS CANT CHANGE COLOUR
    // SO NEED TO CHECK  can_change_color()  but works my machine

    for (int i = 0; i < N_WALL_SHADES; i++) {
        float t = 0.5f + 0.8f * (1.0f - (float)i / (N_WALL_SHADES - 1));

        // front: base rgb(135, 95, 0)
        init_color(WALL_COLOR_BASE + i,
            (short)(529 * t),   // 135/255*1000
            (short)(373 * t),   // 95/255*1000
            (short)(0));        // stays 0

        init_pair(CP_WALL_F1 + i, WALL_COLOR_BASE + i, -1);

        // side: base rgb(95, 65, 0)
        init_color(WALL_COLOR_BASE + N_WALL_SHADES + i,
            (short)(373 * t),   // 95/255*1000
            (short)(254 * t),   // 65/255*1000
            (short)(0));

        init_pair(CP_WALL_S1 + i, WALL_COLOR_BASE + N_WALL_SHADES + i, -1);
    }

    // { pair id, fg, bg }  — use -1 for terminal default bg
    static const short PAL[][3] = {
        // TODO: need to get rid but used by title rn
        { CP_WALL4,    226,          -1  }, 
        { CP_HUD,      COLOR_BLACK,  COLOR_WHITE },
        // ── 3D view (not used but here just in case) ───────────────────
        { CP_WALL1,    94,           -1  },
        { CP_XWALL,    58,           -1  },  // olive — dark side face
        //
        { CP_FLOOR,    240,          -1  },
        { CP_CEIL,     17,           -1  },  // dark navy

        //22
        // ── minimap radar ─────────────────────────────────────────────────
        { CP_MAP_P,     124,         232 },  // red on near-black
        { CP_MAP_EMPTY, 232,         232 },  // near-black solid floor
        { CP_WALL_M,   82,          -1  },  // mid green solid
        //82, 34, 40, 28

        // ── UI widget colors ───────────────────────────────────────────────
        { CP_UI_BDR,   64,          -1  },  // olive fg, dark green bg
        { CP_UI_GOOD,  46,           -1  },  // good/connected/healthy
        { CP_UI_WARN,  226,          -1  },  // warning/medium status
        { CP_UI_BAD,   124,          -1  },  // bad/offline/danger
        { CP_UI_LABEL, 81,           -1  },  // cyan label text
        { CP_UI_TEXT,  250,          -1  },  // neutral light grey

        // ── gun ───────────────────────────────────────────────────────────
        { CP_GUN,      238,          -1  },
        { CP_FLASH,    226,          -1  },
        { HAND_CLR,    124,          -1  },
        { HAND_CLR_S,  131,          -1  },
        { GUN_BDR,     232,          -1  },
        { MUZ_1,       255,          -1  },
        { MUZ_2,       217,          -1  },
        { MUZ_3,       88,           -1  },
        { GUN_ACC,     51,          -1  },  // aqua
        { GUN_BODY,    64,           -1  },  // olive green  — barrel
        { GUN_TRIM,    22,           -1  },  // dark green   — frame
        { GUN_DIRT,    58,           -1  },  // khaki        — worn detail

        // ── title ─────────────────────────────────────────────────────────
        { TITLE1,      93,           -1  },
        { TITLE2,      90,           -1  },
        { TITLE3,      202,          -1  },
        { TITLE4,      208,          -1  },
        { TITLEBG,     0,            -1  },

        // ── entity — red
        { CP_ENTITY_R,  160,       -1  },
        { CP_ENTITY_R1,  160,       160  },
        { CP_ENTITY_R2,  124,       124  },
        { CP_ENTITY_R3,  88,         88  },
 
        // ── entity — yellow
        { CP_ENTITY_Y,  226,       -1  },
        { CP_ENTITY_Y1,  226,       226  },
        { CP_ENTITY_Y2,  220,       220  },
        { CP_ENTITY_Y3,  214,       214  },
 
        // ── entity — blue
        { CP_ENTITY_B,   39,         -1  },
        { CP_ENTITY_B1,  39,         39  },
        { CP_ENTITY_B2,  27,         27  },
        { CP_ENTITY_B3,  21,         21  },
 
        // ── entity — shared
        { CP_ENTITY_W,   255,       255  },  // white eye


        // death skull - white skull, red eyes
        {CP_SKULL_W, 255, -1},
        {CP_SKULL_R, 124, -1},

        // blood for hit indicators.
        {CP_BLOOD_1, 124, -1},
        {CP_BLOOD_2, 160, -1},

        // KILLSSSS
        { CP_FACE_1, 226, -1 },   // bright yellow
        { CP_FACE_2, 220, -1 },   // yellow-orange
        { CP_FACE_3, 214, -1 },   // orange
        { CP_FACE_4, 208, -1 }   // dim orange


    };

    for (int i = 0; i < (int)(sizeof PAL / sizeof PAL[0]); i++)
        init_pair(PAL[i][0], PAL[i][1], PAL[i][2]);
}

void apply_player_color(int col) {
    short fg;
    switch (col) {
        case CP_ENTITY_R: fg = 160; break;
        case CP_ENTITY_Y: fg = 226; break;
        case CP_ENTITY_B: fg =  39; break;
        default:          fg = 255; break;
    }
    init_pair(GUN_ACC,     fg, -1);
    init_pair(CP_UI_LABEL, fg, -1);
}

static void death(Player *player){
    player->x = -100;
    player->y = -100;
    client_send_position(player->x, player->y, player->angle, 0);
    show_death_screen(player);
    client_send_position(player->x, player->y, player->angle, 0);

}

static void on_server_update(ClientUpdate u)
{
    if (u.id == player.id){
        ui_log_event("I GOT HIT");
        player.health = u.health;
        trigger_hit_indicator();
        return;
    }
    entity_upsert(u.id, u.col, u.x, u.y, u.angle, u.health, u.kills);
}

static void on_server_remove(int id)
{
    entity_remove(id);
}

static void on_server_kill(int killer_id, int victim_id)
{
    if (killer_id == player.id) {
        player.kills += 1;
        ui_log_event("I KILLED");
        int krab = (player.cur_gun != 4);
        trigger_face_glow(krab);
        player.unlocked_guns = player.unlocked_guns + 1;
        if (player.unlocked_guns > GUN_COUNT) player.unlocked_guns = GUN_COUNT;
        player.cur_gun = player.unlocked_guns - 1;

    } else if (victim_id == player.id) {
        ui_log_event("I DIED");
        if (player.health <= 0){
            death(&player);
        }

    }
    entity_upsert_kill(killer_id, victim_id);
}

static void on_server_win(int killer_id, int victim_id){
    if (killer_id == player.id) {
        player.kills += 1;
        ui_log_event("I KILLED");
        int krab = (player.cur_gun != 4);
        trigger_face_glow(krab);
        player.unlocked_guns = player.unlocked_guns + 1;
        if (player.unlocked_guns > GUN_COUNT) player.unlocked_guns = GUN_COUNT;
        player.cur_gun = player.unlocked_guns - 1;

    } else if (victim_id == player.id) {
        ui_log_event("I DIED");
        if (player.health <= 0){
            death(&player);
        }

    }
    entity_upsert_kill(killer_id, victim_id);
    show_end_screen(&player, entities, num_entities);

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

    player = (Player){ 0, 8.0, 8.0, 0.0, 100, CP_ENTITY_R, 0, 0, 1};
    map_find_spawn(&player.x, &player.y);
    init_guns();

    // if (net_connect("127.0.0.1") != 0) {
    //     endwin();
    //     fprintf(stderr, "Failed to connect to server\n");
    //     return 1;
    // }
    // entities_init(player.x + 1.0, player.y);
    // client_connect("127.0.0.1", NETWORK_PORT);
    // client_recv_initial(on_server_update);

    

    // show_title_screen();
    
    // flushinp(); typedef struct {

    int show_map = 1;
    int hit_flash = 0; // count down frames to show hit indicator

    struct timespec ts = { 0, 16000000L };  // ~60 fps

    while (1) {
        client_recv_updates(on_server_update, on_server_remove, on_server_kill, on_server_win);
        int ch = getch();

        if ((ch == 'c' || ch == 'C') && !client_is_connected()) {
            client_connect("127.0.0.1", NETWORK_PORT);
            client_recv_initial(&player, on_server_update);
            if(client_is_connected()){
                apply_player_color(player.col);
            }
        }

        if (ch == 'q' || ch == 'Q') break;
        if (ch == 'm' || ch == 'M') show_map = !show_map;


        // PLACEHOLDER FOR TESTING
        if (ch == '5'){
            death(&player);
        }
        // placeholder for testing
        if (ch == '6'){
            trigger_hit_indicator();
        }

        if (ch == '7'){
            int krab = (player.cur_gun != 4);
            trigger_face_glow(krab);
        }
        
        if (ch == '8'){
            show_end_screen(&player, entities, num_entities);
        }


        
        if (ch == 'k' || ch == 'K') {
            if (player.cur_gun == 4){
                player.angle -= ROT_SPD/3;
            }else{
                player.angle -= ROT_SPD;
            }
            
            if (player.angle < 0)        player.angle += 2*M_PI;
            client_send_position(player.x, player.y, player.angle, 0);
        }
        if (ch == 'l' || ch == 'L') {
            if (player.cur_gun == 4){
                player.angle += ROT_SPD/3;
            }else{
                player.angle += ROT_SPD;
            }
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
            client_send_position(player.x, player.y, player.angle, guns[player.cur_gun].dmg);
        }
        if (gun_timer > 0) {
            gun_timer--;
            if (gun_timer == 0) gun_frame = 0;
        }
        // double guns
        if (ch == 'e' || ch == 'E'){
            player.cur_gun = (player.cur_gun + 1) % player.unlocked_guns;
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