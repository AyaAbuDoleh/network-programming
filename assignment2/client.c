/*  *****Client******
    Assignment 3
    Group 9#  : Aiah AbuDouleh
		Maysam Qasem 
*/
//Libraries
#include <stdio.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <strings.h>
#include <stdint.h>
#include <inttypes.h>
#include <signal.h>
#include <time.h>
#include <sys/select.h>
#include <stdbool.h>
#include <unistd.h> 

#define MAXLINE 1024
#define SA struct sockaddr
#define STDIN 0

//** Variable decleration **//
int sockfd , connfd , rd , wt ,sz;
struct sockaddr_in servaddr;
struct msgsnt msgsen;
struct msgrcv msgrcv;
char buff[MAXLINE];
bool ok;


//**  To use the exit() function **//
extern void exit (int code); 
int max(int x, int y)
{
	if (x > y)
		return x;
	return y;
}
//** sending the file **//
struct msgsnt {
	char buff[MAXLINE];
	double average;
}; 

//** reciving the summary **//
struct msgrcv {
	char buff[MAXLINE];
	double average;
};


//** termination using cntrl+/ **//
void sig_quit(int signo)
{
	printf("\nClient terminated. Good BYE!\n");
	shutdown(sockfd,SHUT_RDWR);	
	exit(1);
	
}


int main(int argc , char * argv[]){
	
	//** check of number of arguments **//
	if(argc != 3){ 
	perror("Invalid arguments: ");
	exit(1);}
	
	//**Socket Creation**//
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) {
		perror("Socket error");
		exit(1);
		}

;

	//** Clear **//
	memset(&servaddr, 0, sizeof(servaddr));
	memset(&msgsen, 0, sizeof(msgsen));
	memset(&msgrcv, 0, sizeof(msgrcv));

	servaddr.sin_family = AF_INET;//ipv4
	servaddr.sin_port = htons(atoi(argv[2]));//port

	//** Check the ip **//
	if (inet_pton(AF_INET, argv[1], &servaddr.sin_addr) <= 0) {
		perror("inet_pton error < enter a correct format of ip address > : ");
		exit(1);
		}
		
	int  sendbuff,recvbuff;
 	socklen_t optlen;
 	int snd = 0 , rcv=0;

	//** Get send buffer size **//
 	optlen = sizeof(sendbuff);
 	snd = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);
	if(snd == -1) printf("Error getsockopt one\n");
 	else printf("Initial Send buf(TCP): %d Bytes\n", sendbuff);
		
	//** get rcv buffer size **//		
	rcv = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuff, &optlen);
	if(rcv == -1) printf("Error getsockopt one\n");
 	else printf("Initial Recive buf(TCP): %d Bytes\n", recvbuff);
		
	//** update and print new value**// 		
	printf("Please enter new values for the send and receive buffers: \n");
	scanf("%d%d",&sendbuff,&recvbuff);
	sendbuff=(sendbuff+1)/2; recvbuff=(recvbuff+1)/2; 

	snd = setsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, sizeof(sendbuff));
	if (snd == -1) printf("Error setsockopt one\n");
	snd = getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendbuff, &optlen);
	if (snd == -1)  printf("Error getsockopt new value\n");
	else printf("New SND_SOCKET value = %d\n",sendbuff);

		
	rcv = setsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuff, sizeof(recvbuff));
	if (rcv == -1) printf("Error setsockopt one\n");
	rcv = getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvbuff, &optlen);
	if (snd == -1)  printf("Error getsockopt new value\n");
	else printf("New RCV_SOCKET value = %d\n",recvbuff);
	
	
		
	//** connecting **//
	connfd = connect(sockfd, (SA*)&servaddr, sizeof(servaddr));
	if (connfd < 0) {
		perror("Connect error: ");
		exit(1);
		}

	signal(SIGQUIT, sig_quit);	
	fgets(buff,MAXLINE,stdin);

	//**Select Operation variables**//
	int mxfd;
	fd_set rset; 
	FD_ZERO(&rset);
	int n=1;

	puts("\nEnter grades and credit hours as following");
	puts("mark,credit,mark,credit,..... \n");

	while(n) { 

	memset(&msgsen, 0, sizeof(msgsen));
	memset(&msgrcv, 0, sizeof(msgrcv));
	
	signal(SIGQUIT, sig_quit);

	//**Select Operation**//
	FD_ZERO(&rset);
	FD_SET(STDIN,&rset);
	FD_SET(sockfd,&rset);
	mxfd=max(sockfd , STDIN) +1;
	ok=true; 

	if (select(mxfd, &rset, NULL, NULL, NULL) < 0) perror(" Select error : "), exit(0); 

	//** Sockfd FD ready for read **//	
	if (FD_ISSET(sockfd,&rset))
		{
		if (rd = read(sockfd, &msgrcv, sizeof(struct msgrcv)) ==0) { 
		puts("Server treminated!"); exit(1);}
		if (msgrcv.average == -1) printf("Error in values \n");
		else  printf("The GPA is  = %.2f\n",msgrcv.average);

		} 

	//** STDIN FD: input is ready**//

		if (FD_ISSET(STDIN, &rset)) {
		fgets(buff,MAXLINE,stdin);
			
		strcpy(msgsen.buff,buff);

		if ( wt = write(sockfd, &msgsen, sizeof(struct msgsnt)) < 0){
				perror("Sending faild"); exit(1);
		}
		}
		
		else {
		puts("\nEnter grades and credit hours as following");
		puts("mark,credit,mark,credit, ..... \n");
		}
		
	}



return 0;
}
