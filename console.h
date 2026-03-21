#ifndef CONSOLE_H
#define CONSOLE_H

// Screen dimensions - set at startup by reading the actual terminal size
extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;

void initConsole(void);
void shutdownConsole(void);
void drawHUD(void);

#endif