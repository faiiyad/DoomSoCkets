#ifndef SERVER_UI_H
#define SERVER_UI_H

#include <poll.h>
#include "client_manager.h"

#define SERVER_LOG_LINES 16
#define SERVER_LOG_LINE_LENGTH 256

void init_server_ui(void);
void shutdown_server_ui(void);
void server_log(const char *fmt, ...);
void draw_server_ui(Client *clients, nfds_t nfds);
void server_ui_handle_resize(void);
int  server_ui_get_input(void);

#endif
