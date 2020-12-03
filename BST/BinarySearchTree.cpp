#include "BinarySearchTree.h"
#include<iostream>
#include<stdio.h>
#include<stdlib.h>

BinarySearchTree::~BinarySearchTree()
{
}


void BinarySearchTree::insert(SoccerPlayerData& data) // Binary Search Tree insert function.
{
	TreeNode * TR = m_root; // set root
	TreeNode *pNew = new TreeNode; // set data node
	pNew->m_data.m_ability = data.m_ability;
	pNew->m_data.m_name = data.m_name;
	pNew->m_data.m_position = data.m_position;
	pNew->m_data.m_transfer_fee = data.m_transfer_fee;
	// pNew node = data node.
	if (m_root == NULL) // if there is no node??? pNew is root node. also, left and right node is null.
	{
		m_root = pNew;
		m_root->m_left = NULL;
		m_root->m_right = NULL;
	}
	else
	{
		while (1) // use while statement to find correct place of pNew.
		{
			if (TR->m_data.m_ability > data.m_ability && TR->m_left != NULL) // node ability is bigger than pNew-data and left node is not null??? move left.
				TR = TR->m_left;
			else if (TR->m_data.m_ability > data.m_ability && TR->m_left == NULL) // node ability is bigger than pNew-data and left node is  null??? left node is pNew and break while statement.
			{
				TR->m_left = pNew;
				break;
			}
			else if (TR->m_data.m_ability < data.m_ability && TR->m_right != NULL)// node ability is lower than pNew-data and right node is not null??? move right.
				TR = TR->m_right;
			else if (TR->m_data.m_ability < data.m_ability && TR->m_right == NULL) // node ability is lower than pNew-data and right node is  null??? right node is pNew and break while statement.
			{
				TR->m_right = pNew;
				break;
			}
		}
	}
}
void BinarySearchTree::deletion(int ability) // ability = key, deletion function
{
	TreeNode * TR = m_root;
	TreeNode * TRB = m_root; // set two node to delete function
	if (m_root == NULL)// there is no node??? exit function.
	{
		exit(1);
	}
	while (1) // find the ability node to delete
	{
		if (TR->m_data.m_ability < ability) // if data is lower than key?? move right
		{
			TRB = TR;
			TR = TR->m_right;
		}
		else if (TR->m_data.m_ability > ability) // if data is higher than key?? move left.
		{
			TRB = TR;
			TR = TR->m_left;
		}
		else if (TR->m_data.m_ability == ability) // if data is same as key??? break.
			break;
	}
	TreeNode *DelNode = TR; // set delete node
	if (TR == m_root) // if deleted node is root node???
	{
		if (TR->m_right == NULL && TR->m_left == NULL) // there is no children?? delete right now.
		{
			free(TR);
			m_root = NULL;
		}
		else if (TR->m_right == NULL && TR->m_left != NULL) // if left child is exist and right child is not exist??? left child is root node.
		{
			m_root = TR->m_left;
			free(TR);
		}
		else if (TR->m_right != NULL && TR->m_left == NULL) // if right child is exist and left child is not exist?? right child is root node.
		{
			m_root = TR->m_right;
			free(TR);
		}
		else // if both children is exist??? the biggest node of left children can be root node.
		{
			TR = TR->m_left; // move left.
			while (TR->m_right) // move right to find biggest node.
			{
				DelNode = TR;
				TR = TR->m_right;
			}
			DelNode->m_right = TR->m_left; // delnode is NULL. TR->left is almost NULL.
			TR->m_left = m_root->m_left;	//
			TR->m_right = m_root->m_right;
			m_root = TR;
		}
	}
	else if (TRB->m_left == TR) // if node is left-side node??
	{
		if (TR->m_right == NULL && TR->m_left == NULL) // if node has no children?? just delete node.
		{
			TRB->m_left = NULL;
			free(TR);
		}
		else if (TR->m_right == NULL && TR->m_left != NULL) // if node has left child?? delete node and left node is TR node.
		{
			TRB->m_left = TR->m_left;
			free(TR);
		}
		else if (TR->m_right != NULL && TR->m_left == NULL)// if node has right child?? delete node and right node is TR node.
		{
			TRB->m_left = TR->m_right;
			free(TR);
		}
		else // if node has children??? move left and find biggest node.(left-side biggest node)
		{
			TR = TR->m_left;
			while (TR->m_right) // move right and find biggest node.
			{
				DelNode = TR;
				TR = TR->m_right;
			}
			DelNode->m_right = TR->m_left; // biggest-node-before- left node is biggest-node-left node.
			TR->m_left = TRB->m_left->m_left; // biggest node is root node.
			TR->m_right = TRB->m_left->m_right;
			TRB->m_left = TR;

		}
	}
		else if (TRB->m_right == TR) // if node is right side node??
		{
			if (TR->m_right == NULL && TR->m_left == NULL) // if node has no children??? just delete root node.
			{
				TRB->m_right = NULL;
				free(TR);
				//TR = NULL;
			}
			else if (TR->m_right == NULL && TR->m_left != NULL)// if node has left child?? delete node and left node is TR node.
			{
				TRB->m_right = TR->m_left;
				free(TR);
				//TR = NULL;
			}
			else if (TR->m_right != NULL && TR->m_left == NULL)// if node has right child?? delete node and right node is TR node.
			{
				TRB->m_right = TR->m_right;
				free(TR);
			}
			else// if node has children??? move left and find biggest node.(left-side biggest node)
			{
				TR = TR->m_left;
				while (TR->m_right)// move right and find biggest node.
				{
					DelNode = TR;
					TR = TR->m_right;
				}
				DelNode->m_right = TR->m_left;// biggest-node-before- left node is biggest-node-left node.
				TR->m_left = TRB->m_right->m_left; // biggest node is root node.
				TR->m_right = TRB->m_right->m_right;
				TRB->m_right = TR;

			}
	}

}
