#pragma once
#include"Node.h"
class SnakeGame
{
private:
	Node *node = NULL;
public:
	SnakeGame();
	~SnakeGame();

	void insertNode(Node * node, int type); // Insert node function
	void Changecoordinate(Node *node, int row, int colum); // change coordinate function

	Node*GetNode(); // get headnode function


};

