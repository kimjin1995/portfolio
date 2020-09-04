////////////////////////////////////////////////////////////////
//File Name	: tue_2014722024_ls.c				//
//Date		: 2019/4/25					//
//Os		: Ubuntu 16.05 LTS 64bits			//
//Author	: Kin jin young					//
//Student ID	: 201472204					//
//--------------------------------------------------------------//
//Title : System Programming Assignment #2-3(Make l,a,ls function)
//        using option, h,S,r option and wildcard
//////////////////////////////////////////////////////////////////
#include<stdio.h>
#include<unistd.h>
#include<string.h>
#include<fnmatch.h>
#include<dirent.h>
#include<sys/stat.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include"tue_2014722024_ls.c"// get the ls.c header

int html(FILE* file,int argc, char **argv,char path[])
{
	

	char *wd=(char*)malloc(sizeof(char)*50);
	getcwd(wd,50);
	fprintf(file,"<body>"); // make html body
	fprintf(file,"<table border = \"1\">");


	struct dirent *dir;
	struct stat file_info;
	DIR *dirp;
	int aflag = 0,bflag = 0, hvalue=0, Svalue=0, rvalue=0;
	char *avalue = NULL;
	char*a;
	int c = 0;
	int index = 0;
	
	int i = 0;
	opterr = 0;
	// read a,l option
	while((c = getopt(argc,argv,"alhSr")) != -1)
	{
		// optind
		switch(c)
		{
			case 'a':
				aflag = 1;
				break;
			case 'l':
				bflag = 1;
				break;
			case 'h':
				hvalue = 1;
				break;
			case 'S':
				Svalue = 1;
				break;
			case 'r':
				rvalue = 1;
				break;
			case '?':
				//printf("Unknown option character\n");
				fprintf(file,"Unknown option character<br/>");
				break;
		}
	}
	int find = 0;//compare and add file or directory

	dirp = opendir("."); // open directory

	/////////////////// searching not existing file or dir function//////////////////////////////
	if(argc >= 2) // if argumane is 2?? path or function.
	{
		for(int i=optind; i<argc; i++) // get the string to compare
		{
			if(argv[i][strlen(argv[i])-1] != '*') // if it is wildcard??
			{
				while((dir=readdir(dirp))!= NULL)//read directiry
				{
					if(!chdir(argv[i])) // if it is directory and exist?
					{
						chdir(wd);
						find++;
					}
					else if(!fnmatch(argv[i],dir->d_name,0)) // compare it, if found, find ++
					{
						if(aflag == 1) // if a function, hiddenfile can find
						{
							int result = chdir(argv[i]);
							chdir(wd); // back to original path
							if(result == 0)
							{
								find++;
							}
							else
							{
								find++;
							}
						}
						else if(aflag != 1 && argv[i][0] != '.') // if a is 0, hidden file can't find.
						{
							int result = chdir(argv[i]);
							chdir(wd);
							if(result == 0)
							{
								find++;
							}
							else
							{
								find++;
							}
						}

					}
				}
			}
			rewinddir(dirp);//rewind dir.
			chdir(wd); // return dir
			if(find == 0 && argv[i][strlen(argv[i])-1] != '*' &&  chdir(argv[i])) // there is no file and it is not wildcard and it is file?:
			{
				//printf("cannot access '%s' : No such file or directory\n",argv[i]);
				fprintf(file,"Can't access '%s' : No such file or directory<br/>",argv[i]);
			}
			chdir(wd); // return.
			find = 0; // find initialization

		}

		////////////////////////////////directory or file  searching function //////////////////////////////////

		for(int i=optind; i<argc; i++)
		{
			chdir(wd);
			if(argv[i][strlen(argv[i])-1] != '*') // if it is wildcard?
			{
				while((dir=readdir(dirp))!= NULL) // readdir
				{
					if(!chdir(argv[i])) // if there is directory??
					{
						chdir(wd);
						find++;
					}
					else if(!fnmatch(argv[i],dir->d_name,0)) // compare it, if found, find ++
					{
						if(aflag == 1) // if afunction, hiddenfile can find
						{
							int result = chdir(argv[i]);
							chdir(wd);
							if(result == 0)
							{
								find++;
							}
							else // if there is file
							{
								
								//printf("%s\n",dir->d_name);
								fprintf(file,"%s<br/>",dir->d_name);
								find++;
							}
						}
						else if(aflag != 1 && argv[i][0] != '.')// if not afunction, hiddenfile can't find.
						{
							int result = chdir(argv[i]);
							chdir(wd);
							if(result == 0)
							{
								find++;
							}
							else // if there is file??
							{
								//printf("%s\n",dir->d_name);
							fprintf(file,"%s<br/>",dir->d_name);
								find++;
							}
						}

					}
				}
			}
			rewinddir(dirp);
			find = 0;

		}





	}


	if(argc == 2) // argc = 2? function or path
		if((aflag == 1 && bflag == 1))  	 	//al
		{
			chdir(wd);
			lsfunction(file,argc,argv,2,".",rvalue,Svalue,hvalue,aflag,bflag,path);
		}
		else if(aflag == 1)			//a
		{
			chdir(wd);
			lsfunction(file,argc,argv,1,".",rvalue,Svalue,hvalue,aflag,bflag,path);
		}
		else if(bflag == 1)//l
		{
			chdir(wd);
			lsfunction(file,argc,argv,0,".",rvalue,Svalue,hvalue,aflag,bflag,path);
		}
		else if(Svalue == 1 || hvalue == 1 || rvalue == 1) // only S,h,rvalue?
		{
			chdir(wd);
			lsfunction(file,argc,argv,3,".",rvalue,Svalue,hvalue,aflag,bflag,path);
		}
		else
		{
			if(!chdir(argv[1])) // if argv[1] is path?
			{
				chdir(wd);
				lsfunction(file,argc,argv,6,argv[1],rvalue,Svalue,hvalue,aflag,bflag,path);
			}
			else if(!strcmp(argv[1],"*")) // if argv[1] is wildcard??
			{
				chdir(wd);
				lsfunction(file,argc,argv,5,"*",rvalue,Svalue,hvalue,aflag,bflag,path);
			}
			else if(argv[1][strlen(argv[1])-1] == '*') // if argv[1] is path and wildcard?
			{
				chdir(wd);
				lsfunction(file,argc,argv,5,argv[1],rvalue,Svalue,hvalue,aflag,bflag,path);
			}

		}
	else if(argc == 1) // default value
	{
		lsfunction(file,argc,argv,6,".",rvalue,Svalue,hvalue,aflag,bflag,path);
	}
	else// if aggv > 2?
		while(optind < argc) //get optind
		{
			//printf("%d %d\n", aflag, bflag);
			chdir(wd);
			if(!strcmp(argv[optind],"*")) // if it is wildcard?
			{

				lsfunction(file,argc,argv,5,"*",rvalue,Svalue,hvalue,aflag,bflag,path);
				chdir(wd);
				optind++;

			}
			else if(argv[optind][strlen(argv[optind])-1] == '*') // it is wildcard and path?
			{
				lsfunction(file,argc,argv,5,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				chdir(wd);
				optind++;
			}//
			else if(aflag == 1 && bflag == 1) // if function is -al
			{
				if(!chdir(argv[optind]))
				{
					chdir(wd);
					lsfunction(file,argc,argv,2,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				}
				optind++;
			}
			else if(aflag == 1) // if function is -a
			{
				if(!chdir(argv[optind]))
				{
					chdir(wd);
					lsfunction(file,argc,argv,1,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				}
				optind++;
			}
			else if(bflag == 1) // if function is -l
			{
				if(!chdir(argv[optind]))
				{
					chdir(wd);
					lsfunction(file,argc,argv,0,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				}
				optind++;
			}
			else if(Svalue == 1 || hvalue == 1 || rvalue == 1)
			{
				if(!chdir(argv[optind]))
				{
					chdir(wd);
					lsfunction(file,argc,argv,3,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				}
				optind++;
			}
			else
			{
				if(!chdir(argv[optind]))// it is path and there is no option??
				{
					chdir(wd);
					lsfunction(file,argc,argv,3,argv[optind],rvalue,Svalue,hvalue,aflag,bflag,path);
				}
				optind++;
			}

		}
 // close file
}

