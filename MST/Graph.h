#ifndef GRAPH_H
#define GRAPH_H

#include "Vertex.h"
#include "Edge.h"
#include "MinHeap.h"
#include <fstream>
#include <vector>
#include <iostream>
using namespace std;
#define IN_FINITY 999999
#define visited true
#define unvisited false



class Graph
{
private:
	// the head pointer for the linked list of the vertics
	Vertex* m_pVHead;
	//set variable
	int m_vSize; // vertex size
	int sortresult[100]; // result
	int resultD[100]; // copy result
	int resultarray[2][100]; // array store result
	int before[100]; // before vertex node
	int changed[100]; // changed node
	int tempchanged[100]; // copy changed node
	int washal[100][100]; // washal table node
public:
	int shape[200][200]; // copy the input graph

	Graph();
	~Graph();


	// add vertex function
	void AddVertex(int vertexKey);

	/// add edge from the vertex which the number is startVertexKey to the vertex which the number is endVertexKey
	// add Edge function with three variable
	void AddEdge(int startVertexKey, int endVertexKey, int weight);

	/// get the vertex which the key is vertexNum
	// find vertex function using key
	Vertex* FindVertex(int key);

	/// get the number of the vertics
	//size set function
	int Size(int size) ;

	/// memory free for the vertics
	// memory clear function
	void Clear();

	/// print out the graph as matrix form
	// print shape function
	void Print(std::ofstream& fout);
	// print result function
	void Printgraph(std::ofstream& fout,int vertex);
	/// check whether the graph has negative edge or not.
	// negative edge? return false
	bool IsNegativeEdge();


	/// find the shortest path from startVertexKey to endVertexKey with Dijkstra using std::set
	// Dijkstra Using Set STL function
	std::vector<int> FindShortestPathDijkstraUsingSet(int startVertexKey, int endVertexKey);
	int Dijkstra(int start);

	/// find the shortest path from startVertexKey to endVertexKey with Dijkstra using MinHeap
	// Dijkstra MinHeap function
	std::vector<int> FindShortestPathDijkstraUsingMinHeap(int startVertexKey, int endVertexKey);

	/// find the shortest path from startVertexKey to endVertexKey with Bellman-Ford
	// BellmanFord function
	std::vector<int> FindShortestPathBellmanFord(int startVertexKey, int endVertexKey);

	//set Floyd-washal function
	std::vector<vector<int> > FindShortestPathFloyd();
	//print washal function
	void Printwashal(std::ofstream& fout);

};

#endif
