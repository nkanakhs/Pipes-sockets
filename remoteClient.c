/* i n e t _ s t r _ c l i e n t . c : Internet stream sockets client */
#include <stdio.h>
#include <sys/types.h>
#include <sys/resource.h>
#include <sys/socket.h> 
#include <sys/wait.h>
#include <errno.h> 
#include <arpa/inet.h>
#include <netinet/in.h>
/* internet sockets */
#include <unistd.h>
/* read , write , close */
#include <netdb.h>
/* g e t h o s t b y a d d r */
#include <stdlib.h>
/* exit */
#include <string.h>

#define MAX 100
#define MAX2 128

int main( int argc, char** argv){
	if(argc!=5){ return -1;}
	

	char* serverName = argv[1];
	struct hostent* rem;
	int serverPort = atoi(argv[2]);
	int clientPort = atoi(argv[3]);
  
    int count = 0;
   	
    pid_t childPid;
    childPid=fork();
  	if (childPid==0)
  	{
  		//usleep(10000);
  		int socket1;
  		char tmp[512]; 
  		char buffer[256]; 
  		struct hostent *rem1; 
    	char hostbuffer[256]; 
    	char *IPbuffer; 
    	struct hostent *host_entry; 
    	int hostname; 
    	int yes = 1;
  		
  		bzero(tmp,512);
    	// To retrieve hostname 
    	hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
  
    	// To retrieve host information 
    	host_entry = gethostbyname(hostbuffer); 
  
    	// To convert an Internet network 
    	// address into ASCII string 
    	IPbuffer = inet_ntoa(*((struct in_addr*)host_entry->h_addr_list[0])); 
   		
   		//printf("Host IP: %s", IPbuffer);
  
  		//printf("Hostname: %s\n", hostbuffer);
 		struct sockaddr_in server, client;
  		
  		rem1 = (struct hostent*)gethostbyname(argv[1]);
  		//struct sockaddr_in server , client;  
  		socklen_t serverlen;
  		//struct sockaddr *serverptr = (struct sockaddr *) &server; 
  		//struct sockaddr *clientptr = (struct sockaddr *) &client; 

  		//wait(NULL);

  		if ((socket1 = socket(AF_INET , SOCK_DGRAM , 0)) < 0) 
  		{ 
    		perror("socket"); 
   			exit(1); 
  		} 

		/*"address already in use" error message */
		if(setsockopt(socket1, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
		{
			perror("Server-setsockopt() error lol!");
			exit(1);
		}
		//printf("Server-setsockopt() is OK...\n");


 	 	//memset(&server,0,sizeof(server));
  		server.sin_family = AF_INET;  

    	//server.sin_addr.s_addr = INADDR_ANY; 

  		//server.sin_addr.s_addr = serverName;
  		//memcpy(&server.sin_addr.s_addr , rem1->h_addr , rem1->h_length); 

		server.sin_addr.s_addr = htonl(INADDR_ANY);
  		server.sin_port = htons(clientPort);
		//memset(&(server.sin_zero), '\0', 8);
		//printf("port: %d",server.sin_port);
		//printf("address: %d",server.sin_addr.s_addr);

  		if (bind(socket1 , (struct sockaddr *)&server , sizeof(server)) < 0) 
  		{ 
    		perror("bind");
    		exit(1); 
  		}  
  		//printf("Server-bind() is OK...\n");
  		serverlen = sizeof(server);
		char* commandBuffer = (char*)malloc(sizeof(char)*500000);

		FILE *fpt;
		int command_count = 0;
		FILE *fp;
		char c;
		char* command_count_text = (char*)malloc(sizeof(char)*100);
		command_count_text[0]='\0';
  		//wait(NULL);
		for(;;)
		{

			//printf("lalaLalalalla Swhile.\n");
  			if (recvfrom(socket1 , tmp, 512, MSG_WAITALL, (struct sockaddr *)&server , &serverlen) < 0) 
  			{ 
    			perror("recvfrom");
    			exit(1);
  			} 
  			if (strstr(tmp, "$$$***!!!") != NULL){
  				command_count += 1;
  			   strcat(command_count_text,"output.");
  			   c=command_count + '0';
  			   strncat(command_count_text, &c,1);
			   fp = fopen(command_count_text, "w+");
			   fputs(commandBuffer, fp);
			   fclose(fp);
			   free(command_count_text);
			   command_count_text = (char*)malloc(sizeof(char)*100);
			   command_count_text[0]='\0';
			   free(commandBuffer);
			   commandBuffer = (char*)malloc(sizeof(char)*500000);
			   commandBuffer[0]='\0';
  			}
  			else{
  				strcat(commandBuffer, tmp);
  			}
  			

  			//printf(" lalalallalalalalalal length :: %d ",serverlen);
  			//printf("%s",tmp);
 		
 			//bzero(tmp,512);
  		}

  		bzero(tmp, sizeof(tmp)); 
  	} /* END OF CHILD PROCESS */
  	else
  	{
	   	long n=clientPort;

	    while (n != 0) {
	        n /= 10;     // n = n/10
	        ++count;
	    }
	    //printf("Number of digits: %d", count);

		FILE *fptr;
		int sock;
		char* file=argv[4];
		sock = socket ( AF_INET , SOCK_STREAM , 0) ;
		if(sock == -1)
		{
				perror ( " Socket creation failed ! " ) ;
				return 0;
		}
		struct sockaddr_in serv_addr;
		struct in_addr myaddress;
		/* 
		IPv4 dot -number into binary form (network byte order) 
		inet_aton(argv[1], &myaddress);
		rem=gethostbyaddr((const char*)&myaddress , sizeof(myaddress), AF_INET);
		*/
		//printf("Input is: %s", serverName);
		rem = (struct hostent*)gethostbyname(serverName);
		serv_addr.sin_family = AF_INET;
		memcpy (&serv_addr.sin_addr,rem->h_addr,rem->h_length ) ; // copy from rem to sin.addr
	    serv_addr.sin_port = htons(serverPort);
	    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	    {
	        printf("\nConnection Failed \n");
	        return -1;
	    }
		//printf("Input is: %s", file);
		fptr=fopen(file,"r");
		char str[MAX];

		if (fptr==NULL){
			printf("Error opening file!");
			exit(1);
		}
		int cnt=0;
		char* tmp;
		char* tmp2;
		int seperator=0;
		int seperator_s=0;
		char bufh[MAX2];
		
		while (fgets(str,MAX,fptr)){
			//printf("\nOK! Lets see : %s %ld", str,strlen(str));
			if (cnt==10-1){
				cnt=0;
				//usleep(5000);
			}
			seperator=0;
			seperator_s=0;
			for(int i=0;i<strlen(str);i++){
				if(str[i]==';'){
					tmp=(char*)malloc(sizeof(char)*MAX2);
					tmp2=(char*)malloc(sizeof(char)*MAX2);
					//printf("bhka!!");
					strncpy(tmp, str+seperator_s, seperator-seperator_s);
					seperator_s=seperator+1;
					strcat(tmp, "~");
					sprintf(tmp2,"%d",clientPort);
					strcat(tmp, tmp2);
					//sprintf(string+strlen(string),"%d",number);
					//strcat(tmp, (char)clientPort);
					//int help = strlen(tmp) + count +1;
	      			//snprintf(bufh, help, tmp , clientPort); // puts string into buffer
	      			//printf("%s\n", bufh);
					strcat(tmp, "\n");
					//printf("\n !!!!!!!!!Lets see : %s", tmp);
					send(sock , tmp ,strlen(tmp) ,0);
					//usleep(1000);
					usleep(1000);
					cnt++;
					bzero(tmp,100);
					free(tmp);
				}
				seperator++;
			}
			tmp=(char*)malloc(sizeof(char)*MAX);
			tmp2=(char*)malloc(sizeof(char)*MAX2);
			strncpy(tmp, str+seperator_s, seperator-seperator_s-1);
			strcat(tmp, "~");
			sprintf(tmp2,"%d",clientPort);
			strcat(tmp, tmp2);
			//printf("\n !!!!!!!!!Lets see : %s %ld", tmp,strlen(tmp));
			send(sock ,tmp ,strlen(tmp) ,0);
			usleep(1000);
			bzero(tmp,100);
			free(tmp);
			cnt++;
			bzero(str,100);
			memset(str, 0, 100);
			//fflush(stdin);
		}
		bzero(str, MAX);
		close(sock);
	} // end of father process

	return 0;
	//fgets(str,MAX,fptr);
	//printf("\nOK! Lets see : %s", str);

}
