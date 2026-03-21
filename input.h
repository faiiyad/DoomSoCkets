#ifndef INPUT_H
#define INPUT_H

typedef struct {
    int moveForward;
    int moveBackward;
    int strafeLeft;
    int strafeRight;
    int turnLeft;
    int turnRight;
} Input;

extern Input input;

void handleInput(void);

#endif