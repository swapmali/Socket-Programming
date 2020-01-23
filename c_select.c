// Not guarented to work

#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/types.h>
#include<unistd.h>
#include<strings.h>
#define MAX(a,b) a>b?a:b

int main()
{
	struct sockaddr_in TCP_address,UDP_address;
	int TCP_Socket,UDP_Socket,i;
	fd_set rset;
	TCP_Socket = socket(AF_INET,SOCK_STREAM,0);
	UDP_Socket = socket(AF_INET,SOCK_DGRAM,0);
	char data[50];


	TCP_address.sin_family = AF_INET;
	TCP_address.sin_port = htons(8080);

	UDP_address.sin_family = AF_INET;
	UDP_address.sin_port = htons(8070);

	bzero(&TCP_address.sin_addr,sizeof(TCP_address.sin_addr));

	bzero(&UDP_address.sin_addr,sizeof(UDP_address.sin_addr));

	inet_pton(AF_INET,"127.0.0.1",&TCP_address.sin_addr);
	inet_pton(AF_INET,"127.0.0.1",&UDP_address.sin_addr);

	bind(TCP_Socket,(struct sockaddr *)&TCP_address,sizeof(TCP_address));
	listen(TCP_Socket,10);

	bind(UDP_Socket,(struct sockaddr *)&TCP_address,sizeof(TCP_address));

	FD_ZERO(&rset);

	int maxval =  MAX(TCP_Socket,UDP_Socket) + 1;



	for(;;)
	{
		FD_SET(TCP_Socket,&rset);
		FD_SET(UDP_Socket,&rset);		
		int number = select(maxval,&rset,NULL,NULL,NULL);
		printf("Hello\n");
		if(FD_ISSET(TCP_Socket,&rset))
		{
			printf("Got Connection from TCP\n");
			int socklen = sizeof(TCP_address);
			int newsock = accept(TCP_Socket,(struct sockaddr *)&TCP_address,(socklen_t *)&socklen);
			if(fork()==0)
			{
				int dataReceived = recv(newsock,data,50,0);			
				for(i=0;i<dataReceived;i++)
				{
					printf("%c",data[i]);
					data[i] = data[i] - 32;
				}
				send(newsock,data,dataReceived,0);
			}
			close(newsock);
			
		}
		if(FD_ISSET(UDP_Socket,&rset))
		{

			printf("Got Connection from UDP");
			int socklen = sizeof(TCP_address);
			int dataReceived = recvfrom(UDP_Socket,data,1024,0,(struct sockaddr *)&TCP_address,(socklen_t *)&socklen);
			for(i=0;i<dataReceived;i++)
				data[i] = data[i] - 32;
			int sizeUDPAddr = sizeof(UDP_address);
			sendto(UDP_Socket,data,dataReceived,0,(struct sockaddr *)&UDP_address,sizeUDPAddr);
		}

	}
}
