#pragma once
class Node
{
private:
	Node *Prev = NULL;
	Node *Next = NULL;
	Node *Head = NULL;
	int row = 0;
	int column = 0;
	int changerow =  0;
	int changecolumn =  0;
	int changeindex = 0; // set initialized node's value.
public:
	Node();
	~Node();

	void SetNextNode(Node *next); // setNextNode function
	void SetPrevNode(Node *prev); // setPrevNode function
	void Setrow(int row);		 // setrow function
	void Setcolumn(int column); // set column function


	Node *GetNextNode(); // get nextnode function
	Node *GetPrevNode(); // get prevnode function
	int Getrow(); // getrow function
	int Getcolumn(); // get column function

};
