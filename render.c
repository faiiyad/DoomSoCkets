#include "render.h"
#include "player.h"
#include "map.h"
#include "console.h"
#include <math.h>
#include <ncurses.h>

// ============================================================================
// Cast a single ray from the player at the given angle
//
// The ray marches forward in small steps until it hits a wall or exceeds
// MAX_DISTANCE. This is called once per screen column every frame.
// ============================================================================
double castRay(double angle, int* wallType) {
    double sinA = sin(angle);
    double cosA = cos(angle);

    double stepSize = 0.05;  // Smaller = more accurate but slower
    double distance = 0.0;
    double x = player.x;
    double y = player.y;

    while (distance < MAX_DISTANCE) {
        x += cosA * stepSize;
        y += sinA * stepSize;
        distance += stepSize;

        int mapX = (int)x;
        int mapY = (int)y;

        // Out of bounds counts as a wall
        if (mapX < 0 || mapX >= MAP_WIDTH || mapY < 0 || mapY >= MAP_HEIGHT) {
            *wallType = 1;
            return distance;
        }

        if (worldMap[mapY][mapX] != 0) {
            *wallType = worldMap[mapY][mapX];
            return distance;
        }
    }

    *wallType = 0;
    return MAX_DISTANCE;
}

// ============================================================================
// Render the full 3D view
//
// For each screen column we cast one ray, measure the wall distance, and draw
// a vertical strip of ceiling / wall / floor characters.
//
// Wall height formula: height = scale / distance
// This is perspective projection - distant things appear smaller.
// ============================================================================
void renderView(void) {
    double fovRad = FOV * M_PI / 180.0;

    for (int col = 0; col < SCREEN_WIDTH; col++) {
        // Spread rays evenly across the field of view
        double rayAngle = player.angle - fovRad / 2.0 + (fovRad * col / SCREEN_WIDTH);

        int wallType;
        double distance = castRay(rayAngle, &wallType);

        // Fish-eye correction - without this edges look curved
        distance = distance * cos(rayAngle - player.angle);

        // Perspective projection - closer walls are taller on screen
        int wallHeight = (int)(SCREEN_HEIGHT * 0.5 / (distance + 0.1));
        if (wallHeight > SCREEN_HEIGHT) wallHeight = SCREEN_HEIGHT;

        int wallTop    = SCREEN_HEIGHT / 2 - wallHeight / 2;
        int wallBottom = SCREEN_HEIGHT / 2 + wallHeight / 2;

        // --- Ceiling ---
        for (int row = 0; row < wallTop; row++) {
            mvaddch(row, col, ' ');
        }

        // --- Wall ---
        // Wall type 2 uses % to look different from type 1
        char wallChar = (wallType == 2 && distance >= 3.0) ? '%' : '#';
        for (int row = wallTop; row < wallBottom; row++) {
            if (row >= 0 && row < SCREEN_HEIGHT)
                mvaddch(row, col, wallChar);
        }

        // --- Floor ---
        // Stop one row before the bottom to leave room for the HUD
        for (int row = wallBottom; row < SCREEN_HEIGHT - 1; row++) {
            mvaddch(row, col, '-');
        }
    }
}