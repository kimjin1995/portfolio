#include "Graph.h"
#include <map>
#include<math.h>
#pragma warning(disable:4996)
Graph::Graph(ofstream * flog)
{
	this->flog = flog; // get flog textfile
}

Graph::~Graph()
{
}


// setList functoin
void Graph::SetList(AVLNode *root)
{
	//if root is null?? break.
	if (root == NULL)
		return;
	/*
	use inorder function.
	set mList data. key is location id, and second is citydata
	*/
	SetList(root->GetLeft()); // left list
	(mList)->insert(pair<int, CityData*>(++graphcount, root->GetCityData())); // insert data
	SetList(root->GetRight()); // right list

}

//Build function
bool Graph::Build(AVLTree * root)
{
	// set mlist dynamic allocation.  set memory.
	mList = new map<int, CityData *>;

	map<int, CityData *>::iterator ListCount; // set listcount to compute list size.
	vertex = new CityData*[100]; // set vertex dynamic allocation, two demension and size is 100*100
	for (int i = 0; i < 100; i++)
		vertex[i] = new CityData[100];


	AVLNode *Node = new AVLNode; // set node
	Node = root->Getroot();// get rootnode
	SetList(Node); // set mList.

	int count = 0; // get count
	for (ListCount = mList->begin(); ListCount != mList->end(); ListCount++) // count mList'nodes.
	{
		vertex[count][0] = *(ListCount->second); // vertex is graph node's information.
		count++;
	}
	size = count; // size ifs count.

	// set complete connected graph.
	for (int i = 0; i < count; i++)
	{
		for (int j = 1; j < count + 1; j++)
		{
			vertex[i][j].SetLocationId(abs(vertex[i]->GetLocationId() - vertex[j - 1]->GetLocationId())); // set complete connected graph. calculate distance difference. and store it to vertex array.
		}
	}


	return true;
}

//print_gp function
bool Graph::Print_GP()
{
	// if size is 0??? there is no graph. return false.
	if (size == 0)
		return false;
	map<int, CityData *>::iterator ListCount;

	// print start
	for (int i = 0; i < size; i++)
	{
		for (int j = 1; j < size + 1; j++)
		{
			*flog << vertex[i][j].GetLocationId() << "\t"; // start i = 0, j = 1, and print all of the locationId.
		}
		*flog << endl; // input endl.
	}
	return true;
}

//print_mst function
bool Graph::Print_MST()
{
	// if mstcount is 0?? there is no mst. break.
	if (MSTcount == 0)
		return false;
	// int total
	int total = 0;
	for (int i = 0; i < size - 1; i++) // print the shortest distance and print it.
	{
		if (edgecount[i] != 0)
		{
			*flog << "(" << cityone[i] << " , " << citytwo[i] << "), " << edgecount[i] << endl;
			total += edgecount[i]; // get total number
		}
	}
	*flog << "Total :" << total << endl; // print total

	return true;
}
// kruskal algorithm
bool Graph::Kruskal()
{
	// if size is 0?? there is no graph. return false
	if (size == 0)
		return false;
	// make mst and plus mstcount.
	MSTcount++;
	make_set(); // make_set function.get the MSTvertex, parent, many of variable.
	for (int i = 0; i < size; i++) // get size
		parent[i] = i;
	int edge = 0; // edge initialization
	int MSTcost = 10000, node1 = 0, node2 = 0; // initialization three node.

	while (edge < size - 1) // edge is size -1.
	{
		MSTcost = 10000; // biggest number is 9999. so, set the biggest number
		node1 = 0; // set two node is 0.
		node2 = 0;
		for (int i = 0; i < size; i++)
		{
			for (int j = 0; j < size; j++)
			{
				if (find(i) != find(j) && MSTvertex[i][j + 1].GetLocationId() < MSTcost && MSTvertex[i][j + 1].GetLocationId() != 0) // if i,j parent is not same and cost is lower than MSTcost??
				{
						MSTcost = MSTvertex[i][j + 1].GetLocationId(); // change two node, change cost of node.
						node1 = i;
						node2 = j;
				}
			}
		}
		union_set(node1, node2); // union the parent. node1, node2 have same parent.edgecount[edge] = MSTcost; 
		//store it to array
		edgecount[edge] = MSTcost; // edge cost insert
		strcpy(cityone[edge], tempstring[node1]); // edge name insert
		strcpy(citytwo[edge], tempstring[node2]); // edge name insert
		edge++; // edge plus
	}

	return true;
}

//make_set function
void Graph::make_set()
{
	tempstring = new char*[100](); // two demension dynamic allocation start, size is 100, length is 1000
	for (int i = 0; i < size; i++)
		tempstring[i] = new char[1000];
	for (int i = 0; i < size; i++) // copy the city name
		strcpy(tempstring[i], vertex[i][0].Getcountry());

	parent = new int[size + 1](); // parent size dynamic allocation
	edgecount = new int[size + 1](); // edgecount size dynamic allocation
	cityone = new char*[size + 1](); // city size dynamic allocation
	for (int i = 0; i < size + 1; i++)
		cityone[i] = new char[100]();
	citytwo = new char*[size + 1](); // citysize dynamic allocation
	for (int i = 0; i < size + 1; i++)
		citytwo[i] = new char[100]();
	// set four variable. parent, edgecount,cityone,citytwo.

	//set two demension dynamic allocation to store kruscal algorithm
	MSTvertex = new CityData*[100];
	for (int i = 0; i < 100; i++)
		MSTvertex[i] = new CityData[100];

	for (int i = 0; i < size; i++) // copy the locationId of vertex.
		for (int j = 1; j < size + 1; j++)
			MSTvertex[i][j].SetLocationId(vertex[i][j].GetLocationId()); // insert location data
}

//union_set function.6
void Graph::union_set(int x, int y)
{
	// find parent function. 
	int a = find(x);
	int b = find(y);
	//find parent function. if find parent, y's parent become x.
	if (a == b) return;
	parent[b] = a;
}
//find function.
int Graph::find(int x)
{
	// if X is parnet?? find it. return x.
	if (parent[x] == x) {
		return x;
	}
	else {
		// x is child?? find parnet until we find parent.
		return find(parent[x]);
	}
}

