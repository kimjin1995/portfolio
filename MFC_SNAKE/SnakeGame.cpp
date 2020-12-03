#include "stdafx.h"
#include "SnakeGame.h"


SnakeGame::SnakeGame()
{
}


SnakeGame::~SnakeGame()
{
}


void SnakeGame::insertNode(Node * node, int type) // node insert sequence start. get two parameter.
{
	Node *pCur = this->node; // set pcurnode. pCur is head.
	Node *pCurtemp = pCur; // set pcurtemp to get tail->prev node.
	if (pCur == NULL) // if pcur is NULL? need headnode
		this->node = node;
	else if(type == 0) // if first loop sequence??
	{
		while (pCur->GetNextNode()) // move to next node is null?
		{
			pCur = pCur->GetNextNode();
		}
		pCur->SetNextNode(node); // set nextnode
		pCur->GetNextNode()->SetPrevNode(pCur); // set prevnode
	}
	else
	{
		while (pCur->GetNextNode()) // move until next node is NULL
		{
			pCur = pCur->GetNextNode(); // move Node.
			pCurtemp = pCurtemp->GetNextNode();
		}
		pCur = pCur->GetPrevNode(); // pCur is tail->prevnode

		pCur->SetNextNode(node); // set tail->prev->nextnode
		pCurtemp->SetPrevNode(node); // set tail->prevnode

		int nodetemprow;
		int nodetempcolumn;
		nodetemprow = node->Getrow(); // get information of node row
		nodetempcolumn = node->Getcolumn(); // get information of node column

		node->Setrow(pCurtemp->Getrow()); // change information. swap data
		node->Setcolumn(pCurtemp->Getcolumn());
		pCurtemp->Setcolumn(nodetempcolumn);
		pCurtemp->Setrow(nodetemprow);

		node->SetNextNode(pCurtemp); // node insert is end.
		node->SetPrevNode(pCur);

	}

}
void SnakeGame::Changecoordinate(Node *node,int row, int column) // changecoordinate function
{
	Node *pCur = node; // set pcurnode and change information
	pCur->Setrow(row); // change row
	pCur->Setcolumn(column); // change column
}

Node*SnakeGame::GetNode() // getnode function
{
	return node; // return node
}