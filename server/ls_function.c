////////////////////////////////////////////////////////////////
//File Name	: tue_2014722024_ls.c				//
//Date		: 2019/4/20					//
//Os		: Ubuntu 16.05 LTS 64bits			//
//Author	: Kin jin young					//
//Student ID	: 201472204					//
//--------------------------------------------------------------//
//Title : System Programming Assignment #2-3(Make ls function)  //
//////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<dirent.h>
#include<string.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<unistd.h>
#include<pwd.h>
#include<grp.h>
#include<time.h>
#include<fnmatch.h>
#include<unistd.h>
#include<dirent.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define URL_LEN 256
#define PORTNO 40206



#define K 1024
#define M 1024*1024
#define G 1024*1024*1024


//Sort function//

void Sort(FILE *file,int jcount, int kcount, char **str,int count,char **A,int sum,int index,int rvalue, int Svalue, int hvalue,double size[],int aflag, int bflag,char root[]);

//////////////// Get the information of Directory////////////////
void printdir(FILE *file,char path[],int index,int rvalue, int Svalue, int hvalue, int aflag, int bflag,char root[])
{

	int i=0,count = 0,find=0;
	char *temppath =(char *)malloc(sizeof(char)* 100) ;
	int jcount=0, kcount=0;
	DIR *dirp;
	char *str[20000];
	char *A[20000];
	double tempsize[20000];
	int sum= 0,recursive = 0;
	char permission[11];
	struct dirent *dir;
	char *file_name;
	struct stat file_info;
	struct tm *time;
	mode_t file_mode;
	struct passwd *my_passwd;
	struct group *my_group;
	chdir(path);

	dirp = opendir(".");

	char name[100];
	getcwd(name,100);
	chdir(name);
	if(index != 5 && index != 6) // print diractory path if not searching function, print HTML
	{
		fprintf(file,"Directory path: %s\n",name);
		//printf("Directory path: %s \n",name);
	}
	while((dir=readdir(dirp)) != NULL)
	{
		str[count] = dir->d_name;
		lstat(dir->d_name,&file_info);

		my_passwd = getpwuid(file_info.st_uid);
		my_group = getgrgid(file_info.st_gid);
		file_mode = file_info.st_mode;
		int i=0;
		// make permission string
		if (S_ISLNK(file_mode))
			permission[i++]= 'l';	
		else if (S_ISDIR(file_mode))
			permission[i++]= 'd';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IRUSR)
			permission[i++]= 'r';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IWUSR)
			permission[i++]= 'w';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IXUSR)
			permission[i++]= 'x';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IRGRP)
			permission[i++]= 'r';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IWGRP)
			permission[i++]= 'w';	
		else
			permission[i++]= '-';	

		if(file_mode & S_IXGRP)
			permission[i++]= 'x';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IROTH)
			permission[i++]= 'r';	
		else
			permission[i++]= '-';	

		if (file_mode & S_IWOTH)
			permission[i++]= 'w';	
		else
			permission[i++]= '-';	

		if(file_mode & S_IXOTH)
			permission[i++]= 'x';	
		else
			permission[i++]= '-';

		permission[10] = '\0';
		//get the information of directory or file
		char * p = (char*)malloc(sizeof(char)*100);
		char * q = (char*)malloc(sizeof(char)*100);
		char * r = (char*)malloc(sizeof(char)*100);
		char * s = (char*)malloc(sizeof(char)*100);
		char * t = (char*)malloc(sizeof(char)*100);
		char * u = (char*)malloc(sizeof(char)*100);
		strcpy(p, permission); // permition[10] per[9] = ---------
		sprintf(q,"%ld",file_info.st_nlink); // link
		if(hvalue == 1) // hvalue = convert unit size
		{
			if(file_info.st_size < K) // size < 1024
				sprintf(r,"%ld",file_info.st_size);
			else if(file_info.st_size < M) // size < 1024 * 1024
			{
				double fsize =(double)file_info.st_size/K;
				sprintf(r,"%.1f",fsize);
				strcat(r,"K");
			}
			else if(file_info.st_size < G) // size < 1024 *1024 * 1024
			{
				double fsize =(double) file_info.st_size/M;
				sprintf(r,"%.1f",fsize);
				strcat(r,"M");
			}
			else // size > G
			{
				double fsize =(double) file_info.st_size/G;
				sprintf(r,"%.1f",fsize);
				strcat(r,"G");

			}
		}
		else // if not h function
			sprintf(r,"%ld",file_info.st_size);

		tempsize[count] = file_info.st_size;
		time = localtime(&(file_info.st_mtime));
		strftime(s,80,"%b %d %H:%M", time);
		strcpy(t, my_passwd->pw_name);
		strcpy(u,my_group->gr_name);	

		//combine all string, make a string	
		A[count] =p; //*A[200],count = read file
		strcat(A[count],"\t");
		strcat(A[count],q);
		strcat(A[count],"\t");
		strcat(A[count],t);
		strcat(A[count],"\t");
		strcat(A[count],u);
		strcat(A[count],"\t");
		strcat(A[count],r);
		strcat(A[count],"\t");
		strcat(A[count],s);
		strcat(A[count],"\t");

		if(index == 1 || index == 2)//if a function is contained, print hidden file
			sum += file_info.st_blocks;
		else if(str[count][0] != '.') // if a function is not containded, hiddenfile can't find.
			sum += file_info.st_blocks;
		count++;

	}
	int change = 0;
	if(index == 5) // if need recursive function, wildcard
	{		

		char nametemp[100];
		char strtemp[100];
		if(aflag == 1 && bflag == 1) // wildcard function, must get the index
			index = 2;
		else if(aflag == 1)
			index = 1;
		else if(bflag == 1)
			index = 0;
		else 
		{
			index = 3;	
		}
		getcwd(nametemp,100); // get temppath

		Sort(file,jcount,kcount,str,count,A,sum/2,7,rvalue,Svalue,hvalue,tempsize,aflag,bflag,root);

		printdir(file,".",index,rvalue,Svalue,hvalue,aflag,bflag,root);
		printdir(file,"..",index,rvalue,Svalue,hvalue,aflag,bflag,root);
		change ++;

		chdir(nametemp);
		memset(nametemp,0,100);
		int pathnum =0;
		for(int i=0; i<count; i++) // for statement for searchinf directory
		{

			getcwd(nametemp,100);
			if(aflag == 1) // it can find hidden directory
			{
				if(!chdir(str[i])) // it is directory??
				{
					pathnum++; 
				}
			}
			else if(bflag == 1 && aflag == 0) // it can't find hidden directory
			{
				if(!chdir(str[i])) // it is directory?
					pathnum++;
			}
			chdir(nametemp);
			memset(nametemp,0,100);
		}	


		for(int i=0; i<count; i++) // for statement for print directory
		{
			char *wildcardtemp = (char*)malloc(sizeof(char)*100);
			getcwd(wildcardtemp,100);
			if(pathnum != 1) // if directory count is not 1? print directory
			{
				if(aflag == 1) // it can print hidden directory
				{
					if(!chdir(str[i])) // it is directory?
					{
						if(strcmp(str[i],"..")  && strcmp(str[i],"."))
						{
							printdir(file,str[i],3,rvalue,Svalue,hvalue,aflag,bflag,root);
						}
						chdir(wildcardtemp);
					}
				}
				else if(aflag == 0 && str[i][0] != '.') // it can't print hiddedn directory
				{
					if(!chdir(str[i])) 
						printdir(file,str[i],3,rvalue,Svalue,hvalue,aflag,bflag,root);
					chdir(wildcardtemp);
				}
			}
			else // if directory is only 1?
			{
				strcpy(temppath,path); // make path
				strcat(temppath,"/");
				strcat(temppath,str[i]);

				if(aflag == 1) // it can print hidden directory
				{
					if(!chdir(temppath)) // it is directory?
					{
						if(strcmp(str[i],"..")  && strcmp(str[i],".")) // it is not root folder
						{
							//printf("%s\n",temppath);
							fprintf(file,"%s\n",temppath);
						}
						chdir(wildcardtemp);
					}
				}
				else if(aflag == 0 && str[i][0] != '.') // if it is not hidden hiddenfile or rootfolder
				{
					if(!chdir(temppath))
					{
						//printf("%s\n",temppath);
						fprintf(file,"%s\n",temppath);
					}
					chdir(wildcardtemp);
				}

			}

			chdir(nametemp);
			memset(nametemp,0,100);

		}
	}

		if(aflag == 1 && bflag == 1) // wildcard function, must get the index
			index = 2;
		else if(aflag == 1 )
			index = 1;
		else if(bflag == 1)
			index = 0;
	if(change == 0)
	{
		if(index == 1 || index == 3 || index == 6) // if function is -l
			Sort(file,jcount,kcount,str,count,0,sum/2,index,rvalue,Svalue,hvalue,tempsize,aflag,bflag,root);
		if(index == 2 || index == 0 ) // if function is -a or -al
{
			Sort(file,jcount,kcount,str,count,A,sum/2,index,rvalue,Svalue,hvalue,tempsize,aflag,bflag,root);
}
	}
	sum = 0;

	free(temppath);
	closedir(dirp);
}
// Print the contents of Directory, and Close Directory and End function////
/////////////////ls function//////////////////////////////////
int lsfunction(FILE *file,int argc, char **argv,int index,char*path,int rvalue, int Svalue, int hvalue,int aflag, int bflag,char root[])
{

	DIR*dirp;
	struct dirent *dir;
	char temppath[100];
	memset(temppath,0,100);
	char nametemp[100];
	getcwd(nametemp,100);
	if( path[0] == '/') // if search function and get directorypath 
	{

		if(path[strlen(path)-1] == '*') // if there is wildcard
		{
			strncpy(temppath,path,strlen(path)-2); // get rid of wildcard and change directory
			if(chdir(temppath))
			{
				//printf("ls : cannot access %s/* : no such file or directory\n",temppath);
				fprintf(file,"ls : cannot access %s/* : no such file or directory\n",temppath);
				return 0;
			}
			chdir(nametemp);


			printdir(file,temppath,5,rvalue,Svalue,hvalue,aflag,bflag,root); // if it is directory

		}
		else
		{
			if(chdir(path))
			{
				//printf("ls : cannot access %s/* : no such file or directory\n",temppath);
	fprintf(file,"ls : cannot access %s/* : no such file or directory\n",temppath);
				return 0;
			}
			chdir(nametemp);


			strcpy(temppath,path); // if there is no wildcard

			printdir(file,temppath,index,rvalue,Svalue,hvalue,aflag,bflag,root); // if it is directory
		}

	}
	else if(index == 5 && !strcmp(path,"*"))
	{
		printdir(file,nametemp,5,rvalue,Svalue,hvalue,aflag,bflag,root); // if it is directory
	}
	else  // if searching function and it is not a path
	{ 
		chdir(nametemp);
		if(chdir(path))
		{
			//printf("ls : cannot access %s/* : no such file or directory\n",path);
				fprintf(file,"ls : cannot access %s/* : no such file or directory\n",temppath);
			return 0;
		}
		chdir(nametemp);
		printdir(file,path,index,rvalue,Svalue,hvalue,aflag,bflag,root);
		chdir(nametemp);
	};

	return 0;
}
/////////////End function, close Directory/////////////

////////// Start Sort function////////////////////////
void Sort(FILE *file,int jcount, int kcount, char **str,int count,char **A,int sum,int index,int rvalue, int Svalue, int hvalue,double size[],int aflag, int bflag ,char root[])
{

	int tempindex = index;
	fprintf(file,"<body>"); // make HTML body
	fprintf(file,"<table border = \"1\">");
	if(index == 6)
		index = 3;
	int countj=0, countk=0;
	if(index != 3 && tempindex != 7) // index is 3?? just print function, body start
{
		//printf("total : %d\n",sum);
	fprintf(file,"<br>Total : %d<br/>",sum);
	if(bflag == 1)
		fprintf(file,"<tr><th>Name</th><th>Permission</th><th>Link</th><th>Owner</th><th>Group</th><th>Size</th><th>Last Modified</th></tr>");
	else
		fprintf(file,"<tr><th>Name</th></tr>");
}
	if(aflag == 0 && bflag == 1)
		index = 0;
	else if(aflag == 1 && bflag == 1)
		index = 2;
	else if(aflag == 1 && bflag == 0)
		index = 1;
	else if((rvalue || Svalue || hvalue) && aflag == 0 && bflag == 0)
		index = 3;

	// if file name is changed, file information must be changed
	if(Svalue == 0) // if Svalue is 0, normal order is executed.
	{
		for(int j=0; j<count; j++)// for statement for compare two string
		{
			for(int k=j+1; k<count; k++)
			{	
				char *tempstring = (char*)malloc(sizeof(char)*100); //dynamic allocation two string to compare
				char *tempstring1 = (char*)malloc(sizeof(char)*100);
				strcpy(tempstring,str[j]); // get the string
				strcpy(tempstring1,str[k]);
				if(tempstring[0] == '.') // if it is hiddenfile??
				{
					strncpy(tempstring,tempstring+1,sizeof(tempstring)-1);
				}
				if(tempstring1[0] == '.') 
				{
					strncpy(tempstring1,tempstring1+1,sizeof(tempstring1)-1);
				}

				if(str[j][0] == '.' && str[j][1] < 91 && str[j][1]>63) // str[j] is hiddenfile and uppercase??
				{
					str[j][1] += 32; countj=1;
					tempstring[0]+=32;
				}
				else if(str[j][0] != '.' &&str[j][0] < 91 && str[j][0]>63)
				{
					str[j][0] += 32; jcount=1;
					tempstring[0] += 32;
				}
				if(str[k][0] == '.' &&str[k][1] < 91 && str[k][1]>63) // str[k] is hiddenfile and uppercase>
				{
					str[k][1] += 32; countk=1;
					tempstring1[0] += 32;
				}
				else if(str[k][0] != '.' &&str[k][0] < 91 && str[k][0]>63)
				{
					str[k][0] += 32; kcount=1;
					tempstring1[0] += 32;
				}

				if(strcmp(tempstring,tempstring1) > 0) // compare method
				{
					char *stemp = str[j]; // change attributes
					str[j] = str[k];
					str[k] = stemp;
					if(index == 2 || index == 0) // if al or l
					{
						char *temp = A[j];
						A[j] = A[k];
						A[k] = temp;
					}
					if(countj == 1)	{str[k][1] -= 32; countj=0;} // it is changed, rechange it
					if(countk == 1)	{str[j][1] -= 32; countk=0;}
					if(jcount == 1)	{str[k][0] -= 32; jcount=0;}
					if(kcount == 1)	{str[j][0] -= 32; kcount=0;}
				}
				if(countj == 1)	{str[j][1] -= 32; countj=0;}
				if(countk == 1)	{str[k][1] -= 32; countk=0;}
				if(jcount == 1)	{str[j][0] -= 32; jcount=0;}
				if(kcount == 1)	{str[k][0] -= 32; kcount=0;}

			}
		}
	}
	else
	{
		for(int j=0; j<count; j++) // if Svalue is 1, order method is changed.
			for(int k=j; k<count; k++)
			{

				if(size[j] < size[k]) // ascending order is executed.
				{
					char *stemp = str[j]; // change attributes
					str[j] = str[k];
					str[k] = stemp;
					double temps = size[j];
					size[j] = size[k];
					size[k] = temps;
					if(index == 2 || index == 0) // if function is al or a?
					{
						char *temp = A[j];
						A[j] = A[k];
						A[k] = temp;
					}
				}
				else if(size[j] == size[k]) // if size is same, compare string and change decsending order
				{
					char *tempstring = (char*)malloc(sizeof(char)*100); // set two string to compare
					char *tempstring1 = (char*)malloc(sizeof(char)*100);
					strcpy(tempstring,str[j]);
					strcpy(tempstring1,str[k]);
					if(tempstring[0] == '.') // it is hiddenfile??
					{
						strncpy(tempstring,tempstring+1,sizeof(tempstring)-1);
					}
					if(tempstring1[0] == '.')
					{
						strncpy(tempstring1,tempstring1+1,sizeof(tempstring1)-1);
					}
					if(str[j][0] == '.' && str[j][1] < 91 && str[j][1]>63) // if str[j][0] is hiddenfile and uppercase?
					{
						str[j][1] += 32; countj=1;
						tempstring[0]+=32;
					}
					else if(str[j][0] != '.' &&str[j][0] < 91 && str[j][0]>63) 
					{
						str[j][0] += 32; jcount=1;
						tempstring[0] += 32;
					}


					if(str[k][0] == '.' &&str[k][1] < 91 && str[k][1]>63) // if str[k][0] is hiddenfile and uppercase?
					{
						str[k][1] += 32; countk=1;
						tempstring1[0] += 32;
					}
					else if(str[k][0] != '.' &&str[k][0] < 91 && str[k][0]>63)	
					{
						str[k][0] += 32; kcount=1;
						tempstring1[0] += 32;
					}
					if(strcmp(tempstring,tempstring1) > 0) // compare method
					{
						char *stemp = str[j]; // descending order
						str[j] = str[k];
						str[k] = stemp;
						if(index == 2 || index == 4) // if al or l
						{
							char *temp = A[j];
							A[j] = A[k];
							A[k] = temp;
						}
						if(countj == 1)	{str[k][1] -= 32; countj=0;} // if string is changed
						if(countk == 1)	{str[j][1] -= 32; countk=0;}
						if(jcount == 1)	{str[k][0] -= 32; jcount=0;}
						if(kcount == 1)	{str[j][0] -= 32; kcount=0;}
					}
					if(countj == 1)	{str[j][1] -= 32; countj=0;}
					if(countk == 1)	{str[k][1] -= 32; countk=0;}
					if(jcount == 1)	{str[j][0] -= 32; jcount=0;}
					if(kcount == 1)	{str[k][0] -= 32; kcount=0;}


				}
			}
	}

	char temp7[100]; // Name
	getcwd(temp7,100);

	char buff[100];
	int rootA = strlen(root);
	int rootB = 0;
	for(int i=0; i<strlen(temp7); i++)
		if(root[i] == temp7[i])
			rootB++;
		else
			break;

	if(rootB != strlen(temp7))
		strncpy(buff,temp7+rootB,strlen(temp7)-rootB);
	//printf("%s %s %s\n",buff,temp7,root);


	if(rvalue == 0) // if rvalue is 0, nomal function is executed.
	{
		if(index == 0) // if function is -l, make body
		{
			for(int j=0; j<count; j++)
			{
				if(tempindex != 7)
				if((str[j][0] != '.' && strcmp(str[j],"html_ls.html") && tempindex != 7) || (A[j][0] != 'd' && tempindex == 7))
				{
					//printf("%s ",A[j]);
					//printf("%s\n",str[j]);
				char **tok = (char**)malloc(sizeof(char*)*20);
				for(int t = 0; t <20; t++)
					tok[t] = (char*)malloc(sizeof(char)*20);
				int tempt = 0;
					tok[tempt++] = strtok(A[j],"\t");
					while(tok[tempt-1] != NULL)
						tok[tempt++] = strtok(NULL,"\t");
					if(tok[0][0] == 'd')
								fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
					else if(tok[0][0] == 'l')
						fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
					else
				
fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);


				}	

			}
		}
		else if(index == 1) // if function is -al or -a, make body
			for(int j=0; j<count; j++)
			{			
			if((strcmp(str[j],"html_ls.html") && tempindex != 7) || (tempindex == 7 && A[j][0] != 'd'))	
{
				//printf("%s\n",str[j]);	
				fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th></tr>",str[j],str[j]);

}			}
		else if(index == 2)
			for(int j=0; j<count; j++)
			{
	if((strcmp(str[j],"html_ls.html") && tempindex != 7) || (tempindex == 7 &&A[j][0] != 'd')) // if file is not html, print body
{
				//printf("%s ",A[j]);
				//printf("%s\n",str[j]);	
				int tempt = 0;
				char **tok = (char**)malloc(sizeof(char*)*20);
				for(int t = 0; t <20; t++)
					tok[t] = (char*)malloc(sizeof(char)*20);
				tok[tempt++] = strtok(A[j],"\t");
				while(tok[tempt-1] != NULL)
					tok[tempt++] = strtok(NULL,"\t");
				strcat(buff,"/");
				strcat(buff,str[j]);
				if(tok[0][0] == 'd')
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th></tr>",buff,str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				else if(tok[0][0] == 'l')
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th></tr>",buff,str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				else
					
fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th></tr>",buff,str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				memset(buff+strlen(temp7)-rootB,0,100-strlen(temp7)+rootB);	


		}
			}
		else if (index == 3 || index == 5) // default function or wildcard, print html
		{
			for(int j=0; j<count; j++)
			{
				if((str[j][0] != '.' && strcmp(str[j],"html_ls.html") && tempindex != 7) ||(tempindex == 7 && A[j][0] != 'd'))
				{
					//printf("%s\n",str[j]);
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th></tr>",str[j],str[j]);

				}


			}
		}


	}
	else if (rvalue == 1) // rvalue is on, reverse function is executed.
	{
	
		if(index == 0) // if function is -l
		{
			for(int j=count-1; j>=0; j--) // reverse, makebody and print result
			{
				if(str[j][0] != '.'&& strcmp(str[j],"html_ls.html"))
				{
					//printf("%s ",A[j]);
					//printf("%s\n",str[j]);
				char **tok = (char**)malloc(sizeof(char*)*20);
				for(int t = 0; t <20; t++)
					tok[t] = (char*)malloc(sizeof(char)*20);
				int tempt = 0;
					tok[tempt++] = strtok(A[j],"\t");
					while(tok[tempt-1] != NULL)
						tok[tempt++] = strtok(NULL,"\t");

					if(tok[0][0] == 'd')
						fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
					else if(tok[0][0] == 'l')
						fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
					else
						
fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				}	
			}
		}
		else if(index == 1) // if function is -al or -a, make body and print result
			for(int j=count-1; j>=0; j--)
			{
				if(strcmp(str[j],"html_ls.html"))
{
				//printf("%s\n",str[j]);	
				fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th></tr>",str[j],str[j]);
}
			}
		else if(index == 2) // al function, make body and print result
			for(int j=count-1; j>=0; j--)
			{
if(strcmp(str[j],"html_ls.html"))
{
				//printf("%s ",A[j]);
				//printf("%s\n",str[j]);	
				char **tok = (char**)malloc(sizeof(char*)*20);
				int tempt = 0;
				for(int t = 0; t <20; t++)
					tok[t] = (char*)malloc(sizeof(char)*20);
				tok[tempt++] = strtok(A[j],"\t");

				while(tok[tempt-1] != NULL)
					tok[tempt++] = strtok(NULL,"\t");

				if(tok[0][0] == 'd')
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th><th style=\"color:blue\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				else if(tok[0][0] == 'l')
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th><th style=\"color:green\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
				else
					
fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th><th style=\"color:red\">%s</th></tr>",str[j],str[j],tok[0],tok[1],tok[2],tok[3],tok[4],tok[5]);
			}
}
		else if (index == 3) // default function.
		{
			for(int j=count-1; j>=0; j--)
			{
				if(str[j][0] != '.'&& strcmp(str[j],"html_ls.html"))
				{				
					//printf("%s\n",str[j]);
					fprintf(file,"<tr><th><a href = \"%s\"> %s </a> </th></tr>",str[j],str[j]);
				}
			}
		}
	}
	fprintf(file,"</table></body>");

	//printf("\n");
}
//////////////////////End of sort function////////////////////

