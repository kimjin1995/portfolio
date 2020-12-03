#include "AVLNode.h"
AVLNode::AVLNode()
{
}

AVLNode::~AVLNode()
{
}

CityData * AVLNode::GetCityData()
{
	// return Citydata type.
	return pCityData;
}

AVLNode * AVLNode::GetLeft()
{
	// return pLeft
	return pLeft;
}

AVLNode * AVLNode::GetRight()
{
	// return pRight
	return pRight;
}

int AVLNode::GetmBF()
{
	// return mBF
	return mBF;
}

void AVLNode::SetCityData(CityData * node)
{
	// set pcitydata
	pCityData = node;
}

void AVLNode::SetLeft(AVLNode * node)
{
	//set leftnode
	pLeft = node;
}

void AVLNode::SetRight(AVLNode * node)
{
	//set rightnode
	pRight = node;
}

void AVLNode::SetmBF(int n)
{
	// set mbf
	mBF = n;
}
