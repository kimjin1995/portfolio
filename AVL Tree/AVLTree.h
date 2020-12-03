#include "AVLNode.h"
#include <iostream>
#include <fstream>// header file declare
using namespace std;
class AVLTree
{
public:
	AVLNode * root;	// AVLTree Root
	ofstream * flog; // text file
	int findnum = 0; //  search variable. search success?? findnum++
	int deletenum = 0; // delete variable. delete success?? deletenum ++

public:
	AVLTree(ofstream * flog); // avltree constructor
	~AVLTree(); // avltree destructor


	AVLNode* Left_Left(AVLNode *parent, AVLNode *prev); // LL rotate
	AVLNode* Left_Right(AVLNode *parent, AVLNode *prev); // LR rotate
	AVLNode* Right_Left(AVLNode *parent, AVLNode *prev); // RL rotate
	AVLNode* Right_Right(AVLNode *parent, AVLNode *prev); // RR rotate
	void balance(AVLNode *node); // set avltree height
	void inorder(AVLNode *node); // inorder function to search node to set mBF num
	void inorderTree(AVLNode *node); // inorder Tree function. using postorder function to search smallest node.
	int height(AVLNode *node); // height calculate function
	int heightCal(AVLNode *node); // return biggest height
	void printinorder(AVLNode *node); // print inorder function. using inorder function to print node
	void searchinorder(AVLNode *node, int num); // search function. using inorder function to search node
	AVLNode* deleteinorder(AVLNode *node, int num); // delete node function. find and delete node.
	AVLNode  *  Getroot(); // get root function, return root
	void	 	Setroot(AVLNode * node); // set root function.
	bool		Insert(CityData * node); // insert function
	bool	Delete(int num); // delete function
	bool	Search(int num); // serarch function
	bool		Print(); // print function

};

