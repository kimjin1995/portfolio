#include "stdafx.h"
#include "Node.h"


Node::Node()
{
}


Node::~Node()
{
}
void Node::SetNextNode(Node *next) // Next node is next.
{
	Next = next;
}
void Node::SetPrevNode(Node *prev) // Prev node is prev.
{
	Prev = prev;
}


void Node::Setrow(int row) // this->row is row.
{
	this -> row = row;
}
void Node::Setcolumn(int column) // this->column is column
{
	this->column = column;
}

Node *Node::GetNextNode() // return Nextnode.
{
	return Next;
}
Node *Node::GetPrevNode() // return Prevnode
{
	return Prev;
}
int Node::Getrow() // return row.
{
	return row;
}
int Node::Getcolumn() // return column
{
	return column;
}