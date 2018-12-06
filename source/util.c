#include <switch.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>


int setupServerSocket(int port)
{
    int lissock;
    struct sockaddr_in server;
    lissock = socket(AF_INET, SOCK_STREAM, 0);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(port);

    while (bind(lissock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        svcSleepThread(1e+9L);
    }
    listen(lissock, 1);
    return lissock;
}