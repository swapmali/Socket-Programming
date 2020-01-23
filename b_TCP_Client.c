#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include <arpa/inet.h> 
#include<unistd.h>


int main()
{
struct sockaddr_in connectionsocket;

int soc;
soc = socket(AF_INET,SOCK_STREAM,0);

char filename[10];

char data[1024];

connectionsocket.sin_family = AF_INET; 
connectionsocket.sin_port = htons(8080);

inet_pton(AF_INET,"127.0.0.1",&connectionsocket.sin_addr);

//bind(socket,(struct sockaddr *)&address,sizeof(address));

connect(soc,(struct sockaddr *)&connectionsocket,sizeof(connectionsocket));
int size=10;
while(size==10)
{
	size = recv(soc,data,10,0);
	int i=0;
	for(i=0;i<size;i++)
		printf("%c",data[i]);
}


scanf("%s",filename);

send(soc,filename,strlen(filename)+1,0);

sleep(2);

size = 1;
while(size == 1)
{
	size = recv(soc,data,1,0);
	int i=0;
	printf("%c",data[i]);
}

}	

