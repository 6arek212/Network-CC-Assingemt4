#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#define SERVER_PORT 5060
#define SIZE 1024

void printError(int code, char *str)
{
    if (code == -1)
    {
        if (str)
        {
            printf("%s\n", str);
            return;
        }
        printf("error\n");
    }
}

int main(int argc, char **argv)
{
    char buf[SIZE];
    socklen_t len;
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    printError(sock, "error sock");

    len = sizeof(buf);
    printError(getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len), "error getsockopt");

    printf("Current: %s\n", buf);

    // "sockaddr_in" is the "derived" from sockaddr structure
    // used for IPv4 communication. For IPv6, use sockaddr_in6
    //
    struct sockaddr_in serverAddress;
    memset(&serverAddress, 0, sizeof(serverAddress));

    serverAddress.sin_family = AF_INET;
    serverAddress.sin_addr.s_addr = INADDR_ANY;
    serverAddress.sin_port = htons(SERVER_PORT); //network order

    // Bind the socket to the port with any IP at this port
    if (bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) == -1)
    {
        printf("Bind failed with error code ");
        return -1;
    }

    printf("Bind() success\n");

    // Make the socket listening; actually mother of all client sockets.
    if (listen(sock, 500) == -1) //500 is a Maximum size of queue connection requests
                                 //number of concurrent connections
    {
        printf("listen() failed with error code ");
        return -1;
    }

    //Accept and incoming connection
    printf("Waiting for incoming TCP-connections...\n");

    struct sockaddr_in clientAddress; //
    socklen_t clientAddressLen = sizeof(clientAddress);
    time_t start, end;
    time_t timeForCubic = 0, timeForReno = 0;

    memset(&clientAddress, 0, sizeof(clientAddress));
    clientAddressLen = sizeof(clientAddress);
    int clientSocket = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLen);
    printError(clientSocket, "error sock");

    // Cubic TCP

    int b;
    time(&start);
    int bytesRecv = 0;
    while ((b = recv(clientSocket, buf, SIZE, 0)) > 0)
    {
        bytesRecv += b;
    }
    time(&end);
    timeForCubic = (end - start);
    printf("bytesRecv %d\n", bytesRecv);

    close(clientSocket);

    // Reno TCP

    strcpy(buf, "reno");
    len = strlen(buf);
    if (setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len) != 0)
    {
        perror("setsockopt");
        return -1;
    }
    len = sizeof(buf);
    if (getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len) != 0)
    {
        perror("getsockopt");
        return -1;
    }

    clientSocket = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLen);
    printError(clientSocket, "error sock");
    printf("New: %s\n", buf);

    bytesRecv = 0;
    time(&start);
    while ((b = recv(clientSocket, buf, SIZE, 0)) > 0)
    {
        bytesRecv += b;
    }
    time(&end);
    timeForReno = end - start;
    close(clientSocket);

    printf("bytesRecv %d\n", bytesRecv);

    //END -- dispaly results

    printf("avrage for cubic %lf seconds\n", (double)timeForCubic / 5);

    printf("avrage for reno %lf seconds\n", (double)timeForReno / 5);

    close(sock);
    return 0;
}
