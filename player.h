#ifndef PLAYER_H
#define PLAYER_H

// Holds everything about the player's current state
typedef struct {
    double x, y;    // Position in the world
    double angle;   // Direction the player is facing (radians)
    double vx, vy;  // Velocity (unused, kept for expansion)
} Player;

extern Player player;

void updatePlayer(void);

#endif