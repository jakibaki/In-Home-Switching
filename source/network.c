#include "network.h"
#include <libavutil/timestamp.h>
#include <libavformat/avformat.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>

void networkInit(const SocketInitConfig* conf) 
{
    socketInitialize(conf);
    nxlinkStdio();
    avformat_network_init();
}

void networkDestroy() 
{
    avformat_network_deinit();
    socketExit();
}

JoyConSocket* createJoyConSocket()
{
    JoyConSocket* socket = (JoyConSocket*)malloc(sizeof(JoyConSocket));
    socket->sock = -1;
    socket->lissock = -1;
    return socket;
}

void freeJoyConSocket(JoyConSocket* connection)
{
    free(connection);
}


int connectJoyConSocket(JoyConSocket* connection, int port)
{
    if (connection->lissock == -1)
    {
        if (connection->sock != -1)
        {
            close(connection->sock);
            connection->sock = -1;
        }

        connection->lissock = 1;
    }

    if (connection->sock == -1)
    {
        connection->sock = socket(AF_INET, SOCK_STREAM, 0);
        if (connection->sock < 0) 
        {
            connection->lissock = -1;
            connection->sock = -1;
            printf("Could not create socket\n");
            return 0;
        }

        // Todo: Find the best way for a user to update the server ip  
        struct sockaddr_in server;
        server.sin_addr.s_addr = inet_addr("192.168.0.117");
        server.sin_family = AF_INET;
        server.sin_port = htons(port);

        int failed = connect(connection->sock, (struct sockaddr *)&server, sizeof(server));
        if (failed) {
            connection->sock = -1;
            connection->lissock = -1;
            printf("Could not connect socket\n");
            return 0;
        }
        printf("Established gamepad-connection\n");
    }
    return 1;
}

void sendJoyConInput(JoyConSocket* connection, const JoyPkg* pkg)
{
    if (send(connection->sock, pkg, sizeof(JoyPkg), 0) != sizeof(JoyPkg))
    {
        connection->sock = -1;
    }
}
