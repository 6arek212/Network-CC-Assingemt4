#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <arpa/inet.h>

#define SERVER_PORT 5060
#define SERVER_IP_ADDRESS "10.0.2.15"
#define SIZE 1024

char *filename = "1gb.txt";

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

void sendData(int sock)
{
	FILE *f;
	printf("connected to server\n");
	char buffer[SIZE] = {0};
	f = fopen(filename, "rb");
	while (fgets(buffer, SIZE, f))
	{
		send(sock, buffer, sizeof(buffer), 0);
	}
}

int main(int argc, char **argv)
{
	int sock;
	char buf[256];
	socklen_t len;

	sock = socket(AF_INET, SOCK_STREAM, 0);
	printError(sock, "error sock");
	len = sizeof(buf);
	printError(getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len), "error sock");
	printf("Current: %s\n", buf);

	struct sockaddr_in serverAddress;
	memset(&serverAddress, 0, sizeof(serverAddress));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(SERVER_PORT);
	int rval = inet_pton(AF_INET, (const char *)SERVER_IP_ADDRESS, &serverAddress.sin_addr);
	printError(rval, "error rval");
	printError(connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)), "error connect() failed with error code ");

	for (int i = 0; i < 5; i++)
	{
		sendData(sock);
	}

	close(sock);



	sock = socket(AF_INET, SOCK_STREAM, 0);
	strcpy(buf, "reno");
	len = strlen(buf);
	printError(setsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, len), "error sock");
	len = sizeof(buf);
	printError(getsockopt(sock, IPPROTO_TCP, TCP_CONGESTION, buf, &len), "error sock");
	printError(connect(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress)), "error connect() failed with error code ");
	printf("New: %s\n", buf);

	for (int i = 0; i < 5; i++)
	{
		sendData(sock);
	}
	close(sock);


	return 0;
}
