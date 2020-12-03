#include "Manager.h"
#include <stdlib.h>
#include <fstream>
#include<string>
using namespace std;
#pragma warning(disable:4996)
Manager::Manager() // manager constructor.
{
	flog.open("log.txt", ios::app); // open file
	flog.setf(ios::fixed); // append function set

	avl = new AVLTree(&flog); // avl and gp function
	gp = new Graph(&flog);
	iter = 0;
	cmd = NULL;
}

Manager::~Manager()
{
	flog.close(); // file close
}

void Manager::run(const char * command) // fun function
{
	fin.open(command); // file open

	cmd = new char[40];

	while (!fin.eof()) // read file
	{
		iter = iter + 1;
		fin.getline(cmd, 40);

		char * one = strtok(cmd, " "); // get command


		if (!strcmp(one, "LOAD")) {
			// call LOAD function. success?? print successcode. fail? print errorcode.
			if (LOAD()) printSuccessCode("LOAD");
			else printErrorCode(100, "LOAD");
		}
		else if (!strcmp(one, "INSERT")) {
			// call INSERT function. success?? print successcode. fail? print errorcode.
			char * Location;
			char * country;
			char * capital;
			Location = strtok(NULL, " ");
			country = strtok(NULL, " ");
			capital = strtok(NULL, " ");
			// get three data and send three data.
			if (INSERT(Location, country, capital));
			else
				printErrorCode(100, "INSERT");

		}
		else if (!strcmp(one, "PRINT_AVL")) {
			// call PRINT_AVL function. success?? print successcode. fail? print errorcode.
			if (PRINT_AVL());
			else
				printErrorCode(300, "PRINT_AVL");
		}
		else if (!strcmp(one, "SEARCH_AVL")) {
			// call SEARCH_AVL function. success?? print successcode. fail? print errorcode.
			char * 	Location = strtok(NULL, " ");
			if (Location == NULL) // if location is null?? there is no location. false.
				printErrorCode(500, "SEARCH_AVL");
			else
			{
				if (SEARCH_AVL(atoi(Location)));
				else
					printErrorCode2(500, "SEARCH_AVL");
			}
		}
		else if (!strcmp(one, "DELETE_AVL")) {
			// call DELETE_AVL function. success?? print successcode. fail? print errorcode.
			char * 	Location = strtok(NULL, " ");
			if (DELETE_AVL(atoi(Location))) printSuccessCode4("DELETE_AVL", atoi(Location));
			else
				printErrorCode(400, "DELETE_AVL");
		}
		else if (!strcmp(one, "BUILD_GP")) {
			// call BUILD_GP function. success?? print successcode. fail? print errorcode.
			if (BUILD_GP()) 	printSuccessCode("BUILD_GP");
			else
				printErrorCode(600, "BUILD_GP");
		}
		else if (!strcmp(one, "PRINT_GP")) {
			// call PRINT_GP function. success?? print successcode. fail? print errorcode.
			if (PRINT_GP());
			else
				printErrorCode2(700, "PRINT_GP");
		}
		else if (!strcmp(one, "BUILD_MST")) {
			// call BUILD_MST function. success?? print successcode. fail? print errorcode.
			if (BUILD_MST()) printSuccessCode("BUILD_MST");
			else
				printErrorCode(800, "BUILD_MST");
		}
		else if (!strcmp(one, "PRINT_MST")) {
			// call PRINT_MST function. success?? print successcode. fail? print errorcode.
			if (PRINT_MST());
			else
				printErrorCode2(900, "PRINT_MST");
		}
		else if (!strcmp(one, "EXIT")) {
			printSuccessCode("EXIT");
			// exit function.
			EXIT();
		}
		else { // unknown function.
			printErrorCode(0, "Unknown");
		}
	}
	fin.close(); // file close.
	return;
}
//LOAD function.
bool Manager::LOAD()
{
	ifstream fin("city_list.txt"); // read text file
	string city;
	char * copystring = new char[100];
	char * Location;
	char * country;
	char * capital;
	//  set four variable
	if (fin.is_open() != true || Loadcount == 1) // if it is already called or there is no file?? return false
		return false;

	Loadcount++; // locacount++.

	while (getline(fin, city)) // seperate each element, enroll node.
	{
		CityData * CD = new CityData; // set city data set.
		strcpy(copystring, city.c_str());
		Location = strtok(copystring, "\t");
		country = strtok(NULL, "\t");
		capital = strtok(NULL, "\t");
		// use strtok, seperate information and enroll to node.
		CD->SetLocationId(atoi(Location));
		CD->Setcountry(country);
		CD->Setname(capital);

		//insert function start.
		avl->Insert(CD);
	}

	return true;
}
// insert function
bool Manager::INSERT(char* Location, char*country, char*capital)
{
	// if rootnode is null??? return false.
	if (avl->Getroot() == NULL)
		return false;

	// set city data set.
	CityData *CD = new CityData;
	CD->SetLocationId(atoi(Location));
	CD->Setcountry(country);
	CD->Setname(capital);
	// we receive parameter. nformation enroll to node.
	// print insert function
	printSuccessCode2("INSERT");
	flog << "(" << Location << ", " << country << ", " << capital << ")" << endl;

	// insert start
	avl->Insert(CD);


	return true;
}
// PRINT_AVL function.
bool Manager::PRINT_AVL()
{
	// if rootnode is null?? return false.
	if (avl->Getroot() == NULL)
		return false;
	// print_avl function start.
	printSuccessCode2("PRINT_AVL");
	avl->Print();
	return true;
}
//search_avl function.
bool Manager::SEARCH_AVL(int Location)
{
	// print search_avl code.
	printSuccessCode3("SEARCH_AVL", Location);
	// ir avl_search success??? return true. else? return false,
	if (avl->Search(Location) == true)
		return true;
	else
		return false;
}
//delete_avl function
bool Manager::DELETE_AVL(int Location)
{
	//delete_Avl function set.
	if (avl->Delete(Location))// delete success?? return true. else?? return false.
		return true;
	else
		return false;
}
//build_gp function
bool Manager::BUILD_GP()
{
	// if there is no rootnode?? return false
	if (avl->Getroot() == NULL)
		return false;
	// print all of the node.
	gp->Build(avl);
	return true;
}
// print_gp function
bool Manager::PRINT_GP()
{
	// if print_gp success?? return true. else? return false
	printSuccessCode2("PRINT_GP");
	if (gp->Print_GP())
		return true;
	else
		return false;
}
// buile mst function
bool Manager::BUILD_MST()
{
	// if mst set is success?? return true, else?? return false
	if (gp->Kruskal())
		return true;
	else
		return false;
}
// print_mst function
bool Manager::PRINT_MST()
{
	// if print_mst function is complete?? return true. else?? return false.
	printSuccessCode2("PRINT_MST");
	if (gp->Print_MST())
		return true;
	else
		return false;
}
// exit function
void Manager::EXIT()
{
	// exit program
	avl = NULL;
	gp = NULL;
	exit(0);
}
// print errorcode. priunt errorcode and command
void Manager::printErrorCode(int n, const char * cmdname) {//ERROR CODE PRINNT FUNCTION
	flog << "==> command " << iter << ") " << cmdname << endl;
	flog << "Error code: " << n << endl;
}
// print successcode. print successcode and command
void Manager::printSuccessCode(const char * cmdname) {//SUCCESS CODE PRINNT FUNCTION
	flog << "==> command " << iter << ") " << cmdname << endl;
	flog << "Success" << endl;
}
//print command
void Manager::printSuccessCode2(const char * cmdname) {//SUCCESS CODE PRINNT FUNCTION
	flog << "==> command " << iter << ") " << cmdname << endl;
}
// print errorcode.
void Manager::printErrorCode2(int n, const char * cmdname) {//ERROR CODE PRINNT FUNCTION
	flog << "Error code: " << n << endl;
}
void Manager::printSuccessCode3(const char * cmdname, int num)
{
	flog << "==> command " << iter << ") " << cmdname << " " << num << endl;
}
void Manager::printSuccessCode4(const char * cmdname, int num)
{
	flog << "==> command " << iter << ") " << cmdname << " " << num << endl;
	flog << "Success" << endl;
}
