#include <stdio.h>
#include "client_fn.h"
#include "render.h"
#include "ui.h"
#include "title.h"
#include "client.h"
#include "entity.h"
#include "gun.h"

void on_server_initial(ClientUpdate u, Player *player){
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Joined", u.id);
    ui_log_event(buf);
    player->id = u.id;
    player->x = u.x;
    player->y = u.y;
    player->angle = u.angle;
    player->col = col_from_char(u.col);
    player->kills = u.kills;
}

void on_server_respawn(int respawn_id, double new_x, double new_y, Player *player){
    if (respawn_id == player->id){
        player->x = new_x;
        player->y = new_y;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Respawned", respawn_id);
    ui_log_event(buf);
    client_send_position(player->x, player->y, player->angle, 0);
}

void on_server_update(ClientUpdate u, Player *player)
{
    if (u.id == player->id){
        player->health = u.health;
        trigger_hit_indicator();
        return;
    }
    int isnew = 0;
    entity_upsert(u.id, u.col, u.x, u.y, u.angle, u.health, u.kills, &isnew);
    if (isnew){
        char buf[64];
        snprintf(buf, sizeof(buf), "P%d Joined", u.id);
        ui_log_event(buf);
    }
}

void on_server_remove(int id)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Left", id);
    ui_log_event(buf);
    entity_remove(id);
}

void on_server_kill(int killer_id, int victim_id, Player *player)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d killed P%d", killer_id, victim_id);
    ui_log_event(buf);
    if (killer_id == player->id) {
        int krab = (player->cur_gun != 4);
        trigger_face_glow(krab);
        player->unlocked_guns = player->unlocked_guns + 1;
        if (player->unlocked_guns > GUN_COUNT) player->unlocked_guns = GUN_COUNT;
        player->cur_gun = player->unlocked_guns - 1;

    } else if (victim_id == player->id) {
        if (player->health <= 0){
            player->x = -100;
            player->y = -100;
            client_send_position(player->x, player->y, player->angle, 0);
            show_death_screen();
            player->health = 100;
        }
    }
    entity_upsert_kill(killer_id);
}

void on_server_win(double win_x, double win_y, Player *player){
    ui_log_event("Game Over");
    show_end_screen(player, entities, num_entities);
    player->cur_gun = 0;
    player->unlocked_guns = 1;
    player->kills = 0;
    player->x = win_x;
    player->y = win_y;

}