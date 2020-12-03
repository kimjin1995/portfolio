#define _CRT_SECURE_NO_WARNINGS
#ifndef SOLUTION_H
#define SOLUTION_H

#include "Result.h"
#include "Graph.h"
#include <fstream>

class Manager
{
private:
	// the filepath for the result log
	const char* RESULT_LOG_PATH = "log.txt";
	const char* RESULT_OPEN_PATH = "command.txt";//open path
	// the filepath for the error log


	// the file stream for the result log
	std::ofstream fout;
	// the file stream for the error log
	std::ifstream fin;
	// graph instance to manage the vertics.
	Graph m_graph;

public:
	int nodecount=0; // set nodecount

	~Manager();
	void Run(const char* filepath); // command start
	void PrintError(Result result); // error print

private:
	Result Load(const char* filepath); // Load function
	Result Print(); // Print function
	Result FindShortestPathDijkstraUsingSet(int startVertexKey, int endVertexKey); // Dijkstra funtion
	Result FindShortestPathDijkstraUsingMinHeap(int startVertexKey, int endVertexKey); // MinHeap function
	Result FindShortestPathBellmanFord(int startVertexKey, int endVertexKey); // BellmanFord function
	Result FindShortestPathFloyd(); // Floyd function
};

#endif
