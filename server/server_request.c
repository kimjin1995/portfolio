////////////////////////////////////////////////////////////////
//File Name	: srv_request.c					//
//Date		: 2019/5/22					//
//Os		: Ubuntu 16.05 LTS 64bits			//
//Author	: Kin jin young					//
//Student ID	: 201472204					//
//--------------------------------------------------------------//
//Title : System Programming Assignment #3-3(fork Server and client) //
//////////////////////////////////////////////////////////////////

#define _GNU_SOURCE
#include<stdio.h>
#include<string.h>
#include<semaphore.h>
#include<sys/types.h>
#include<fnmatch.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<signal.h>
#include<time.h>
#include<wait.h>
#include"ls.c"
#include<pthread.h>
#include<sys/ipc.h>
#include<sys/shm.h>
#define KEY_NUM 40206
#define MEM_SIZE 1024

#define URL_LEN 256
#define BUFSIZE 4096
#define PORTNO 40206
static int taddress = 0;
static char addressstore[100][50] = {0,};
static int portstore[100] = {0};
static char porttime[100][50] = {0,};
static int portpid[100] = {0};

// static method to copy the port no, address, ip, time
pthread_mutex_t counter_mutex = PTHREAD_MUTEX_INITIALIZER;

void printaddress() // print address function, MaxHistory
{
	int shm_id;
	void *addr;
	if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // set shared memory
		printf("fail\n");
		return;
	}
	if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // shared memory attatch.
	{
		printf("shmat fail3\n");
		return;
	}
	time_t t;
	time(&t);
	char time[50];
	memset(time,0,50);
	strncpy(time,ctime(&t),24); // set time.




	int port = 0,portcount=0; 
	while(portstore[portcount] != 0) // count the number of login history
		portcount++;	


	for(int i=0; i<portcount; i++)
		for(int j=0; j<portcount; j++)
			if((strcmp(porttime[i],porttime[j])>0) || (strcmp(porttime[i],porttime[j])==0 && portpid[i] >portpid[j])) // ascending order history.
			{
				char temp[50];
				int k;
				strcpy(temp,porttime[i]);
				strcpy(porttime[i],porttime[j]);
				strcpy(porttime[j],temp);

				strcpy(temp,addressstore[i]);
				strcpy(addressstore[i],addressstore[j]);
				strcpy(addressstore[j],temp);

				k = portstore[i];
				portstore[i] = portstore[j];
				portstore[j] = k;

				k = portpid[i];
				portpid[i] = portpid[j];
				portpid[j] = k;
			}


	pthread_mutex_lock(&counter_mutex); // mutex lock
	while(portstore[port] != 0 ) // print history address, pid, port ,time
	{
		char hicode[500];

		sprintf(hicode,"%d\t%s\t%d\t%d\t%s",(port+1),addressstore[port],portpid[port],portstore[port],porttime[port]); // stringcopy history to shared memory.
		strcat(((char*)addr),"\n");
		strcat(((char*)addr),hicode);

		port++;
	}
	pthread_mutex_unlock(&counter_mutex); // mutex unlock.

}

void *addhistory(void *vptr) // addhistory function.
{
	printaddress();
	return NULL;
}
const char * logtextfile = "server_log.txt";
static int filestatic = 0;
void *logfile(void *vptr) // addhistory function.
{
	int fd;
	char tempfile[500];
	char cpyfile[500]; // set copystring
	memset(cpyfile,0,500);
	memset(tempfile,0,500);
	strcpy(tempfile,((char *)vptr)); // get string
	sem_t *mysem = sem_open("40206",O_CREAT | O_EXCL, 0700, 1); // file open
	sem_close(mysem);
	mysem = sem_open("40206",O_RDWR);// semaphore open
		sem_wait(mysem);
	if(filestatic == 0) // file open is first?? initialization
	{
	fd = open(logtextfile,O_CREAT | O_TRUNC | O_RDWR | O_APPEND, 0666);
	filestatic++;
	}
	else // file is not open firt?? append.
	fd = open(logtextfile,O_CREAT | O_RDWR | O_APPEND, 0666);
	for(int i=0; i<500; i++)
		if(tempfile[i] != '\0')
			cpyfile[i] = tempfile[i];

	lseek(fd,0,SEEK_END);	// move
	write(fd,cpyfile,strlen(cpyfile)); // write string
	close(fd);
	sem_post(mysem); // end semaphore
	sem_close(mysem);
	
}



void sigint_handler(int signo) // set alarm function.
{
	pthread_t tpid;
	pthread_create(&tpid,NULL,&addhistory,NULL); // call addhistory function.
	pthread_join(tpid,NULL);
}
void *check(void *vptr);
void *idledecrease(void *vptr); // get check and idlechange.
void *reagain(void *vptr) // regain increase idlestate.
{
	int shm_id;
	void *addr;
	if((shm_id = shmget((key_t)KEY_NUM,MEM_SIZE,IPC_CREAT |0666)) == -1){ // set shared memoryd.
		printf("fail\n");
		return NULL;
	}
	if((addr = shmat(shm_id,NULL,0)) == (void*)-1) // attach shared memory.
	{
		printf("shmat fail2\n");
		return NULL;
	}

	time_t t;
	time(&t);
	char time[50];
	memset(time,0,50);
	strncpy(time,ctime(&t),24); // set time.
	pthread_mutex_lock(&counter_mutex); // mutex lock.
	(int)((char*)addr)[46]++;
	char temphis[48];
	strncpy(temphis,(char*)addr,47); // idlestate increase
	printf("%s\n",temphis);

	char templog[500];
	pthread_t tpid;
	sprintf(templog,"%s\n",temphis);
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);


	pthread_mutex_unlock(&counter_mutex); // mutex unlick.
	if ( -1 == shmdt( addr)) // detatch shared memory.
	{
		printf( "fail to detatch shared memory.\n");
		return NULL;
	}
	return NULL;
}


int req_server(int argc, char **argv,int i, int socketfd, int addrlen){
	char buff[100],buff1[100];
	getcwd(buff,100);
	getcwd(buff1,100);

	struct sockaddr_in server_addr, client_addr;
	int client_fd;
	int len_out; // set variable.



	char access[50][50];


	char tempstr[1024];
	char *pStr2;
	int accessfile = 0;
	FILE* faccess = fopen( "accessible.usr", "r" ); // it is accepted ip???
	if( faccess != NULL )
	{
		while( !feof( faccess ) )
		{
			pStr2 = fgets( tempstr, sizeof(tempstr), faccess ); // read a line
			if(pStr2 == NULL) // if line is null??
				break;
			strncpy(access[accessfile++],pStr2,strlen(pStr2)-1); // string copy to compare

		}
		fclose( faccess );
	}

	signal( SIGUSR1, sigint_handler); // set SigUsr.
	//alarm(10);

	int count = 0;
	int Z = 0;
	pid_t pid[5];
	int status[5];

	while(1)
	{
		clock_t start,end;
		pthread_t tpid;
		optind = 0;
		struct in_addr inet_client_address;
		char buf[BUFSIZE] = {0,};
		char tmp[BUFSIZE] = {0,};
		char url[1024] = {0,};
		char response_header[BUFSIZE] = {0,};
		char response_message[BUFSIZE] = {0,};
		char response_body[BUFSIZE] = {0,};

		char method[20] = {0,};
		char *tok = NULL;
		memset(response_message,0,BUFSIZE); // set variable.

		client_fd = accept(socketfd, (struct sockaddr*)&client_addr, &addrlen); // client connection accept
		if(client_fd <0){
			printf("Server:accept failed\n");
			return 0;
		}
		inet_client_address.s_addr = client_addr.sin_addr.s_addr;

		read(client_fd,buf,BUFSIZE);
		strcpy(tmp,buf);
		tok = strtok(tmp," "); // read and url parsing
		if(tok == NULL) // url parsing is failed??
		{
			continue;
		}

		double clockresult;
		start = clock();

		time_t t;
		time(&t);
		strcpy(addressstore[taddress],inet_ntoa(inet_client_address)); // copy the information of client
		strcpy(porttime[taddress],ctime(&t));
		portstore[taddress]=client_addr.sin_port;
		portpid[taddress++]= getpid();

		char logfilestring[500];
		sprintf(logfilestring,"%s %s %d %d\n",addressstore[taddress-1],porttime[taddress-1],portstore[taddress-1],portpid[taddress-1]);
		pthread_create(&tpid,NULL,&addhistory,NULL);
		pthread_join(tpid,NULL);
		//pthread_create(&tpid,NULL,&logfile,(void *)&logfilestring);
		//pthread_join(tpid,NULL);

		int same = 0;
		for(int i=0; i<accessfile;i++)
		{
			if(!fnmatch(access[i],inet_ntoa(inet_client_address),0)) // there is accepted ip???
			{
				same++;
			}
		}
		char temptime[50];

		if(same == 0) // no?? access denied.
		{

			FILE*file = fopen("html_ls.html","wb"); // file open
			sprintf(response_message,"<h1>Access denied!</h1><h1>Your IP : %s</h1><h3>You have no permission to access this web server.</h3><h3>HTTP 403.6 - Forbidden: IP address reject</h3>",inet_ntoa(inet_client_address));
			fclose(file);
			sprintf(response_header,
					"HTTP/1.0 200 OK \r \n"
					"Server:2019 simple web server \r\n" 
					"Content-length:%lu\r\n"
					"Content-type:text/html\r\n\r\n",strlen(response_message));
			write(client_fd,response_header,strlen(response_header));
			write(client_fd,response_message,strlen(response_message));
			write(client_fd,response_body,strlen(response_body));
			//	printf("%s %d client was disconnected\n", inet_ntoa(inet_client_address),client_addr.sin_port);
			close(client_fd);
		memset(temptime,0,50);
		strncpy(temptime,ctime(&t),24);

		}
		else
		{
				memset(temptime,0,50);
		strncpy(temptime,ctime(&t),24);
		printf("==============New Client==============\n");

		char templog[500];
	pthread_t tpid;
	sprintf(templog,"==============New Client==============\n");// thread call, make logfile
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);
	pthread_join(tpid,NULL);
		//printf("%s %d client was connected\n", inet_ntoa(inet_client_address), client_addr.sin_port);
		//printf("Request from %s %d\n", inet_ntoa(inet_client_address), client_addr.sin_port);
		//puts(buf);



		strcpy(method, tok);
		if(strcmp(method,"GET") == 0)  // url parsing
		{
			tok = strtok(NULL, " ");
			strcpy(url,tok);
		}
		printf("TIME : [%s]\nURL : %s\nIP : %s \nPort : %d\nPID : %d\n\n",temptime,url,inet_ntoa(inet_client_address),client_addr.sin_port,getpid()); // new client connection

	sprintf(templog,"TIME : [%s]\nURL : %s\nIP : %s \nPort : %d\nPID : %d\n\n",temptime,url,inet_ntoa(inet_client_address),client_addr.sin_port,getpid());
	pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
	pthread_join(tpid,NULL);


		if(!strcmp(url+1,"favicon.ico"))
			strcpy(url,"/.");
		pthread_create(&tpid,NULL,&idledecrease,NULL);
		pthread_join(tpid,NULL);
		if(chdir(url+1) && strcmp(url,"/") && strcmp(url,buff)) // it is directory and not rootfolder???
		{

			struct stat sb;
			lstat(url+1,&sb);
			int size = sb.st_size;
			char buf1[500000]; 
			int A;
			char tempurl[1024];
			memset(tempurl,0,1024);
			memset(buf1,0,500000);



			FILE* rfile = fopen(url+1,"r"); // file open

			if(rfile == NULL) // if file is error
			{
				FILE*file = fopen("html_ls.html","wb"); // file open
				sprintf(response_message,"<h1>Not Found</h1></br><h3>The request url %s was not found on this server</h3><h3>HTTP 404-Not Page Found</h3>",url);
				fclose(file);
				sprintf(response_header,
						"HTTP/1.0 200 OK \r \n"
						"Server:2019 simple web server \r\n" 
						"Content-length:%lu\r\n"
						"Content-type:text/html\r\n\r\n",strlen(response_message));

				write(client_fd,response_header,strlen(response_header));
				write(client_fd,response_message,strlen(response_message));
				write(client_fd,response_body,strlen(response_body));
				//printf("%s %d client was disconnected\n", inet_ntoa(inet_client_address),client_addr.sin_port);
				close(client_fd); // send data to client

			}
			else
			{
				char strTemp[1024];
				char strsum[200000] = {0,};
				char *pStr;
				if( rfile != NULL ) // if file is open??
				{
					while( !feof( rfile ) )
					{
						pStr = fgets( strTemp, sizeof(strTemp), rfile );
						if(pStr != NULL)
							strcat(strsum,strTemp);	
					}
					if(strsum != NULL)
						sprintf(response_message,"%s",strsum); // get data and copy to message
					else
						sprintf(response_message,"<h1>Not Found</h1></br><h3>The request url %s was not found on this server</h3><h3>HTTP 404-Not Page Found</h3>",url); // there is no data
				}
				fclose(rfile);


				for(Z = 0; Z < strlen(strsum); Z++) // binary data seperate
				{
					if((strsum[Z] >>7) == 1) // top bit is 1??binary file
						break;
				}

				if(Z != strlen(strsum)-1)
					Z = 1;
				else
					Z = 0;


				// it is image??
				if(fnmatch("*.jpg",url+1,FNM_CASEFOLD)== 0 || fnmatch("*.png",url+1,FNM_CASEFOLD)== 0||fnmatch("*.jpeg",url+1,FNM_CASEFOLD)== 0)
				{
					sprintf(response_header,
							"HTTP/1.0 200 OK \r \n"
							"Server:2019 simple web server \r \n" 
							"Content-length:%d\r\n"
							"Content-Type:image/*\r\n\r\n",size);

				}
				else if(Z == 1) // it is binary data?
				{
					sprintf(response_header,
							"HTTP/1.0 200 OK \r \n"
							"Server:2019 simple web server \r \n" 
							"Content-length:%d\r\n"
							"Content-Type:text/plain\r\n\r\n",size);
				}
				else // else
				{
					sprintf(response_header,
							"HTTP/1.0 200 OK \r \n"
							"Server:2019 simple web server \r \n" 
							"Content-length:%d\r\n"
							"Content-type:text/html\r\n\r\n",size);
				}




			}

		}
		else
		{
			chdir(buff); // chdir rootfolder
			memset(response_message,0,sizeof(response_message));
			if(!strcmp(url,"/"))
			{
				FILE*file = fopen("html_ls.html","w"); // file open
				if(file == 0) // if file is error
				{
					printf("error1\n");
					return -1;
				}
				sprintf(response_message,"<h1>WELCOME TO SYSTEM PROGRAMMING HTTP</h1>");
				char *A[100];	 // set url
				{
					strcpy(url,"/.");
					A[1] = "-l";
				}

				A[0] = "./a.out";
				A[2] = url+1;
				html(file,3,A,buff); // make html file
				fclose(file);


			}

			else
			{

				FILE*file = fopen("html_ls.html","w"); // file open
				if(file == 0) // if file is error		
				{
					printf("error2\n");
					return -1;
				}


				char *A[100];

				sprintf(response_message,"<h1>System Programming Http</h1>");

				A[0] = "./a.out"; // set url
				A[1] = "-al";
				A[2] = url+1;

				html(file,3,A,buff); // make html
				fclose(file);

			}

			sprintf(response_header,
					"HTTP/1.0 200 OK \r \n"
					"Server:2019 simple web server \r \n" 
					"Content-length:%lu\r\n"
					"Content-type:text/html\r\n\r\n",strlen(response_message)); // it is directory,there is only one type.



			FILE *pFile = NULL;
			char strTemp1[1024];
			char strsum1[200000] = {0,};
			char *pStr1;
			pFile = fopen( "html_ls.html", "r" );
			if( pFile != NULL )
			{
				while( !feof( pFile ) )
				{

					pStr1 = fgets( strTemp1, sizeof(strTemp1), pFile );
					strcat(strsum1,pStr1);

				}
				fclose( pFile );
				sprintf(response_body,"%s",strsum1); // read html data and make body
			}


		}







		struct stat sb;
		lstat(url+1,&sb);
		int size = sb.st_size; // get filesize
		char buf1[500000];
		int A;
		int fd = open(url+1,O_RDONLY|O_NONBLOCK);
		if (S_ISLNK(sb.st_mode))
			size = strlen(response_message);


		if(fnmatch("*.jpg",url+1,FNM_CASEFOLD)== 0 || fnmatch("*.png",url+1,FNM_CASEFOLD)== 0||fnmatch("*.jpeg",url+1,FNM_CASEFOLD)== 0 && S_ISLNK(sb.st_mode))
		{ // get filesize if it is linkfile
			FILE *fp = fopen(url+1, "rb"); 
			fseek(fp, 0, SEEK_END);   
			size = ftell(fp);             
			fseek(fp,0,SEEK_SET); // file pointer initilization
			fclose(fp);

		}






		read(fd,buf1,size);	// copy binary information
		write(client_fd,response_header,strlen(response_header));
		if(fnmatch("*.jpg",url+1,FNM_CASEFOLD)== 0 || fnmatch("*.png",url+1,FNM_CASEFOLD)== 0||fnmatch("*.jpeg",url+1,FNM_CASEFOLD)== 0 || Z==1)
		{ // it is binary file?? write size
			write(client_fd,buf1,size);
			Z=0;
		}
		else // else
		{

			write(client_fd,response_message,strlen(response_message));
			Z=0;
		}

		write(client_fd,response_body,strlen(response_body)); // make body
		}
		end = clock();
		sleep(5);

		clockresult = (double)(end - start);
		printf("==============Disconnected Client==============\n");

		char templog[500];
		sprintf(templog,"==============Disconnected Client==============\n");// thread call, make logfile
		pthread_create(&tpid,NULL,&logfile,(void *)&templog);
		pthread_join(tpid,NULL);


		printf("TIME : [%s]\nURL : %s\nIP : %s \nPort : %d\nPID : %d\n",temptime,url,inet_ntoa(inet_client_address),client_addr.sin_port,getpid()); // new client connection

	sprintf(templog,"TIME : [%s]\nURL : %s\nIP : %s \nPort : %d\nPID : %d\n",temptime,url,inet_ntoa(inet_client_address),client_addr.sin_port,getpid());
		pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
		pthread_join(tpid,NULL);

		printf("CONNECTING TIME : %.0f(us)\n\n",clockresult);

	sprintf(templog,"CONNECTING TIME : %.0f(us)\n\n",clockresult);
		pthread_create(&tpid,NULL,&logfile,(void *)&templog);// thread call, make logfile
		pthread_join(tpid,NULL);

		pthread_create(&tpid,NULL,&reagain,NULL);
		pthread_join(tpid,NULL);


		//printf("%s %d client was disconnected\n\n\n", inet_ntoa(inet_client_address),client_addr.sin_port); 
		close(client_fd); // close connection
		//_exit(EXIT_SUCCESS);

		//signal(SIGCHLD,(void*)child_handler); // signal handler is to terminate child process to prevent zombie process.


	}

	close(socketfd);
	return 0;
}


