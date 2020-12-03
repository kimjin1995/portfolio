#pragma once

#include "SoccerPlayerData.h"
#include <iostream>

class TreeNode
{
	friend class BinarySearchTree;
	friend class TransferWindowManager;
private:
	SoccerPlayerData m_data;
	TreeNode* m_left;
	TreeNode* m_right; // set private variable
public:
	TreeNode() :m_left(NULL), m_right(NULL) {} // set constructor
	~TreeNode() { }
	TreeNode(SoccerPlayerData & data, TreeNode* left = NULL, TreeNode* right = NULL) // set data and node
	{
		this->m_data = data;
		this->m_left = left;
		this->m_right = right;
	}
	TreeNode* getLeftNode() const { return m_left; }
	TreeNode* getRightNode() const { return m_right; }
	friend std::ostream& operator<<(std::ostream& os, const TreeNode* node) // print inorder-traversal of binary search tree, using recursive function.
	{
		if(node != NULL) // node is null??
		os << node->m_data << std::endl; // print data
		return os;
	}
};
