#ifndef MAP_H
#define MAP_H

#define MAP_WIDTH 32
#define MAP_HEIGHT 24

extern int worldMap[MAP_HEIGHT][MAP_WIDTH];

// Returns 1 if the given world coordinate is inside a wall, 0 if not
int mapIsWall(int x, int y);

#endif