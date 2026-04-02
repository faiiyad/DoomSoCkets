#ifndef GUN_H
#define GUN_H


extern int gun_frame;
extern int gun_status;
extern float gun_offset;
extern float gun_offset2;
extern int gun_timer;


void draw_gun(int gun_id, int rows, int cols);

#endif