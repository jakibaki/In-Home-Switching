#include <switch.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <stdio.h>

#include "context.h"
#include "input.h"
#include "network.h"

struct JoyPkg
{
    unsigned long heldKeys;
    short lJoyX;
    short lJoyY;
    short rJoyX;
    short rJoyY;
};

void gamePadSend(JoyConSocket* connection)
{

    hidScanInput();

    JoystickPosition lJoy;
    JoystickPosition rJoy;
    struct JoyPkg pkg;
    pkg.heldKeys = hidKeysHeld(CONTROLLER_P1_AUTO);
    hidJoystickRead(&lJoy, CONTROLLER_P1_AUTO, JOYSTICK_LEFT);
    hidJoystickRead(&rJoy, CONTROLLER_P1_AUTO, JOYSTICK_RIGHT);
    pkg.lJoyX = lJoy.dx;
    pkg.lJoyY = lJoy.dy;
    pkg.rJoyX = rJoy.dx;
    pkg.rJoyY = rJoy.dy;

    if (send(connection->sock, &pkg, sizeof(pkg), 0) != sizeof(pkg))
    {
        connection->sock = -1;
    }
}

void handleInput(JoyConSocket* connection)
{
    if(connectJoyConSocket(connection, 2223))
        gamePadSend(connection);
}

void inputHandlerLoop(void* dummy)
{
    JoyConSocket* connection = createJoyConSocket();
    while(appletMainLoop())
    {
        handleInput(connection);
        svcSleepThread(23333333);
    }

    freeJoyConSocket(connection);
}