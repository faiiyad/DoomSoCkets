void on_server_respawn(int respawn_id, double new_x, double new_y){
    if (respawn_id == player.id){
        player.x = new_x;
        player.y = new_y;
    }
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Respawned", respawn_id);
    ui_log_event(buf);
    client_send_position(player.x, player.y, player.angle, 0);
}

void on_server_update(ClientUpdate u)
{
    if (u.id == player.id){
        // ui_log_event("I GOT HIT");
        player.health = u.health;
        trigger_hit_indicator();
        return;
    }
    entity_upsert(u.id, u.col, u.x, u.y, u.angle, u.health, u.kills);
}

void on_server_remove(int id)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d Left", id);
    ui_log_event(buf);
    entity_remove(id);
}

void on_server_kill(int killer_id, int victim_id)
{
    char buf[64];
    snprintf(buf, sizeof(buf), "P%d killed P%d", killer_id, victim_id);
    ui_log_event(buf);
    if (killer_id == player.id) {
        int krab = (player.cur_gun != 4);
        trigger_face_glow(krab);
        player.unlocked_guns = player.unlocked_guns + 1;
        if (player.unlocked_guns > GUN_COUNT) player.unlocked_guns = GUN_COUNT;
        player.cur_gun = player.unlocked_guns - 1;

    } else if (victim_id == player.id) {
        ui_log_event("YOU DIED");
        if (player.health <= 0){
            death(&player);
        }

    }
    entity_upsert_kill(killer_id);
}

void on_server_win(double win_x, double win_y){
    show_end_screen(&player, entities, num_entities);
    player.cur_gun = 0;
    player.unlocked_guns = 1;
    player.kills = 0;
    player.x = win_x;
    player.y = win_y;

}