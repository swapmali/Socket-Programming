#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include<string.h>

int main()
{
struct sockaddr_in address;
FILE *f;
char data[1024];
char FileNameToSend[1024];

int serverfd,addrlen,newsocket,test;

serverfd = socket(AF_INET,SOCK_STREAM,0);

address.sin_family = AF_INET;
address.sin_addr.s_addr = INADDR_ANY;
address.sin_port = htons(8080);

bind(serverfd,(struct sockaddr *)&address,sizeof(address));

printf("I am listining\n");

listen(serverfd,100);

newsocket = accept(serverfd,(struct sockaddr *)&address,(socklen_t*)&addrlen);

printf("Got Connection\n");

system("ls /root/NSLAB/ > file.txt");

f=fopen("file.txt","r");

int counter=0;
while(!feof(f))
{
char c = fgetc(f);
data[counter++]=c;
}

fclose(f);

data[counter-1]='\0';

strcat(data,"\n\nPlease Enter the file to send\n");

send(newsocket,data,strlen(data),0);

int bytesread = read(newsocket,FileNameToSend,1024);

printf("Hello1\n");

char Filetosen[500] = "/root/NSLAB/";

FileNameToSend[bytesread-1] = '\0';

strcat(Filetosen,FileNameToSend);


printf("%s  %d\n\n",Filetosen,strlen(Filetosen));

f=fopen(Filetosen,"r");
counter=0;

while(!feof(f))
{
char c = fgetc(f);
send(newsocket,&c,1,0);
}

fclose(f);

shutdown(serverfd,SHUT_RDWR);

}

