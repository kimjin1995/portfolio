#include <iostream>
#include "TransferWindowManager.h"
#include<cstring>
#pragma warning(disable:4996)
using namespace std;
int main(int argc, char** argv) // get two parameter
{
	if (atoi(argv[2]) < 1664) // if money lower than 1664?? break
	{
		std::cout << "The budget must be over than 1664." << std::endl;
		return 0;
	}
	TransferWindowManager transfer_window_manager(argv[1], atoi(argv[2])); // set binary search tree
	cout << transfer_window_manager << endl; // print inorder-traversal function
	TransferWindowManager::SoccerTeam team = transfer_window_manager.getBestTeam(); // get best team
	cout << "Best Players" << endl; // print best players and fee
	cout << team << endl;
	cout << "-----" << endl;
	cout << "The Transfer window close" << endl;
	cout << transfer_window_manager << endl; // delete four player and print result
	return 0;
}

