/*  *****Server******
    Assignment 3
    Group 9#  :  Aiah AbuDouleh
		 Maysam Qasem 
*/

//** Libraries : **//
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
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>
#include <stdbool.h>

#define MAXLINE 1024
#define LISTENQ 1024
#define SA struct sockaddr

//** Variable decleration **//
int listenfd, connfd , bindfd ,rd , wt , len ,pr , peerport,sz,sndbuf,rcvbuf;
char *clntip;//client ip
struct sockaddr_in  servaddr;
struct sockaddr_in clientaddr;
struct msgsnt msgsen;
struct msgrcv msgrcv;
char buff[MAXLINE];

pid_t pid;


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


//** Signal handler for Zombies **//
void sig_child(int signo)
{
	pid_t pid;  

		while(pid = waitpid(-1, NULL, WNOHANG))
		{printf("Child %d terminated.\n", pid);
		puts("****************************************************************\n"); 
		return ;}
}

//** Signal handler for termination using cntrl+/ **//
void sig_quit(int signo)
{
	printf("Server  %d terminated.\n", getpid());
       	exit(0);
}



//** Exit() function **//
extern void exit (int code); 

int main(int argc, char * argv[])
{
	//** Checks for the number of the arguments **//
	if(argc != 2){	
		perror ("Invalid number of arguments : ");
		exit(0);
	}

	
	listenfd = socket(AF_INET, SOCK_STREAM, 0);
	if (listenfd < 0 ) {
		perror("Can't listen: ");
		exit(0);
		}


	//** Reuse Address enable option**//
	int enable = 1;
	if (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) < 0)
    	perror("setsockopt(SO_REUSEADDR) failed : ");

	puts("----------------------------------------------------------------\n");
	printf("waiting for client’s request, WELCOME all \n");

	
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(atoi(argv[1]));

	if (bind(listenfd, (SA *) &servaddr, sizeof(servaddr)) < 0 ){
		perror("Bind error: ");
		exit(0);}

	if(listen(listenfd, LISTENQ) < 0 ) {
		perror("Listen queue: ");
		exit(0);
		}

	signal(SIGCHLD, sig_child);//for zombie state
	signal(SIGQUIT, sig_quit);//for termination
	
	
	while (1)
	{
		
		bzero(&msgrcv,sizeof(msgrcv));
		bzero(&msgsen,sizeof(msgsen));
		
		
		//** information about the client **//
		int len = sizeof(clientaddr);
		connfd = accept(listenfd, (struct sockaddr*) &clientaddr, &len); 
		
		//** restart the accept if error has occured **//
		if(errno == EINTR){	
			close(connfd);
			connfd = accept(listenfd,(struct sockaddr*) &clientaddr,&len);//blocked till a client recieved
			}

		if (connfd < 0)
			 {perror("Socket Accept Error :");
					 exit(0);}
					
		//** Creating Child**//
		pid = fork();
		if (pid<0){
			perror("Error in creating the child : ");
			exit(0);
		}

		else  if (pid == 0)
		{
			puts("----------------------------------------------------------------\n");
			puts("waiting for client’s request");
			signal(SIGCHLD, sig_child);//for zombile state
			signal(SIGQUIT, sig_quit);// catch ctrl+\

			
			//** Close Listen socket in child**//
			if (close(listenfd)<0)
			{
				perror("Error in closing listenfd :");
				exit(0); }


		int pr = getpeername(connfd, (SA *)&clientaddr, &len);
		char * clntip = inet_ntoa(clientaddr.sin_addr);
		int peerport = ntohs(clientaddr.sin_port);

		//*Process ID*//
		pid = getpid();

		printf("Received from prosess (id ::%d) .. and connected at (ip :: %s , port# :: %d ) \n",pid, clntip, peerport);
		puts("----------------------------------------------------------------\n");
				
		
		signal(SIGCHLD, sig_child);//for zombile state
		signal(SIGQUIT, sig_quit);// catch ctrl+\
		
		while (1) {	

			bzero(&msgrcv,sizeof(msgrcv));
			bzero(&msgsen,sizeof(msgsen));
		
			if (rd = read(connfd, &msgrcv, sizeof(struct msgrcv)) < 0) { 
				perror("Reading from client Error");}

			FILE *fp;
			bool ok=true; 
			int mark,credit;	
			fp = fopen("file.txt", "wb");//write on the file
			
			strcpy(buff,msgrcv.buff);
			fwrite(buff,sizeof(char),strlen(buff),fp);		
			
			fclose(fp);
			fp = fopen("file.txt", "r");//read from the file
 		
			
			//** When child not sending any data, skip processing**//	
			fseek(fp, 0, SEEK_END);
			if (ftell(fp) == 0) continue;
			rewind(fp);
			

			double total=0;
			int count=0;

			//** Calc Average **//  
			printf("Received a request with the following info:  ");
			while (fscanf(fp,"%d,",&mark) != EOF) 
			{
			if (fscanf(fp,"%d,",&credit) != EOF){
			printf(" %d , %d",mark,credit);
			if (mark <=0 || credit<=0 || mark>100) ok=false;
			total+=(mark*credit); count+=credit;}	
			else ok=false;
			}
			
			printf("\n");
			fclose(fp);
			
			msgsen.average=total/count;
			//error input values 
			if (ok == false) {
			printf("Values Error\n");
			msgsen.average=-1;
			}
			//send the GPA to the client
			printf("\nsend GPA = %.2f to the client\n",msgsen.average);
			puts("----------------------------------------------------------------\n");
			if (wt = write(connfd, &msgsen, sizeof(struct msgsnt)) < 0){
				perror("Sendeing the fail faild"); exit(1);
			}

			}
			
			}		
		
		

		else close(connfd);//in server


	}
return 0 ;

}
