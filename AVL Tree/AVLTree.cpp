#include "AVLTree.h"
#include<math.h>
AVLTree::AVLTree(ofstream * flog)
{
	this->flog = flog; // get flog textfile
}
AVLTree::~AVLTree()
{
}
AVLNode * AVLTree::Getroot()
{
	return root; // return root
}
void AVLTree::Setroot(AVLNode * node)
{
	root = node; // set root
}

// rotate type LL
AVLNode* AVLTree::Left_Left(AVLNode *parent, AVLNode *prev)
{
	AVLNode *child = parent->GetLeft(); // get Left child because of LL rotate type.
	if (prev->GetLeft() == parent && prev != parent)
	{
		/*
		change three node.
		define three node. prev, parent, parent->Left(child).
		this node shape is change.
		prev->Left is child, parent->left is child->right, child->right is parent.
		*/
		prev->SetLeft(child);
		parent->SetLeft(child->GetRight());
		child->SetRight(parent);

	}
	else if (prev->GetRight() == parent && prev != parent)
	{
		/*
		change three node.
		define three node. prev, parent, parent->right(child).
		this node shape is change.
		prev->Left is child, parent->left is child->right, child->right is parent.
		*/
		prev->SetRight(child);
		parent->SetLeft(child->GetRight());
		child->SetRight(parent);
	}
	else
	{
		/*
		it is mean that prev == parent. it is that root node have to changed.
		change three node.
		define two node. parent, parent->Left(child).
		this node shape is change.
		parne->left is child->right, child->right is parent. and child becom root node.
		*/
		parent->SetLeft(child->GetRight());
		child->SetRight(parent);
		Setroot(child);
	}
	return 0;
}
//rotate type RR
AVLNode* AVLTree::Right_Right(AVLNode *parent, AVLNode *prev)
{
	AVLNode *child = parent->GetRight(); // get Left child because of RR rotate type.
	if (prev->GetLeft() == parent && prev != parent)// 
	{
		/*
		change three node.
		define three node. prev, parent, parent->right(child).
		this node shape is change.
		prev->Left is child, parent->right is child->left, child->left is parent.
		*/
		prev->SetLeft(child);
		parent->SetRight(child->GetLeft());
		child->SetLeft(parent);
	}
	else if (prev->GetRight() == parent && prev != parent)
	{
		/*
		change three node.
		define three node. prev, parent, parent->right(child).
		this node shape is change.
		prev->Left is child, parent->right is child->left, child->left is parent.
		*/
		prev->SetRight(child);
		parent->SetRight(child->GetLeft());
		child->SetLeft(parent);
	}
	else
	{
		/*
		it is mean that prev == parent. it is that root node have to changed.
		change three node.
		define two node. parent, parent->right(child).
		this node shape is change.
		parne->right is child->left, child->left is parent. and child becom root node.
		*/
		parent->SetRight(child->GetLeft());
		child->SetLeft(parent);
		Setroot(child);
	}
	return 0;
}
// rotate type RL
AVLNode* AVLTree::Right_Left(AVLNode *parent, AVLNode *prev)
{
	/*
	change three node.
	define three node. prev, parent, parent->right(child).
	this node shape is change.
	first of all, we have to make it to RR shape. and then, we should rotate this node to RR.

	parent->right is child->left, child->left is parent->getright->getright, parent->right->right is child.
	and then, do the RR rotate.
	*/
	AVLNode *child = parent->GetRight();
	parent->SetRight(child->GetLeft());
	child->SetLeft(parent->GetRight()->GetRight());
	parent->GetRight()->SetRight(child);
	Right_Right(parent, prev);

	return 0;
}
AVLNode* AVLTree::Left_Right(AVLNode *parent, AVLNode *prev)
{
	/*
	change three node.
	define three node. prev, parent, parent->Left(child).
	this node shape is change.
	first of all, we have to make it to LL shape. and then, we should rotate this node to LL.

	parent->left is child->right, child->right is parent->left->left, parent->left->left is child.
	and then, do the LL rotate.
	and then, do the RR rotate.
	*/
	AVLNode *child = parent->GetLeft();
	parent->SetLeft(child->GetRight());
	child->SetRight(parent->GetLeft()->GetLeft());
	parent->GetLeft()->SetLeft(child);
	Left_Left(parent, prev);

	return 0;
}

// height return function
int AVLTree::height(AVLNode *node)
{
	if (node == NULL) // if node is NULL?? height is 0.
		return 0;
	else // else?? call the heightcall function and recutn height.
		return heightCal(node->GetLeft()) - abs(heightCal(node->GetRight()));
}
// height calculate function
int AVLTree::heightCal(AVLNode *node)
{
	/*
	node is null, then it is leaf node. so return 0.
	else, calculate the height, and compare two height of side and return biggst one.
	we shoud add 1 because height is started to 0.
	*/
	if (node == NULL)
		return 0;
	int Left = heightCal(node->GetLeft());
	int Right = heightCal(node->GetRight());

	if (Left > Right)
		return Left + 1;
	else
		return Right + 1;
}
// balance function
void AVLTree::balance(AVLNode *node)
{
	// if it receive node, calculate height of node and change mBF.
	node->SetmBF(height(node));
}
// inorder function.
void AVLTree::inorder(AVLNode *node)
{
	/*
	calculate all of the node's mBF.
	using postorder function to start to lear node.
	*/
	if (node == NULL)
		return;
	inorder(node->GetLeft());
	inorder(node->GetRight());
	balance(node);

}
// check node and apply rotate function appropriate situation.
void AVLTree::inorderTree(AVLNode *node)
{
	if (node == NULL) // node is null?? left node. return.
		return;
	inorderTree(node->GetLeft());
	inorderTree(node->GetRight());
	/*
	it use's postorder function.
	because, we should apply the rotate function when The smallest node we first met.
	*/

	AVLNode *Head = new AVLNode;
	Head = Getroot(); // started to root


	AVLNode *N = new AVLNode; // set two node to find node
	AVLNode *Np = new AVLNode;
	N = Getroot();
	Np = N;
	while (1)
	{
		/*
		compare two node.
		ndoe is bigger than compare node?? node is smaller than compare node. so, it move to left side.
		ndoe is smaller than compare node?? node is bigger than compare node. so, it move to right side.
		node is same as compare node?? we find it! exit the while statement.
		*/
		if (strcmp(node->GetCityData()->Getcountry(), N->GetCityData()->Getcountry()) < 0)
		{
			Np = N;
			N = N->GetLeft();
		}
		else if (strcmp(node->GetCityData()->Getcountry(), N->GetCityData()->Getcountry()) > 0)
		{
			Np = N;
			N = N->GetRight();
		}
		else
			break;
		if (N == NULL)
			break;
	}
	inorder(Head); // set all of node's mBF.


	/*
	compare node's mBF.
	mbf is bigger than 1 or lower than -1?? it have to apply rotate function.
	and then, We need to know what it looks like.
	so, compare height and apply appropriate rotate function.
	*/
	if (node->GetmBF() > 1)
	{
		if (height(node->GetLeft()) >= 1) // rotate LL type.
			Left_Left(node, Np);
		else // rotate LR type.
			Left_Right(node, Np);
	}
	else if (node->GetmBF() < -1)
	{
		if (height(node->GetRight()) <= -1) // rotate RR type.
			Right_Right(node, Np);
		else // rotate RL type.
			Right_Left(node, Np);
	}


}

// insert function
bool AVLTree::Insert(CityData * node)
{
	AVLNode * Node = new AVLNode;
	AVLNode *Prev = new AVLNode;
	AVLNode * pNew = new AVLNode;
	AVLNode *Head = new AVLNode;
	// set four variable


	Head = Getroot();
	pNew = Getroot();
	Prev = pNew;
	//set four node.
	if (Getroot() == NULL)
	{
		/*
		root is null?? there is no node.so node become root node.
		*/
		Node->SetCityData(node);
		Node->SetLeft(NULL);
		Node->SetRight(NULL);
		Setroot(Node);

	}
	else
	{
		/*
		else?? we find appropriate location.
		node is bigger than compare node?? node is smaller than compare node. so, it move to left side.
		node is smaller than compare node?? node is bigger than compare node. so, it move to right side.
		it move until pNew is NULL.
		*/
		Node->SetCityData(node);
		Node->SetLeft(NULL);
		Node->SetRight(NULL);
		while (pNew)
			if (strcmp(node->Getcountry(), pNew->GetCityData()->Getcountry()) < 0)
			{
				Prev = pNew;
				pNew = pNew->GetLeft();
			}
			else if (strcmp(node->Getcountry(), pNew->GetCityData()->Getcountry()) > 0)
			{
				Prev = pNew;
				pNew = pNew->GetRight();
			}
		/*
		compare node and Prev node.
		if prev node is smaller than node?? node is bigger than prev node.
		so it have to right side.
		if prev node is bigger than node?? node is smaller than prev node.
		so it have to left side.

		node input sequance is complete, set each node height.
		and, compare all of node, and it have to apply rotate function, rotate function start.
		*/
		if (strcmp(node->Getcountry(), Prev->GetCityData()->Getcountry()) > 0)
		{
			Prev->SetRight(Node);
			inorder(Head);
			inorderTree(Head);
		}
		else if (strcmp(node->Getcountry(), Prev->GetCityData()->Getcountry()) < 0)
		{
			Prev->SetLeft(Node);
			inorder(Head);
			inorderTree(Head);
		}
	}
	return true;
}

//deleteinorder function.
AVLNode* AVLTree::deleteinorder(AVLNode *node, int num)
{
	// delete inorder function. if node is null?? return 0.
	if (node == NULL)
		return 0;
	deleteinorder(node->GetLeft(), num);

	//it is inorder function. search the node and if we find node, delete sequance start.

	if (num == node->GetCityData()->GetLocationId())
	{
		//if we find node, deletenum set.
		deletenum++;
		AVLNode *Prev = new AVLNode;
		AVLNode *Del = new AVLNode;
		AVLNode *pNew = new AVLNode;
		AVLNode *pPrev = new AVLNode;
		pNew = root;
		Prev = pNew;
		// set four node.
		while (pNew)
		{
			/*
			compare two node.
			ndoe is bigger than compare node?? node is smaller than compare node. so, it move to left side.
			ndoe is smaller than compare node?? node is bigger than compare node. so, it move to right side.
			node is same as compare node?? we find it! exit the while statement.
			*/
			if (pNew == NULL)
				break;
			if (strcmp(pNew->GetCityData()->Getcountry(), node->GetCityData()->Getcountry()) < 0)
			{
				Prev = pNew;
				pNew = pNew->GetRight();
			}
			else if (strcmp(pNew->GetCityData()->Getcountry(), node->GetCityData()->Getcountry()) > 0)
			{
				Prev = pNew;
				pNew = pNew->GetLeft();
			}
			else
				break;
		}

		if (pNew == Prev)
		{
			/*
			if pNew == prev?? it is root node.
			delete node and the largest node on the left is the root.
			there is four case.
			1. if root node's left and right node is NULL?? just delete node.
			2. if root node's left side is null and right side node is not null?? root->right become root.
			3. if root node's right side is null and left side node is not null?? root->left become root.
			4. if root node's left side and right side node is not null?? delete root node and the largest node on the left is the root.
			*/
			if (pNew->GetLeft() == NULL && pNew->GetRight() == NULL) // delete root
			{
				free(pNew);
				pNew = NULL;

			}
			else if (pNew->GetLeft() == NULL) // right side become root
			{
				Setroot(pNew->GetRight());
				free(pNew);
			}
			else if (pNew->GetRight() == NULL) // left side become root
			{
				Setroot(pNew->GetLeft());
				free(pNew);
			}
			else // the largest node on the left become the root.
			{
				Del = pNew;
				Del = Del->GetLeft();
				if (Del->GetRight() == NULL)
				{
					Del->SetRight(pNew->GetRight());
					Setroot(Del);
				}
				else
				{
					while (Del->GetRight())
					{
						Prev = Del;
						Del = Del->GetRight();
					}
					Prev->SetRight(Del->GetLeft());
					Del->SetLeft(pNew->GetLeft());
					Del->SetRight(pNew->GetRight());
					Setroot(Del);
					free(pNew);
				}
			}
		}
		else
		{
			/*
			first, we have to find ndoe how it is look likes.
			we shoud know the the right side of prev node is pNew or left side of prev node is pNew.
			and delete node.
			delete node and the largest node on the left is the root.
			there is four case.
			1. if root node's left and right node is NULL?? just delete node.
			2. if root node's left side is null and right side node is not null?? root->right pNew node.
			3. if root node's right side is null and left side node is not null?? root->left pNew node.
			4. if root node's left side and right side node is not null?? delete root node and the largest node on the left is the pNew node.
			*/
			if (Prev->GetRight() == pNew)// right side??
			{
				if (pNew->GetLeft() == NULL && pNew->GetRight() == NULL) // it is leaf node?? delete node.
				{
					Prev->SetRight(NULL);
					free(pNew);
				}
				else if (pNew->GetLeft() == NULL) // left side is null?? right side is pNew
				{
					Prev->SetRight(pNew->GetRight());
					free(pNew);
				}
				else if (pNew->GetRight() == NULL) // right side is null?? left side is pNew
				{
					Prev->SetRight(node->GetLeft());
					free(pNew);
				}
				else // left and right side is not null?? the largest node on the left is the pNew node.
				{
					pPrev = Prev;
					Del = pNew;
					Del = Del->GetLeft();
					while (Del->GetRight()) // search node
					{
						Prev = Del;
						Del = Del->GetRight();
					}
					Prev->SetRight(Del->GetLeft());
					Del->SetLeft(pNew->GetLeft());
					Del->SetRight(pNew->GetRight());
					pPrev->SetRight(Del);
					free(pNew);
				}
			}
			else if (Prev->GetLeft() == pNew) // left side node??
			{
				if (pNew->GetLeft() == NULL && pNew->GetRight() == NULL) // it is leaf node?? delete node
				{
					Prev->SetLeft(NULL);
					free(pNew);
				}
				else if (pNew->GetLeft() == NULL) // left side is null? left side is pNew
				{
					Prev->SetLeft(pNew->GetRight());
					free(pNew);
				}
				else if (pNew->GetRight() == NULL) // right side is null? right side is pNew
				{
					Prev->SetLeft(node->GetLeft());
					free(pNew);
				}
				else // left and right side is not null?? the largest node on the left is the pNew node.
				{
					pPrev = Prev;
					Del = pNew;
					Del = Prev->GetLeft();
					while (Del->GetRight()) // search node
					{
						Prev = Del;
						Del = Del->GetRight();
					}
					Prev->SetRight(Del->GetLeft());
					Del->SetLeft(pNew->GetLeft());
					Del->SetRight(pNew->GetRight());
					pPrev->SetLeft(Del); // delete and set pNew
					free(pNew);


				}

			}
		}
		node = NULL;
	}

	if (node != NULL) // inorder function
		deleteinorder(node->GetRight(), num);
	else
		return 0;
}
//delete function
bool AVLTree::Delete(int num)
{
	deletenum = 0; // set deletenum
	AVLNode *pNew = new AVLNode;
	AVLNode *pDel = new AVLNode;
	pNew = Getroot(); //set two node.
	pDel = deleteinorder(pNew, num);

	AVLNode *Head = new AVLNode;
	Head = Getroot();
	inorder(Head);
	inorderTree(Head); // rearrange

	if (deletenum == 0) // delete is complete?? return true. else? return false.
		return false;
	else
		return true;

}
//searchinorder function.
void AVLTree::searchinorder(AVLNode *node, int num)
{
	// if node is NULL?? return
	if (node == NULL)
		return;
	/*
	using inorder function.
	if find node?? findnum ++ and print the result.
	*/

	searchinorder(node->GetLeft(), num);
	if (num == node->GetCityData()->GetLocationId())//we find node?
	{
		*flog << "(" << node->GetCityData()->GetLocationId() << ", " << node->GetCityData()->Getcountry() << ", " << node->GetCityData()->Getname() << ")" << endl;//print node.
		findnum++;
	}
	searchinorder(node->GetRight(), num);
}
// search function
bool AVLTree::Search(int num)
{
	// set findnum.
	findnum = 0;
	AVLNode *P = new AVLNode;
	P = Getroot(); // get root node.
	searchinorder(P, num);
	if (findnum == 1)// we success to find node?? return true. else?? return false.
		return true;
	else
		return false;
}
//print inorder function.
void AVLTree::printinorder(AVLNode *node)
{
	// if node is NULL?? return.
	if (node == NULL)
		return;
	/*
	using inorder function.
	print all of the node.
	*/
	printinorder(node->GetLeft());
	*flog << "(" << node->GetCityData()->GetLocationId() << ", " << node->GetCityData()->Getcountry() << ", " << node->GetCityData()->Getname() << ")" << endl;
	printinorder(node->GetRight());
}
// print node function
bool AVLTree::Print()
{
	/*
	set rootnode and print all of the node.
	*/
	AVLNode *P = new AVLNode;
	P = Getroot();
	printinorder(P);
	return true;
}
