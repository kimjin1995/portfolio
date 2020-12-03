#pragma once

#include "BinarySearchTree.h"

#include <iostream>
#include <string>

class TransferWindowManager
{
private:
	int m_budget;

	BinarySearchTree fwBST;
	BinarySearchTree mfBST;
	BinarySearchTree dfBST;
	BinarySearchTree gkBST;
	 // set variable and Binary Search Tree.
public:

	struct SoccerTeam
	{
		SoccerPlayerData fw;
		SoccerPlayerData mf;
		SoccerPlayerData df;
		SoccerPlayerData gk; // set four data structure.

		int sum_transfer_fee;
		int sum_ability; // set variable

		SoccerTeam();
		SoccerTeam(SoccerPlayerData fw, SoccerPlayerData mf, SoccerPlayerData df, SoccerPlayerData gk); // set soccer team data
		friend std::ostream& operator<<(std::ostream& os, const SoccerTeam& team);
	};

	TransferWindowManager(std::string file_dir, int budget); // set function


	friend std::ostream& operator<<(std::ostream& os, const TransferWindowManager& manager); // set inorder function

	void removePlayer(SoccerPlayerData data); // set remove function

	SoccerTeam getBestTeam(); // get best team function


	void inorder(TreeNode*tree1, TreeNode*tree2, TreeNode*tree3, TreeNode*tree4, SoccerTeam &soccer); // inorder function to calculate using four Binary Search Tree.
	void inorder1(TreeNode*tree1, TreeNode*tree2, TreeNode*tree3, TreeNode*tree4, SoccerTeam &soccer);// using recursive function
	void inorder2(TreeNode*tree1, TreeNode*tree2, TreeNode*tree3, TreeNode*tree4, SoccerTeam &soccer);
	void inorder3(TreeNode*tree1, TreeNode*tree2, TreeNode*tree3, TreeNode*tree4, SoccerTeam &soccer);// calculate lowest fee and highest ability.
};

