#include "TransferWindowManager.h"
#include <fstream>
#include <string>
#include<iostream>
#include<cstdlib>
#include<stdlib.h>
#include<cstring>
#pragma warning(disable:4996)
using namespace std;

TransferWindowManager::SoccerTeam::SoccerTeam()
{
	//You don't need to edit this function.
}

TransferWindowManager::SoccerTeam::SoccerTeam(SoccerPlayerData fw, SoccerPlayerData mf, SoccerPlayerData df, SoccerPlayerData gk) // set sum of transfer fee and ability.
{
	this->fw = fw;
	this->mf = mf;
	this->df = df;
	this->gk = gk; // set four Soccer Player structure and binary search tree.

	this->sum_transfer_fee = fw.m_transfer_fee + mf.m_transfer_fee + df.m_transfer_fee + gk.m_transfer_fee; // set sum of transfer fee
	this->sum_ability = fw.m_ability + mf.m_ability + df.m_ability + gk.m_ability; // set sum of ability
}

std::ostream& operator<<(std::ostream& os, const TransferWindowManager::SoccerTeam& team) // print sum of transfer_fee and ability
{
	os << team.fw << std::endl;
	os << team.mf << std::endl;
	os << team.df << std::endl;
	os << team.gk << std::endl;

	os << "sum_transfer_fee " << team.sum_transfer_fee << std::endl;
	os << "sum_ability " << team.sum_ability << std::endl;

	// set four team and print result.

	return os;
}


TransferWindowManager::TransferWindowManager(string file_dir, int budget) // get file name and budget
{
	m_budget = budget; // set budget
	std::ifstream in(file_dir.c_str()); // get filename

	if (!in.is_open()) // there is no file??? exit program.
	{
		std::cout << "There is no file" << endl;
		exit(1);
	}

	string line;
	char tempstring[100];  
	char name[100];
	char position[100];
	char price[100];
	char stat[100];
	const char* temp; // set variable to get the sentence and use strtok function.

	while (getline(in, line)) // read file.
	{

		TreeNode * pNew = new TreeNode; // set node.

		temp = line.c_str(); // convert string type to array type.
		//Fill in the code to initialize 4 Binary Search Trees(fw, mf, df, gk)
		strcpy(tempstring, temp); // set tempstring to use strsok
		strcpy(name,strtok(tempstring, ",")); // get name
		strcpy(position, strtok(NULL, ",")); // get position
		for (int num = 0; num < strlen(position); num++) // delete space using for statement.
			position[num] = position[num + 1];
		strcpy(price, strtok(NULL, ","));
		for (int num = 0; num < strlen(price); num++) // delete space using for statement.
			price[num] = price[num + 1];
		strcpy(stat, strtok(NULL, " ,")); // get stat
		pNew->m_data.m_name = name; // set pNew data name
		pNew->m_data.m_ability = atoi(stat); // set pNew data  ability
		pNew->m_data.m_position = position; // set pNew data position
		pNew->m_data.m_transfer_fee = atoi(price); // set pNew data transfer_fee

		if(!strcmp(position,"Forward")) // if forward position?? insert node fwBST.
		fwBST.insert(pNew->m_data);
		else if (!strcmp(position, "Midfielder")) // if midfilder position?? insert node mfBST.
			mfBST.insert(pNew->m_data);
		else if (!strcmp(position, "Defender")) // if defender position?? insert node dfBST.
			dfBST.insert(pNew->m_data);
		else if (!strcmp(position, "Goalkeeper")) // if goalkeeper position?? insert node gkBST.
			gkBST.insert(pNew->m_data);
	}
}

std::ostream& operator<<(std::ostream& os, const TransferWindowManager& manager)
{
	os << "********Forward List********" << std::endl; // print forward list using inorder traversal.
	os << manager.fwBST;
	os << "****************************" << std::endl; 

	os << "********Midflder List********" << std::endl; // print midfilder list using inorder traversal.
	os << manager.mfBST;
	os << "*****************************" << std::endl;

	os << "********Defender List********" << std::endl; // print defender list using inorder traversal.
	os << manager.dfBST;
	os << "*****************************" << std::endl;

	os << "********Goalkeeper List********" << std::endl; // print goalkeeper list using inorder traversal.
	os << manager.gkBST;
	os << "*******************************" << std::endl;

	return os;
}

void TransferWindowManager::removePlayer(SoccerPlayerData data) // remove function
{
	if (data.m_position == "Forward") // forward position delete, getting forward player ability.
		fwBST.deletion(data.m_ability);
	else if (data.m_position == "Midfielder") // modfilder position delete, getting modfilder player ability.
		mfBST.deletion(data.m_ability);
	else if (data.m_position == "Defender")// defender position delete, getting defender player ability.
		dfBST.deletion(data.m_ability);
	else //if (data.m_position == "Goalkeeper", goalkeeper position delete, getting goalkeeper player ability.
		gkBST.deletion(data.m_ability);
}

void TransferWindowManager::inorder(TreeNode*fw, TreeNode*mf, TreeNode*df, TreeNode*gk, SoccerTeam &soccer) // inorder function. first is forward.
{
	if (fw == NULL)  // if forward BST is null?? break
		return;
	inorder(fw->m_left, mf, df, gk,soccer); // move fw left
	inorder1(fw, mf, df, gk, soccer); // inorder1 function.
	inorder(fw->m_right, mf, df, gk, soccer); // move fw right
}
void TransferWindowManager::inorder1(TreeNode*fw, TreeNode*mf, TreeNode*df, TreeNode*gk, SoccerTeam &soccer) // inorder function. second is midfilder.
{
	if (mf == NULL) // if midfilder BST is null?? break
		return;
	inorder1(fw, mf->m_left, df, gk, soccer); // move mf left
	inorder2(fw, mf, df, gk, soccer); // inorder2 function
	inorder1(fw, mf->m_right, df, gk, soccer);// move mf right
}
void TransferWindowManager::inorder2(TreeNode*fw, TreeNode*mf, TreeNode*df, TreeNode*gk, SoccerTeam &soccer) // inorder function, third is defender.
{
	if (df == NULL) //if defender BST is null?? break
		return;
	inorder2(fw, mf, df->m_left, gk, soccer); // move df left
	inorder3(fw, mf, df, gk, soccer); // inorder3 function
	inorder2(fw, mf, df->m_right, gk, soccer); // move df right
}
void TransferWindowManager::inorder3(TreeNode*fw, TreeNode*mf, TreeNode*df, TreeNode*gk, SoccerTeam &soccer) // inorder function, forth is goalkeeper.
{
	int fwnum, mfnum, dfnum, gknum,sum; 
	int fwstat, mfstat, dfstat, gkstat, statsum; // set variable to get the stat of sum and fee
	if (gk == NULL)
		return;
	inorder3(fw, mf, df, gk->m_left, soccer); //move gk left
	fwnum = fw->m_data.m_transfer_fee;
	mfnum = mf->m_data.m_transfer_fee;
	dfnum = df->m_data.m_transfer_fee;
	gknum = gk->m_data.m_transfer_fee; // get transfer fee each position
	sum = fwnum + mfnum + dfnum + gknum; // get sum of transfer fee
	fwstat = fw->m_data.m_ability;
	mfstat = mf->m_data.m_ability;
	dfstat = df->m_data.m_ability;
	gkstat = gk->m_data.m_ability; // get ability each position
	statsum = fwstat + mfstat + dfstat + gkstat; // get sum of ability
	if (soccer.sum_transfer_fee <= 0 && sum <= m_budget) // if first data??set sum of transfer fee and ability.
	{
		soccer.sum_transfer_fee = sum;
		soccer.sum_ability = statsum;
		soccer.fw = fw->m_data;
		soccer.mf = mf->m_data;
		soccer.df = df->m_data;
		soccer.gk = gk->m_data;
	}
	else if ((soccer.sum_ability < statsum && sum <= m_budget) || (soccer.sum_transfer_fee > sum && soccer.sum_ability == statsum))
	{// if ability is higher or same than before stat and more cheap?? set sum of ability and transfer fee.
		soccer.sum_transfer_fee = sum;
		soccer.sum_ability = statsum;
		soccer.fw = fw->m_data;
		soccer.mf = mf->m_data;
		soccer.df = df->m_data;
		soccer.gk = gk->m_data;
	}

	inorder3(fw, mf, df, gk->m_right, soccer); // move gk to right
}
TransferWindowManager::SoccerTeam TransferWindowManager::getBestTeam() // get best team function
{
	SoccerTeam best_team; // set function

	//Fill in the code to search the best_team from 4 BSTs
	//The datas for best team must be stored in the variable best_team.
	//*best team means the most powerful team that is defined in the project introduction file. 
	TreeNode *fwbest = fwBST.m_root;
	TreeNode *mfbest = mfBST.m_root;
	TreeNode *dfbest = dfBST.m_root;
	TreeNode *gkbest = gkBST.m_root; //set four BET root node
	inorder(fwbest,mfbest,dfbest,gkbest,best_team); // calculate the best team using inorder-traversal function




	fwBST.deletion(best_team.fw.m_ability);
	mfBST.deletion(best_team.mf.m_ability);
	dfBST.deletion(best_team.df.m_ability);
	gkBST.deletion(best_team.gk.m_ability); // delete player from BST.

	return best_team; // return best team.
}
