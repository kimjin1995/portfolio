#include "Graph.h"
#include<utility>
#include<iostream>
#include<fstream>
#include<string.h>

using namespace std;

class Manager
{
public:
	AVLTree * avl;
	Graph * gp;
	// set two structure.
	char * cmd;
	int iter;
	// set variable
	int Loadcount = 0; // compute loadnumber.


public:
	Manager();
	~Manager();

	ofstream flog; // file output
	ifstream fin;// file input

	void run(const char * command); // run function
	bool LOAD(); // LOAD function
	bool INSERT(char* Location, char*country, char*capital); //INSERT function
	bool PRINT_AVL(); // PRINT_AVL function
	bool SEARCH_AVL(int Location);// SEARCH_AVL function
	bool DELETE_AVL(int Location);// DELETE_AVL function
	bool BUILD_GP();// BUILD_GP function
	bool PRINT_GP();// PRINT_GP function
	bool BUILD_MST();// BUILD_MST function
	bool PRINT_MST();// PRINT_MST function
	void EXIT();// EXIT function

	void printErrorCode(int n, const char * cmdname);// printErrorCode function
	void printSuccessCode(const char * cmdname);// printSuccessCode function
	void printSuccessCode2(const char * cmdname);// printSuccessCode2 function
	void printErrorCode2(int n, const char * cmdname);// printErrorCode2 function
	void printSuccessCode3(const char * cmdname, int num);// printSuccessCode2 function
	void printSuccessCode4(const char * cmdname, int num);// printSuccessCode2 function
};
