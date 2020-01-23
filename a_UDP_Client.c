#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>


int main()
{
struct sockaddr_in bindaddress,oppaddress;
char filedata[1024];
char recvdata[1024];
char FileName[200];
char send[5] = "Hello";
int sock;
sock = socket(AF_INET,SOCK_DGRAM,0);

bindaddress.sin_port = htons(8070);
bindaddress.sin_family = AF_INET;
inet_pton(AF_INET,"127.0.0.1",&bindaddress.sin_addr);


bind(sock,(struct sockaddr *)&bindaddress,sizeof(bindaddress));

oppaddress.sin_port = htons(8080);
oppaddress.sin_family = AF_INET;

bzero(&oppaddress.sin_addr,sizeof(oppaddress.sin_addr));

inet_pton(AF_INET,"127.0.0.1",&oppaddress.sin_addr);

sendto(sock,send,5,0,(struct sockaddr*)&oppaddress,sizeof(oppaddress));


int addrsize = sizeof(oppaddress);
int dataReceived = recvfrom(sock,recvdata,1024,0,(struct sockaddr*)&oppaddress,(socklen_t *)&addrsize);

recvdata[dataReceived] = '\0';

printf("%s",recvdata);

printf("Please Enter the filename which you want to receive\n");

scanf("%s",FileName);

sendto(sock,FileName,strlen(FileName),0,(struct sockaddr*)&oppaddress,sizeof(oppaddress));

do
{
int dataReceived = recvfrom(sock,recvdata,10,0,(struct sockaddr*)&oppaddress,(socklen_t *)&addrsize);
recvdata[dataReceived] = '\0';
printf("%s",recvdata);
}while(dataReceived >= 10);
}
