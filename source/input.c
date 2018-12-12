#include <switch.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

struct JoyPkg
{
    unsigned long heldKeys;
    short lJoyX;
    short lJoyY;
    short rJoyX;
    short rJoyY;
};

int lissock = -1;
int sock = -1;

int handleInput()
{
    if (lissock = -1)
    {
        if (sock != -1)
        {
            close(sock);
            sock = -1;
        }

        struct sockaddr_in server;

        lissock = socket(AF_INET, SOCK_STREAM, 0);
        server.sin_family = AF_INET;
        server.sin_addr.s_addr = INADDR_ANY;
        server.sin_port = htons(2223);

        if (bind(lissock, (struct sockaddr *)&server, sizeof(server)) < 0)
        {
            close(lissock);
            lissock = -1;
            return;
        }

        listen(lissock, 1);
    }

    if (sock == -1)
    {
        // TODO: We might want to be able to not block if no client is connected
        struct sockaddr_in client;
        int c = sizeof(struct sockaddr_in);
        sock = accept(lissock, (struct sockaddr *)&client, (socklen_t *)&c);
        if(sock < 0) {
            close(lissock);
            sock = -1;
            lissock = -1;
        }
    }

    if (sock > 0)
        gamePadSend();
}

void gamePadSend()
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
        sock = -1;
    }
}