#ifndef NETWORK_H
#define NETWORK_H

#define NETWORK_PORT 13108
#define INITIAL_CLIENTS 1

#define ENTITY_SCAN_FMT  "%lf %lf %lf %d"
#define ENTITY_FMT     "%d %c %.2f %.2f %.2f %d\n"
#define ENTITY_FMT_ARGS(e) (e)->id, (e)->col, (e)->x, (e)->y, (e)->angle, (e)->health 

extern int num_clients;

int send_player_data(double x, double y, double angle, double id);

#endif