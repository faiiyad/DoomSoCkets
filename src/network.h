#ifndef NETWORK_H
#define NETWORK_H

#define NETWORK_PORT 13108
#define INITIAL_CLIENTS 1

extern int num_clients;

int send_player_data(double x, double y, double angle, double id);

#endif