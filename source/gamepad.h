
struct JoyPkg
{
    unsigned long heldKeys;
    short lJoyX;
    short lJoyY;
    short rJoyX;
    short rJoyY;
};

int gamePadSend(int sock);