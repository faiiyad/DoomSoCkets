#include "player.h"
#include "input.h"
#include "map.h"
#include <math.h>

// Player starts at position (8.5, 8.5) looking right (angle 0)
Player player = {8.5, 8.5, 0.0, 0.0, 0.0};

void updatePlayer(void) {
    double moveSpeed = 0.15;  // World units per frame
    double rotSpeed  = 0.05;  // Radians per frame

    // === ROTATION ===
    if (input.turnLeft)  player.angle -= rotSpeed;
    if (input.turnRight) player.angle += rotSpeed;

    // === MOVEMENT ===
    // cos(angle) = X component of direction, sin(angle) = Y component
    if (input.moveForward) {
        player.x += cos(player.angle) * moveSpeed;
        player.y += sin(player.angle) * moveSpeed;
    }
    if (input.moveBackward) {
        player.x -= cos(player.angle) * moveSpeed;
        player.y -= sin(player.angle) * moveSpeed;
    }
    if (input.strafeLeft) {
        // Strafe left = 90 degrees left of facing direction
        player.x += cos(player.angle - M_PI / 2.0) * moveSpeed;
        player.y += sin(player.angle - M_PI / 2.0) * moveSpeed;
    }
    if (input.strafeRight) {
        // Strafe right = 90 degrees right of facing direction
        player.x += cos(player.angle + M_PI / 2.0) * moveSpeed;
        player.y += sin(player.angle + M_PI / 2.0) * moveSpeed;
    }

    // === COLLISION DETECTION ===
    // If the new position is inside a wall, undo the movement that caused it
    if (mapIsWall((int)player.x, (int)player.y)) {
        if (input.moveForward) {
            player.x -= cos(player.angle) * moveSpeed;
            player.y -= sin(player.angle) * moveSpeed;
        }
        if (input.moveBackward) {
            player.x += cos(player.angle) * moveSpeed;
            player.y += sin(player.angle) * moveSpeed;
        }
        if (input.strafeLeft) {
            player.x -= cos(player.angle - M_PI / 2.0) * moveSpeed;
            player.y -= sin(player.angle - M_PI / 2.0) * moveSpeed;
        }
        if (input.strafeRight) {
            player.x -= cos(player.angle + M_PI / 2.0) * moveSpeed;
            player.y -= sin(player.angle + M_PI / 2.0) * moveSpeed;
        }
    }
}