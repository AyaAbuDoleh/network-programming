/*  *****Server******
    Assignment 1
    Group #9  : Aiah AbuDouleh 
		Maysam Qasem 
*/
//Libraries :
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <stdint.h>
#include <inttypes.h>
#include <ctype.h>

#define MAXLINE 80
#define LISTENQ 1024
#define SA struct sockaddr
//sending the summary
struct msgsnt {

	char buff[MAXLINE];
}; 
//recieve the file
struct msgrcv {

	char buff[MAXLINE];
};


//*Variable decleration*//
int listenfd, connfd , bindfd ,rd , wt , len ,pr , peerport,sz;
char *clntip;//client ip
struct sockaddr_in  servaddr;
struct sockaddr_in clientaddr;
struct msgsnt msgsen;
struct msgrcv msgrcv;
int count[300];
char buff[MAXLINE];
long long int total=0; //summation of the printable characters

extern void exit (int code); // to use the exit() function

int main(int argc, char * argv[])
{
	//check for the number of the arguments
	if(argc != 2){	
		perror ("Invalid number of arguments : ");
		exit(1);
	}

	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0 ) {
		perror("Can't listen: ");
		exit(1);
		}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0 ){
		perror("Bind error: ");
		exit(1);}

	if(listen(listenfd, LISTENQ) < 0 ) {
		perror("Listen queue: ");
		exit(1);
		}

	for (; ;)
	{


		//**Wait for Client**//	
		puts ("waiting for client messages.");
		
		//** information about the client**//
		int len = sizeof(clientaddr);
		
		connfd = accept(listenfd,(struct sockaddr*) &clientaddr,&len);
		if(connfd<0) perror("Socket Error\n");	

		int pr = getpeername(connfd,(SA *) &clientaddr,&len);
		char * clntip = inet_ntoa(clientaddr.sin_addr);
		int peerport = ntohs(clientaddr.sin_port);
		
		
		//**Recieve file from the client**//
		total=0;
		do {
			rd=read(connfd,&msgrcv,sizeof(struct msgrcv));
			if (rd < 0) {perror("Recv file from Client : "); break;} 
			sz=strlen(msgrcv.buff);

			
			for (int i=0 ; i<sz ; ++i){
			//non printable characters
			if (msgrcv.buff[i]=='\n' || msgrcv.buff[i]=='\r' || msgrcv.buff[i]=='\t' || msgrcv.buff[i]==' ') continue; 			
			count[msgrcv.buff[i]]++;
			total++;			
			}
		
			if (sz < MAXLINE) break;
			
		   }while (rd > 0);
	
		printf("Received file from Client ( ip :: %s ) connected at ( port# :: %d ) \n" , clntip , peerport);
		
		sprintf(buff,"Total number of printable characters =%lld\n",total);
		strcpy(msgsen.buff,buff);
		wt=write(connfd,&msgsen,sizeof(struct msgsnt));
		if (wt < 0) {perror("Send res to server : "); close(connfd); continue;} 

		strcpy(msgsen.buff,"Char\tCount");
		wt=write(connfd,&msgsen,sizeof(struct msgsnt));
		if (wt < 0) {perror("Send res to server : "); close(connfd); continue;} 
		
		printf("Sending Summary to client ..\n");
		printf("*****************************\n");
		//**Send Analytics**//
		for (int i=0 ; i<300 ; ++i)	
		{
		if (count[i] == 0 || (char)(i) ==' ' || (char)(i) =='\n' || (char)(i)=='\t' || (char)(i)=='\t') continue;
		sprintf(buff,"%c\t%d",(char)(i),count[i]);
		
		strcpy(msgsen.buff,buff);
		wt=write(connfd,&msgsen,sizeof(struct msgsnt));
		if (wt == 0) {perror("Send res to server : "); break;} 
			}	
		
		bzero(&count,sizeof(count));
		close(connfd);
	}
return 0 ;
}
