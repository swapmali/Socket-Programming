#include<stdio.h>
#include<stdlib.h>
#include<string.h>

int main()
{
	FILE *fp;
	fp = fopen("/root/NSLAB/dns.txt","r");
	int counter=0;
	char buffer[50];
	char command[50] = "nslookup ";	
	while(!feof(fp))
	{
		char c;
		while((c=fgetc(fp))!='\n' && !feof(fp))
			buffer[counter++] = c;
		buffer[counter] = '\0';
		counter = 0;
		strcat(command,buffer);
		system(command);
		strcpy(command,"nslookup ");
	}
}
