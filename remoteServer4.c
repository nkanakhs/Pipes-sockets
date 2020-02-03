#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h> 
#include <errno.h> 
#include <netdb.h> 
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/resource.h>
#define MAX 100

/* port we're listening on */

void parentHandler(int *p,int PORT);
void childHandler(int *p);

int main(int argc, char *argv[])
{

if(argc != 3){
  printf("Not enough arguments");
  return 0;
}

int PORT = atoi(argv[1]);
int numChildren = atoi(argv[2]);

int p[2];
if ( pipe(p) == -1) { perror ( " pipe call " ); exit (1);}


pid_t childPid;
for(int i=0;i<numChildren; i++)
{
  childPid=fork();
  if (childPid==0)
  {
      //printf("Hello from child: %d", getpid());
    break;
  }
}
if (childPid==0)
{
  //printf("Hello from child: %d with buffer: \n", getpid());
  childHandler(p);
  //return;
}
else if(childPid>0)
{
  //printf("Hello from paps: %d\n", getpid());
  parentHandler(p,PORT);
  //return;
}
return 0;
}


void parentHandler(int *p, int PORT)
{
fd_set master;

fd_set read_fds;

struct sockaddr_in serveraddr;

struct sockaddr_in clientaddr;

int fdmax;

int listener;
int newfd;

char buf[100];
int nbytes;

int yes = 1;

int addrlen;
int i, j;

FD_ZERO(&master);
FD_ZERO(&read_fds);

if((listener = socket(AF_INET, SOCK_STREAM, 0)) == -1)
{
perror("Server-socket() error");
exit(1);
}
//printf("Server-socket() is OK...\n");

/*"address already in use" error message */
if(setsockopt(listener, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
{
perror("Server-setsockopt() error");
exit(1);
}
//printf("Server-setsockopt() is OK...\n");

/* bind */
serveraddr.sin_family = AF_INET;
serveraddr.sin_addr.s_addr = htonl(INADDR_ANY); //for any client
serveraddr.sin_port = htons(PORT);
memset(&(serveraddr.sin_zero), '\0', 8);

if(bind(listener, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) == -1)
{
    perror("Server-bind() error");
    exit(1);
}

//printf("Server-bind() is OK...\n");
/* listen */

if(listen(listener, 10) == -1)
{
     perror("Server-listen() error ");
     exit(1);
}

//printf("Server-listen() is OK...\n");
FD_SET(listener, &master);
/* keep track of the biggest file descriptor */

fdmax = listener; /* so far, it's this one*/
/* loop */
for(;;)
{
  /* copy it */
  read_fds = master;
  if(select(fdmax+1, &read_fds, NULL, NULL, NULL) == -1)
  {
      perror("Server-select() error ");
      exit(1);
  }
  //printf("Server-select() is OK...\n");
   
  /*run through the existing connections looking for data to be read*/
  for(i = 0; i <= fdmax; i++)
  {
      if(FD_ISSET(i, &read_fds))
      { /* we got one... */
      if(i == listener)
       {
           /* handle new connections */
          addrlen = sizeof(clientaddr);
          if((newfd = accept(listener, (struct sockaddr *)&clientaddr, &addrlen)) == -1)
          {
            perror("Server-accept() error");
          }
          else
          {
            //printf("Server-accept() is OK...\n");
            FD_SET(newfd, &master); /* add to master set */
            if(newfd > fdmax)
            { /* keep track of the maximum */
              fdmax = newfd;
            }
            printf("Program: New connection from %s on socket %d\n", inet_ntoa(clientaddr.sin_addr), newfd);
          }
        }
        else
        {
        /* handle data from a client */
        bzero(buf,100);
       // memset(buf, 0, 100);
        if((nbytes = recv(i, buf, sizeof(buf), 0)) <= 0)
        {
        /*got error or connection closed by client */
          if(nbytes == 0)
          /* connection closed */
          printf("Program: socket %d hung up\n",i);
          else
          perror("recv() error ");

        /* close it... */
          close(i);
          bzero(buf,100);
         // memset(buf, 0, 100);
        /* remove from master set */
          FD_CLR(i, &master);
        }
        else
        {
          //printf("\ndata received: %s ", buf);
          
          write(p[1],buf,strlen(buf));
        /* we got some data from a client*/ // /*
          //childHandler(p);
          wait(NULL);
          bzero(buf,100);
          //memset(buf, 0, 100);
          //
        }
        }
      }
  }
}
return;
}

int  getSubString(char *source, char *target,int from, int to)
{
  int length=0;
  int i=0,j=0;
  
  //get length
  while(source[i++]!='\0')
    length++;
  //printf("\n}}}}%d|||||||%d||||||%d\n",length,from,to);
  if(from<0 || from>length){
    printf("Invalid \'from\' index\n");
    return 1;
  }
  if(to>length){
    printf("Invalid \'to\' index\n");
    return 1;
  } 
  
  for(i=from,j=0;i<=to;i++,j++){
    target[j]=source[i];
  }
  
  //assign NULL at the end of string
  target[j]='\0'; 
  
  return 0; 
}

void childHandler(int *p){
  char inbuf[MAX];
  char path[10000];
  char* tmp;
  char* tmp2;

  bzero(inbuf, MAX);
  int rsize=0;

  struct rlimit myrlimit; //RLIMIT_AS:maximumsizeofprocesssvirtualmemoryinbytes 
  rsize = read(p[0],inbuf,MAX);

  if(rsize>0 && strlen(inbuf))
  {
   // printf ("\nfrom child: %d PAP %d %s",(int)getpid(),(int)getppid(),inbuf);
  }
// /*
  int delimiter=0;
  int seperator_s=0;
  int port;

  for (int j=0; j<strlen(inbuf);j++)
    {
        //printf("bhka!!");
      if(inbuf[j]== '~')
      {
        tmp=(char*)malloc(sizeof(char)*MAX);
        tmp2=(char*)malloc(sizeof(char)*MAX);
        strncpy(tmp, inbuf+seperator_s, delimiter-seperator_s);
        strncpy(tmp2, inbuf+j+1, strlen(inbuf));
        seperator_s=delimiter+1;
        //printf("seperated: %s",tmp);
      }
      delimiter++;
    }
  //printf("seperated: %s \n and port %s \n",tmp,tmp2);
    
  int sock; 
  int yes=1;
  char buf[256]; 
  struct hostent *rem; 
  char hostbuffer[256]; 
  char *IPbuffer; 
  struct hostent *host_entry; 
  int hostname; 
  
    // To retrieve hostname 
    hostname = gethostname(hostbuffer, sizeof(hostbuffer)); 
  
    // To retrieve host information 
    host_entry = gethostbyname(hostbuffer); 
  
    // To convert an Internet network 
    // address into ASCII string 
    IPbuffer = inet_ntoa(*((struct in_addr*) 
                           host_entry->h_addr_list[0])); 

    //printf("Host IP: %s", IPbuffer);
  
  //printf("Hostname: %s\n", hostbuffer);

  struct sockaddr_in server;
  rem = (struct hostent*)gethostbyname(IPbuffer);
  //struct sockaddr_in server , client; 
  unsigned int serverlen = sizeof(server); 
  //struct sockaddr *serverptr = (struct sockaddr *) &server; 
  //struct sockaddr *clientptr = (struct sockaddr *) &client; 

  if ((sock = socket(AF_INET , SOCK_DGRAM , 0)) < 0) 
  { 
    perror("socket"); 
    exit(1); 
  } 
   // if ((rem = (struct hostent*)gethostbyname(argv[1])) == NULL) 
      //{ 
       // herror("gethostbyname"); 
        //exit(1); 
     // }
  //printf("\n!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!port::%s %d",tmp2,atoi(tmp2));
  memset(&server,0,sizeof(server));
  server.sin_family = AF_INET; 
  memcpy(&server.sin_addr , rem->h_addr , rem->h_length); 
  //server.sin_addr.s_addr = htonl(INADDR_ANY);
  server.sin_port = htons(((uint16_t)atoi(tmp2)));
  //memset(&(server.sin_zero), '\0', 8);
  //printf(" PORTA : %d",server.sin_port );
  //printf("address: %d",server.sin_addr.s_addr);

  /*"address already in use" error message */
  if(setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
  {
    perror("Server-setsockopt() error");
    exit(1);
  }
  //printf("Server-setsockopt() is OK...\n");

 /* if (bind(sock , (struct sockaddr *)&server , sizeof(server)) < 0) 
  { 
    perror("bind");
    exit(1); 
  }  
  printf("Server-bind() is OK...\n");  */
  char* commandBuffer = (char*)malloc(sizeof(char)*500000);
  FILE * exe = popen(&tmp[0], "r");
  if ( exe == 0 ) {
    fprintf( stderr, "Could not execute\n" );
    exit(1);
  }
  //wait(NULL);
  int temp = sizeof(server);
  bzero(path,10000);
  while (fgets(path, MAX, exe) != NULL) 
  {
          strcat(commandBuffer, path);
  }
  int kati=0;
  char packet[512];
  int packet_count=0;
  //printf("SIZE OF BUFFER: %ld",strlen(commandBuffer));
   if(strlen(commandBuffer)>512){
      if(strlen(commandBuffer)%512 == 0){
        kati =0;
      }
      else{
        kati =1;
      }
      for(int x=0;x<strlen(commandBuffer)/512+kati;x++){
        if(packet_count + 512>strlen(commandBuffer)){
          getSubString(commandBuffer,packet,packet_count, packet_count + strlen(commandBuffer)%512-1);
        }
        else{
          getSubString(commandBuffer,packet,packet_count, packet_count + 512-1);
        }
        
        packet_count += 512;
        if (sendto(sock , packet, 512 , 0, (struct sockaddr *)&server , temp) < 0) 
        { 
          perror("sendto");
          exit(1);
        }
      }
  }
  else{
    if(packet_count + 512>strlen(commandBuffer)){
          getSubString(commandBuffer,packet,0, strlen(commandBuffer)%512-1);
        }
        else{
          getSubString(commandBuffer,packet,0, 512-1);
      }
      
      if (sendto(sock , packet, 512 , 0, (struct sockaddr *)&server , temp) < 0) 
      { 
        perror("sendto");
        exit(1);
      }
  }
  char* termination = "$$$***!!!";
  if (sendto(sock , termination, 512 , 0, (struct sockaddr *)&server , temp) < 0) 
  { 
      perror("sendto");
  }

  //usleep(100);
  
  close(sock);
  bzero(tmp, sizeof(tmp)); 
  //printf("%s\n", buf);   
  //
  //getrlimit(RLIMIT_NLIMITS,&myrlimit); 
  //printf("Maximumstacksize=%luandcurrent=%lu\n", myrlimit.rlim_max,myrlimit.rlim_cur);
  //*/
  //free(tmp);
  //free(tmp2);
  rsize=0;
  bzero(inbuf, MAX);
  exit(1);
}



  






