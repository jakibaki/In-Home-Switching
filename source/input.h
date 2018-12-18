#ifndef INPUT_H
#define INPUT_H

bool is_running;

/* Loop to handle joycon inputs and send theme to the server */
void inputHandlerLoop(void* dummy);

#endif