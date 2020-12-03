#pragma once

#include "TreeNode.h"
#include <iostream>

class BinarySearchTree
{
	friend class TransferWindowManager;
private:
	TreeNode * m_root; // set rootnode
public:
	BinarySearchTree() { m_root = NULL;	} // rootnode is NULL.
	~BinarySearchTree();

	void insert(SoccerPlayerData& data); // insertion function 
	void deletion(int ability); // ability = key

	static TreeNode* inorder(std::ostream& os,TreeNode * tree) // inorder function
	{
		if (tree == NULL) // tree is null??
			return 0;

		inorder(os,tree->getLeftNode()); // go to the leftnode
		os << tree; // print tree
		inorder(os,tree->getRightNode()); // go to the rightnode
	}

	friend std::ostream& operator<<(std::ostream& os, const BinarySearchTree& tree) { // inorder print tree.
		TreeNode *node = tree.m_root;
		
		os << inorder(os,node);
		
		return os;
	}
};
