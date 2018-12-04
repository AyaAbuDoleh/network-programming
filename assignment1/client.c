/*  *****Client******
    Assignment 1
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

#define MAXLINE 80
#define SA struct sockaddr

extern void exit (int code); // to use the exit() function
//sending the file
struct msgsnt {

	char buff[MAXLINE];
}; 
//reciving the summary
struct msgrcv {

	char buff[MAXLINE];
};

//*Variable decleration*//
int sockfd , connfd , rd , wt ,sz;
struct sockaddr_in servaddr;
struct msgsnt msgsen;
struct msgrcv msgrcv;
char buff[MAXLINE];


int main(int argc , char * argv[]){
	//check of number of arguments
	if(argc != 3){ 
	perror("Invalid arguments: ");
	exit(1);}


	sockfd = socket(AF_INET , SOCK_STREAM , 0);//create the socket
	if (sockfd < 0) {perror("Socket error");
			exit(1);}
	//clear
	memset(&servaddr , 0 , sizeof(servaddr));
	memset(&msgsen , 0 , sizeof(msgsen));
	memset(&msgrcv , 0 , sizeof(msgrcv));

	servaddr.sin_family = AF_INET ;//ipv4
	servaddr.sin_port = htons(atoi(argv[2]));//port
	//check the ip
	if(inet_pton(AF_INET , argv[1] , &servaddr.sin_addr) <= 0) { 
		 perror("inet_pton error < enter a correct format of ip address > : ");
		 exit(1);}
	//connecting 
	connfd = connect(sockfd , (SA*) &servaddr , sizeof(servaddr));
	if(connfd < 0) { 
	perror("Connect error: ");
		exit(1);}

	//**Open a file**//
	char file[MAXLINE];
	FILE *fp;
	printf("please enter file name:\n");	
	scanf("%s",file);
	fp=fopen(file,"r");
	if (fp == NULL) 
	{
		perror("Can't open the file: ");
		
	do
	{
		puts("Want to leave insert -1, else insert file name again, ");//try again
		scanf("%s",file);
		if (strlen(file)>=2 && (file[0]=='-' && file[1]=='1')) exit(1);//-1
		fp=fopen(file,"r");			
	} while(fp == NULL);
	}//end if

	//*Reading and Sending File content*//
		
		while (fgets(buff,MAXLINE,fp) != NULL)
		{
		strcpy(msgsen.buff,buff);
		
		if (wt=write(sockfd,&msgsen,sizeof(struct msgsnt)) <0)
		{perror("Sendeing the fail faild"); exit(1);}

		} 
	//Sending the file completed
		sz=strlen(buff);
		printf("file %s has been sent to the server\n",file);
		if (sz==0 ){ puts("empty file "); exit(1);}	
	//**Recieve analytics from the server**//
		rd=read(sockfd,&msgrcv,sizeof(struct msgrcv));
		
		if (rd ==0){ printf("empty file"); exit(1);}
	 while (rd > 0)
		{
		printf("%s\n",msgrcv.buff);
		rd=read(sockfd,&msgrcv,sizeof(struct msgrcv));	
	 	}		
		
	
		close(sockfd);	
return 0;
}
