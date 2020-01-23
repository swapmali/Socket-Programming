#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

int main()
{
    int sock = 0;
    struct sockaddr_in saddr;
    char msg[] = "Hello Server, this is Client 1";  

    sock = socket(AF_INET, SOCK_STREAM, 0);
    saddr.sin_family=AF_INET;
    saddr.sin_port = htons(2222);
    connect(sock, (struct sockaddr *)&saddr, sizeof(saddr));

    send(sock, msg, strlen(msg), 0);
    sleep(20);

    close(sock);

    return 0;
}