//Dependecies
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <wiringPi.h>

//Defines
#define PORTNO 8080
#define CALLS 32 //Max number of possible calls at one time

//Funk Protos
void printCalls(char Call[][CALLS]);
void error(const char *msg);
void addElement(char *Server, char Call[][CALLS]);

//Main (St Frag)
int main(void)
{
  	int sockfd, newsockfd, clien, n, recvlen;
  	char buffer[512];
	char call[512][32], temp[512];
  	struct sockaddr_in serv_addr;
	struct sockaddr_in cli_addr;
	socklen_t clilen =  sizeof(cli_addr);

	wiringPiSetup();

	printf("Opening Socket\n");
  	sockfd = socket(AF_INET, SOCK_DGRAM,0);
	if(sockfd < 0)
		error("ERROR opening socket");

	printf("Socket Open\n");
	memset((char *)&serv_addr, 0, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	inet_aton("192.168.0.2", &serv_addr.sin_addr.s_addr);
	serv_addr.sin_port = htons(PORTNO);

	//Bind
	printf("Binding Socket\n");
	if(bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
		error("Error on binding");

	while(1)
	{
		printf("Waiting on port %d\n",PORTNO);
		recvlen = recvfrom(sockfd, buffer, 256, 0, (struct sockaddr *
			)&cli_addr,&clilen);

		//printf("Recived %d bytes\n", recvlen);

		buffer[recvlen] = 0;
		sprintf(temp, "%d", millis());
		strcat(buffer, ";");
		strcat(buffer, temp);

		//puts(buffer); //Debug Only
		addElement(buffer, call);
		//strcpy(buffer, "Hello Rupert?!");

		sendto(sockfd, buffer, strlen(buffer), 0,
			(struct sockaddr *)&cli_addr, clilen);

		//Dump array into html file
		printCalls(call);
	}

	close(newsockfd);
	close(sockfd);
	return 0;
}

void printCalls(char Call[][CALLS])
{
	int n=0;

	FILE *fp;
	fp = fopen("/var/www/ServerCall.html","w"); //a == append

	if(fp != NULL)
	{
		do{
			fputs(Call[n], fp);
			fprintf(fp,"<br>");
			n++;
		}while(n < 32);
		fprintf(fp,"*PageUP 2016*");
		fclose(fp);
	}
}


void error(const char *msg)
{
	printf("%s\n",msg);
}

void addElement(char *Server, char Call[][CALLS])
{
	int n;
	n = CALLS - 1;

	do
	{
		strcpy(Call[n], Call[n-1]);
		n--;

	}while(n > 0);

	strcpy(Call[0],Server);
}
