////////////////////////////////////////////////////////////////
//File Name	: tue_4-3_2014722024.c				//
//Date		: 2019/6/7					//
//Os		: Ubuntu 16.05 LTS 64bits			//
//Author	: Kin jin young					//
//Student ID	: 201472204					//
//--------------------------------------------------------------//
//Title : System Programming Assignment #4-3(semaphore) //
//////////////////////////////////////////////////////////////////
#include <stdio.h>
#include <stdlib.h>
#include<time.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include"srv_request.c"
#define BUFFSIZE 1024
#define PORTNO 40206
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define KEY_NUM 40206
#define MEM_SIZE 1024



static int maxNchildren; //max # of child process, startServer
static pid_t *pids; //keep child's pid;
static char *buf;

static int MaxChilds,MaxIdleNum,StartServers,MaxHistory;
int MinIdleNum;

static int pidarr[100];
static int pidcount = 0;
static int thr;
int tempc = 1;
int idlestate = 1;
char connecthistory[5000]; // set static variable.

pid_t child_make(int argc, char **argv,int i, int socketfd, int addrlen); // child make history

void sigint_handler1( int signo) // set alarm.
{

	int shm_id;
	void *addr;
	 if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // connect shared memory.
        printf("fail\n");
        return;
  	  }
  	  if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // attach shared memory
   	 {
        printf("shmat fail\n");
        return;
   	 }

	pthread_t tpid; // set thread pid.


	for(int i=0; i<5; i++) // signal sending
	{
	kill(pidarr[i],SIGUSR1);
	}
	alarm(10); 
	char *divide;
	char threadhistory[100][500];
	char originalstring[100][500];
	int threadcount = 0;

	char tempthread[5000];
	strcpy(tempthread,(char*)addr); // original string
	strtok(tempthread,"\n");
	while((divide = strtok(NULL,"\n")) != NULL)
	{
	strcpy(originalstring[threadcount],divide); // copy original
	divide = divide+24;// to get time
	strncpy(threadhistory[threadcount++],divide,24); // get time
	}
	for(int q=0; q<threadcount; q++)
	for(int p=q; p<threadcount; p++)
	if(strcmp(threadhistory[p],threadhistory[q]) > 0) // sort history(ascending order)
	{
		char tempth[500];
		strcpy(tempth,threadhistory[p]);
		strcpy(threadhistory[p],threadhistory[q]);
		strcpy(threadhistory[q],tempth);
		memset(tempth,0,500);
		strcpy(tempth,originalstring[p]);
		strcpy(originalstring[p],originalstring[q]);
		strcpy(originalstring[q],tempth);
	}
	int threadno = 1;
	printf("=======Connection History=======\n"); // print history
	printf("No\tIP\t\tPID\tPORT\tTIME\n");
	if(threadcount >= 20) // if thread over 10?? remove except idle state.
	{
		memset((char*)addr+48,0,sizeof((char*)addr));	
	}
	for(int q=0; originalstring[q][0] > 0;q++) // print history until originalstring NULL
	{

		int checksame =1;
		for(int l=q+1; originalstring[l][0] > 0; l++) // if there is same word??
			if(!strcmp(originalstring[q]+1,originalstring[l]+1) && (originalstring[q]+1) != NULL)
			{
				checksame--;
				break;
			}
		if(checksame == 1 && (originalstring[q][0]) >0) // if there is not same word?? print history.
		{
		printf("%d %s \n",tempc++,originalstring[q]+1);
		if(threadcount >= 20) // attach history.
		{
			strcat((char*)addr,"\n");
			strcat((char*)addr,originalstring[q]);
		}
		}
		if(tempc == MaxHistory+1)
			break;

	}
	//printf("%s %s\n",(char*)addr+96,(char*)ad(dr+96+24);
	tempc = 1; // initialize.
	 if ( -1 == shmdt( addr)) // shared memory detatch.
      {
         printf( "fail to detatch shared memory.\n");
         return;
      }
	//memset((char*)addr+48,0,sizeof((char*)addr+48));
}


void *check(void *vptr); // check function

void *idleincrease(void *vptr) // idle increase.
{

	int shm_id;
	void *addr;
	 if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // set shared memory
        printf("fail\n");
        return NULL;
  	  }
  	  if((addr = shmat(shm_id,NULL,0)) == (void*)-1)  // shared memory attatch.
   	 {
        printf("shmat fail2\n");
        return NULL;
   	 }


	time_t t;
	time(&t);
	char time[50];
	memset(time,0,50);
	strncpy(time,ctime(&t),24); // set time
	char tempaddr[50];
		pthread_mutex_lock(&counter_mutex); // set mutex lock 
	if(idlestate == 1) // if first information??
	{ 
	sprintf((char *)addr,"[%s] idleProcessCount : %d\n", time,idlestate++);  // set idle state
	((char*)addr)[1000] = 49;

	}
	else  // else??
	{
		(int)((char*)addr)[46]++; // just add.
		strncpy((char*)addr+1,time,24); // set timer
		(int)((char*)addr)[1000]++;
	}
	strncpy(tempaddr,(char*)addr,46); // set idle state information
	int count = (int)((char*)addr)[46] - 48; // set count
	printf("%s%d\n",tempaddr,count); // print idlestate and count
	sprintf(tempaddr,"%s%d\n",tempaddr,count);
	pthread_t tpid;
	pthread_create(&tpid,NULL,&logfile,(void *)&tempaddr); // thread call, make logfile
	pthread_join(tpid,NULL);
	pthread_mutex_unlock(&counter_mutex); // mutex unlock.

	 if ( -1 == shmdt( addr)) // shared memory detatch.
      {
         printf( "shared memory detatch failed.\n");
         return NULL;
      }
	return NULL; // return.
}
static char *tempargv[50];
static int tempargc;
static int tempt;
static int tempsocket_fd;
static int tempaddrlen; // set static information
void *idledecrease(void *vptr)
{
	char histemp[48];
	int shm_id;
	void *addr;
	 if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // set shared memory
        printf("fail\n");
        return NULL;
  	  }
  	  if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // shared memory attatch.
   	 {
        printf("shmat fail1\n");
        return NULL;
   	 }
	


	time_t t;
	time(&t);
	char time[50];
	memset(time,0,50);
	strncpy(time,ctime(&t),24); // set time
		pthread_mutex_lock(&counter_mutex); // mutex lock
	(int)((char*)addr)[46]--;
	(int)((char*)addr)[1000]--; // minus idle state
	strncpy((char*)addr+1,time,24);
	strncpy(histemp,((char*)addr),46); // stringcopy address
	int count = (int)((char*)addr)[46] - 48; // set count.
	printf("%s%d\n",histemp,count); // print idle state

	char templog[500];
	pthread_t tpid;
	sprintf(templog,"%s%d\n",histemp,count);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);

	pthread_mutex_unlock(&counter_mutex);  // mutex unlock
	 if ( -1 == shmdt( addr)) // detatch shared memory.
      {
         printf( "failed shared memory detatch.\n");
         return NULL;
      }
	return NULL;
}

void *check(void *vptr)// check function. idlestate fork or terminated.
{
	int shm_id;
	void *addr;
	pthread_t tpid;
	 if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // shared memory set.
        printf("fail\n");
        return NULL;
  	  }
  	  if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // shared memory attatch.
   	 {
     	  	 printf("shmat fail\n");
   	     return NULL;
   	 }
	if(thr >= MaxChilds) // thr is over MaxChild?? ignore.
	{
		
	}
	else if((((char*)addr)[46] < MinIdleNum + 48)) // if idlestate under the MinIdlestate?? 
	{	

		while(((char*)addr)[46] < 48+StartServers) // fork until idlestate is same as StartServers
		{
		pids[thr] = child_make(tempargc, tempargv ,tempt, tempsocket_fd, tempaddrlen); // child make.
		pidarr[thr] = pids[thr]; // store pid.
		(int)((char*)addr)[1000]++;
		pthread_create(&tpid,NULL,&idleincrease,NULL);
		pthread_join(tpid,NULL);// idle increase set.
		thr++;
		}

	}
	if(((char*)addr)[46] >= 48+MaxIdleNum && ((char*)addr)[46] <= MaxChilds + 48) // if idle state is higher than MaxHdlenum or MaxChilds??
	{
	time_t t;
	time(&t);
	char time1[50];
	memset(time1,0,50);
	strncpy(time1,ctime(&t),24);

	srand(time(NULL));
	

	while(((char*)addr)[46] > StartServers+48) // terminate until idle state is same as StartServers.
	{
	int random = rand()%(MaxIdleNum-StartServers) + 1; // set random number.
	printf("[%s] %d process is terminated.\n",time1,pidarr[thr - MinIdleNum]);	 // random process terminate

	char templog[500];
	sprintf(templog,"[%s] %d process is terminated.\n",time1,pidarr[thr - MinIdleNum]);// thread call, make logfile
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);
	pthread_join(tpid,NULL);


	kill(pidarr[thr - MinIdleNum],SIGKILL); // kill process
	pidarr[thr - MinIdleNum] = 0; // set pid t o0.
	for(int e = thr - MinIdleNum; e < thr; e++) // arrange alive state process.
		pidarr[e] = pidarr[e+1];
	--thr;
	pthread_create(&tpid,NULL,&idledecrease,NULL); // print idlestate.
	pthread_join(tpid,NULL);
	}
	}

	 if ( -1 == shmdt( addr)) // shared memory detatch.
      {
         printf( "fail to detatch shared memory.\n");
         return NULL;
      }

	return NULL;
}
static int thcount = 0;
void stop_handler(int sig) // set stop handler.
{
	time_t t;
	time(&t);
	char time[50];
	memset(time,0,50);
	strncpy(time,ctime(&t),24);
	pthread_t tpid;
	int status;
	pid_t pid;

	int shm_id;
	void *addr; // set variable.
	 if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // shared memory set.
        printf("fail\n");
        return;
  	  }
  	  if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // shared memory attatch.
   	 {
        printf("shmat fail1\n");
        return;
   	 }
	int thcount = (int)((char*)addr)[1000]-48;
	if(pid != 0 && pid != -1) // if it is parent process??
	{
	printf("\n");

	char templog[500];
	sprintf(templog,"\n");
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);
	}
	 if ( -1 == shmdt( addr)) // if shared memorty detatch failed??
      {
         printf( "fail to detatch shared memory.\n");
         return;
      }

	for(int i=4; i>=0; i--) // terminate process.
	{
	if(pidarr[i] != 0)
	{

	pid = waitpid(pidarr[i],&status,0); // get pid process.
	pidarr[i] = 0;
	if(pid != -1)
	{
	printf("[%s] %d process is terminated\n",time, pid); // print process and idlestate
	printf("[%s] idleProcessCount : %d\n",time,i);

	char templog[500];
	sprintf(templog,"[%s] %d process is terminated\n",time, pid);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);

	sprintf(templog,"[%s] idleProcessCount : %d\n",time,i);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);
	}
	}
	if(i == 0) // if first loop?? remove shared memory.
		shmctl(shm_id, IPC_RMID, NULL);

	}
	if(pid != 0 && pid != -1) // print server terminated.
	{
	printf("[%s] Server is terminated\n",time);
	char templog[500];
	sprintf(templog,"[%s] Server is terminated\n",time);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);
	}

	
	exit(0);
}

int main(int argc, char *argv[]){

	FILE *httpd = fopen("httpd.conf","r"); // open httpd file
	if(httpd == NULL) // file open fail/./.
	{
		printf("file open fail. end program\n");
	return 0;
	}
	char httpdstring[500]; // set variable
	char *resultstring;
	int resultcount=0;
	while(!feof(httpd))
	{
		resultstring = fgets(httpdstring,sizeof(httpdstring),httpd); // get file information.
		if(resultstring == NULL)
			break;
		strtok(httpdstring," "); // tokenized.
		resultstring = strtok(NULL,"\n");
		if(resultcount == 0) // first is MaxChilds.
			MaxChilds = atoi(resultstring);
		if(resultcount == 1) // Second is MaxIdleNum.
			MaxIdleNum = atoi(resultstring);
		if(resultcount == 2) // Third is MinIdleNum.
			MinIdleNum = atoi(resultstring);
		if(resultcount == 3) // Fourth is StartServers,
			StartServers = atoi(resultstring);
		if(resultcount == 4) // Fifth is MaxHistory.
			MaxHistory = atoi(resultstring);
		resultcount++;
	}

	fclose(httpd);	// clise file
	time_t t;
	time(&t);
	pthread_t tpid;
	memset(pidarr,0,100 * sizeof(int));
	struct sockaddr_in server_addr, client_addr; // set variable.

	int socket_fd, addrlen,  opt=1,client_fd;
	buf = (char *) malloc(BUFFSIZE+1);

	if((socket_fd = socket(PF_INET, SOCK_STREAM, 0)) < 0){
		printf("Server: Can't open stream socket.\n"); // socket open.
		return 0;
	}

	setsockopt(socket_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
	bzero((char *)&server_addr, sizeof(server_addr));
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	server_addr.sin_port = htons(PORTNO); // get bind infornation.

	if(bind(socket_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0){ // binf process.
		printf("Server: Can't bind local address.\n");
		return 0;
	}

	listen(socket_fd, 5); // listen.

	maxNchildren = StartServers;//maximum pre forked- child process
	pids = (pid_t *)malloc(100 * sizeof(pid_t));
	addrlen = sizeof(client_addr);



	char tempchar[50];
	memset(tempchar,0,50);
	strncpy(tempchar,ctime(&t),24);
	printf("[%s] Server is started.\n",tempchar);	 // set time and server start.
		char templog[500];
	sprintf(templog,"[%s] Server is started.\n",tempchar);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);

	///////////Pre forking routine


	for(thr=0; thr < StartServers ; thr++)
	{
		pids[thr] = child_make(argc,argv,thr, socket_fd, addrlen);//parent retruns
		pidarr[thr] = pids[thr];
		pthread_create(&tpid,NULL,&idleincrease,NULL); // idle increase and enroll pid.
		pthread_join(tpid,NULL);
	}
	signal(SIGALRM, sigint_handler1);
	signal(SIGINT,stop_handler); // set signal function.
	alarm(10);

	tempargc = argc;
	tempargv[0] = argv[0];
	tempt = t;
	tempsocket_fd = socket_fd;
	tempaddrlen = addrlen; // set information.


	for( ; ; ) // infinite loop to check idle state change.
	{

	pthread_create(&tpid,NULL,&check,NULL);
	pthread_join(tpid,NULL);
	sleep(0.5);
	}
		pause();//stop


}



pid_t child_make(int argc, char ** argv,int i, int socketfd, int addrlen) {
	pid_t pid;
	time_t t;
	time(&t);
	char temp[50];
	memset(temp,0,50);
	strncpy(temp,ctime(&t),24); // get time

	if((pid = fork()) > 0) // if parent??
	{
	char templog[500];
	pthread_t tpid;
	sprintf(templog,"[%s] %d process is forked.\n",temp,pid);// thread call, make logfile
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);
	pthread_join(tpid,NULL);
	printf("[%s] %d process is forked.\n",temp,pid);
		return(pid);
	}
	req_server(argc,argv,i,socketfd,addrlen); // if child??

}



