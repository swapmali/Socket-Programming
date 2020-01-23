#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include<string.h>

int main()
{
    int sock = 0;
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);
    char buffer[1024] = {0};
    int conn;
    int *status = NULL;
    sock = socket(AF_INET, SOCK_STREAM, 0);
    addr.sin_family=AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(2222);
    bind(sock, (struct sockaddr *)&addr, sizeof(addr));

    listen(sock,5);
    while(1)
    {
        conn = accept(sock, (struct sockaddr *)&addr, (socklen_t *)&addrlen);
        int pid = fork();
        if(pid==0)
        {
            waitpid(-1, &status, WUNTRACED | WCONTINUED);
            char buffer[1024] = {0};
            int readval = read(conn, buffer, 1024);
            printf("%s\n",buffer);
            break;
        }
        else if(pid < 0) printf("Process creation error");

    }

    close(conn);
    close(sock);

    return 0;
}