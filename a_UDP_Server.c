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
char filename[200];
int sock,i;
sock = socket(AF_INET,SOCK_DGRAM,0);

bindaddress.sin_port = htons(8080);
bindaddress.sin_family = AF_INET;
inet_pton(AF_INET,"127.0.0.1",&bindaddress.sin_addr);

bind(sock,(struct sockaddr *)&bindaddress,sizeof(bindaddress));

system("ls /root/NSLAB/ > file.txt");

FILE *file = fopen("/root/NSLAB/file.txt","r");

int counter = 0;
while(!feof(file))
{
	filedata[counter++] = fgetc(file);
}

filedata[counter-1] = '\0';

int addrsize = sizeof(oppaddress);
recvfrom(sock,recvdata,1024,0,(struct sockaddr*)&oppaddress,(socklen_t *)&addrsize);

printf("%s",filedata);


oppaddress.sin_port = htons(8070);
oppaddress.sin_family = AF_INET;

bzero(&oppaddress.sin_addr,sizeof(oppaddress.sin_addr));

sendto(sock,filedata,strlen(filedata),0,(struct sockaddr*)&oppaddress,sizeof(oppaddress));

int recvdatasize = recvfrom(sock,recvdata,1024,0,(struct sockaddr*)&oppaddress,(socklen_t *)&addrsize);

recvdata[recvdatasize] = '\0';


FILE *fp;


fp = fopen(recvdata,"r");

printf("%d\n",file);

while(!feof(fp))
{
	for(i=0;i<10 && !feof(fp);i++)
	{
		filedata[i] = fgetc(fp);
	}
	sendto(sock,filedata,i--,0,(struct sockaddr*)&oppaddress,sizeof(oppaddress));
}

}
