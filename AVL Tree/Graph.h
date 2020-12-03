#include "AVLTree.h"
#include <iostream>
#include <fstream>// header file declare
#include <map>
using namespace std;
class Graph
{
public:
	int *edgecount; // set four variable
	char **cityone, **citytwo;
	char **tempstring;
	map<int, CityData *> * mList;	// mList[from vetex] = map<to vertex, weigth>
	CityData ** vertex;			// vetex[index] = CityData * 
	CityData ** MSTvertex;
	int graphcount = 0;
	int MSTcount = 0; // set mstcount
	int size = 0;					// AVLTree size
	int *parent; // set parent to compute union
	ofstream * flog; // file output 

public:
	Graph(ofstream * flog);
	~Graph();

	bool	Build(AVLTree * root); // build function
	bool 	Print_GP(); // print_gp function
	bool	Print_MST(); // print_mst function

	void	SetList(AVLNode *root); // set lieft function.  get the node information.

	/*for Kruskal union_set*/
	bool	Kruskal(); // kruskal algorithm
	void 	make_set(); // make_set function
	void 	union_set(int x, int y); // union_set function
	int		find(int x); // find function

};


