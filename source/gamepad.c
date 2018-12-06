#include <switch.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

#include "util.h"

struct JoyPkg
{
    unsigned long heldKeys;
    short lJoyX;
    short lJoyY;
    short rJoyX;
    short rJoyY;
};

int gamePadSend(int sock)
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
    if (send(sock, &pkg, sizeof(pkg), 0) != sizeof(pkg))
    {
        return -1;
    }
    return 0;
}