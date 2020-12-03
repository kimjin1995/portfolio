#include "Manager.h"
#include<iostream>
#include <string>
#include<cstring>
#include <stdlib.h>
#include <fstream>
using namespace std;
#pragma warning(disable:4996)
Manager::~Manager() // file close, open method
{
	if (fout.is_open())
		fout.close();
	if (fin.is_open())
		fin.close();
}

void Manager::Run(const char* filepath)
{
	char *cmd;
	cmd = new char[40];

	fout.open(RESULT_LOG_PATH); // open log file
	if (!fout.is_open()) // if there is no file?
	{
		fout << "CommandFileNotExist" << endl; // commandfile not exist error print
		PrintError(CommandFileNotExist);
		return;
	}
	fin.open(filepath); // open commandfile
	while (!fout.eof()) // get file string, file is not end??
	{
		fin.getline(cmd, 40); // get command
		if (!strcmp(cmd,"")) // command is NULL? break
			break;
		char * one = strtok(cmd, "\r "); // get command

		if (!strcmp(one, "LOAD")) // if command is load?
		{
			char * path; // get path
			path = strtok(NULL, "\r\n "); // get command
			Load(path); // read pathfile
		}
		else if (!strcmp(one, "PRINT")) // command is print?
		{
			Print(); // print data
		}
		else if (!strcmp(one, "DIJKSTRA")) // command is dijkstra?
		{
			char*start = NULL, *end = NULL; // get start and end vertex

			start = strtok(NULL, " "); // strtok function
			end = strtok(NULL, " ");
			if (start == NULL || end == NULL) // if start or end is null?? print error
			{
				fout << "====== DIJKSTRA ======" << endl;
				fout << "VertexKeyNotExist" << endl;
				fout << "=================" << endl;
				fout << "=================" << endl;
				PrintError(VertexKeyNotExist);
				fout << "=================" << endl;
			}
			else
				FindShortestPathDijkstraUsingSet(atoi(start),atoi(end)); // else?? eijkstrausing set function
		}
		else if (!strcmp(one, "DIJKSTRAMIN")) // command is dijkstramin??
		{
			char*start = NULL, *end = NULL; // get start and end vertex

			start = strtok(NULL, " "); // strtok function
			end = strtok(NULL, " ");
			if (start == NULL || end == NULL) // if start or end is null?? print error
			{
				fout << "====== DIJKSTRAMIN ======" << endl;
				fout << "VertexKeyNotExist" << endl;
				fout << "=================" << endl;
				fout << "=================" << endl;
				PrintError(VertexKeyNotExist);
				fout << "=================" << endl;
			}
			else
			FindShortestPathDijkstraUsingMinHeap(atoi(start), atoi(end)); // start shortestpathdijkstrausingminheapfunction
		}
		else if (!strcmp(one, "BELLMANFORD")) // command is bellmanford?
		{
			char*start = NULL, *end = NULL; // get start and end vertex

			start = strtok(NULL, " "); // strtok function
			end = strtok(NULL, " ");
			if (start == NULL || end == NULL) // if start or end is null? print error
			{
				fout << "====== BELLMANFORD ======" << endl;
				fout << "VertexKeyNotExist" << endl;
				fout << "=================" << endl;
				fout << "=================" << endl;
				PrintError(VertexKeyNotExist);
				fout << "=================" << endl;
			}
			else
			FindShortestPathBellmanFord(atoi(start), atoi(end)); // findshortestpathbellmanford function start
		}
		else if (!strcmp(one, "FLOYD")) // command is floyd function?
		{
			FindShortestPathFloyd(); // floyd function start
		}
		else // command is unknown??
		{
			fout << "======"<< cmd <<"======" << endl; // print result
			fout << "NonDefinedCommand" << endl; // not define
			fout << "=================" << endl;
			fout << "=================" << endl;
			PrintError(NonDefinedCommand);
			fout << "=================" << endl;
		}

		fout << endl;
	}

	delete cmd;
}
void Manager::PrintError(Result result) // printerror function
{
	fout <<"Error code: " << result << std::endl; // print errorcode
}
Result Manager::Load(const char* filepath)
{

	ifstream read(filepath); // read file
	if (!read.is_open()) // file open is fail? print error
	{
		fout << "====== LOAD ======" << endl;
		fout << "LoadFileNotExist" << endl;
		fout << "=================" << endl;
		PrintError(LoadFileNotExist);
		return Success;
	}


	char input[200]; // set variable
	int readcount = 0;


	while (!read.eof()) // read file
	{
		read.getline(input, 200); // read data
		if (readcount == 0) // if first line?? node count. set size
		{
			nodecount =  atoi(input);
			m_graph.Size(nodecount);
		}

		char* token; // token function
		if (readcount != 0) 
		{
			for (int i = 0; i < nodecount; i++) // read all data
			{
				if (i == 0) // if i is 0? number else? space. use strtok.
					token = strtok(input, " ");
				else
					token = strtok(NULL, " ");

				if (token != NULL)
				{
					m_graph.shape[readcount - 1][i] = atoi(token); // fill the number into the shape array
				}
			}
		}
		readcount++; // readcount++
	}
	
	for(int i = 0; i < nodecount; i++) // vertex add function
			m_graph.AddVertex(i);

	for (int i = 0; i < nodecount; i++) // edge add function
		for (int j = 0; j < nodecount; j++)
			if(m_graph.shape[i][j] != 0) // if graph have a edge?? add edge
			m_graph.AddEdge(i,j,m_graph.shape[i][j]);

	fout << "====== LOAD ======" << endl; // print result
	fout << "Success" << endl;
	fout << "=================" << endl;

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;

	return Success;
}

Result Manager::Print() // print function
{
	fout << "====== PRINT ======" << endl;
	if (nodecount == 0) // if nodecount is 0?? do not start LOAD function. print error
	{
		fout << "GraphNotExist" << endl;
		fout << "=================" << endl;
		PrintError(GraphNotExist);
		return Success;
	}
	m_graph.Print(fout); // print array
	fout << "=================" << endl; // print result

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;
	return Success;
}


Result Manager::FindShortestPathDijkstraUsingSet(int startVertexKey, int endVertexKey) // Dijkstra function, using set
{

	fout << "====== DIJKSTRA ======" << endl;

	std::vector<int> v; // set vector

	v = m_graph.FindShortestPathDijkstraUsingSet(startVertexKey, endVertexKey); // start dijkstrausingset,and get the result input the vector v.
	if (v.at(0) == 0) // if code is 0?? invalid algorithm.
	{
		fout << "InvalidAlgorithm" << endl;
		fout << "=================" << endl;
		PrintError(InvalidAlgorithm);
		return Success;
	}
	else if (v.at(0) == 2) // if code is 2?? invalid vertex key.
	{
		fout << "InvalidVertexKey" << endl;
		fout << "=================" << endl;

		fout << "=================" << endl;
		PrintError(InvalidVertexKey);
		fout << "=================" << endl;
		return Success;
	}
	m_graph.Printgraph(fout,endVertexKey); // else?? success. print graph
	fout << "=================" << endl;

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;
	return Success;
}

Result Manager::FindShortestPathDijkstraUsingMinHeap(int startVertexKey, int endVertexKey) // dijkstra heap function
{

	fout << "====== DIJKSTRAMIN ======" << endl;

	std::vector<int> v; // set vector v.
	v = m_graph.FindShortestPathDijkstraUsingMinHeap(startVertexKey, endVertexKey); // start dijkstrausingset,and get the result input the vector v.

	if(v.at(0) == 0) // if code is 0?? invalidalgorithm error
	{
		fout << "InvalidAlgorithm" << endl;
		fout << "=================" << endl;
		PrintError(InvalidAlgorithm);
		return Success;
	}
	else if (v.at(0) == 2) // if code is 2? invalidvertexkey error
	{
		fout << "InvalidVertexKey" << endl;
		fout << "=================" << endl;

		fout << "=================" << endl;
		PrintError(InvalidVertexKey);
		fout << "=================" << endl;
		return Success;
	}
	m_graph.Printgraph(fout,endVertexKey);//else? success. print result
	fout << "=================" << endl;

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;
	return Success;
}

Result Manager::FindShortestPathBellmanFord(int startVertexKey, int endVertexKey) // bellmanford function
{
	fout << "====== BELLMANFORD ======" << endl;
	// TODO: implement
	std::vector<int> v; // set vector v
	v = m_graph.FindShortestPathBellmanFord(startVertexKey, endVertexKey); // start dijkstrausingset,and get the result input the vector v.
	if (v.at(0) == 0) // if code is 0? negativecycledetected function
	{
		fout << "NegativeCycleDetected" << endl;
		fout << "=================" << endl;
		PrintError(NegativeCycleDetected);
		return Success;
	}
	else if (v.at(0) == 2) // if code is 2? invalidvertexKey error
	{
		fout << "InvalidVertexKey" << endl;
		fout << "=================" << endl;

		fout << "=================" << endl;
		PrintError(InvalidVertexKey);
		fout << "=================" << endl;
		return Success;
	}
	else
	m_graph.Printgraph(fout, endVertexKey); // else? success. print result

	fout << "=================" << endl;

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;
	return Success;

}
Result Manager::FindShortestPathFloyd() // floyd function
{
	std::vector<vector<int>> v; // set vector
	std::vector<int> q; // set vector
	fout << "====== FLOYD ======" << endl;
	v = m_graph.FindShortestPathFloyd(); // start dijkstrausingset,and get the result input the vector v.
	q = v.at(0); // q is v.
	if (q.at(0) == 0) // if code is 0? negatrivecycledetected error.
	{
		fout << "NegativeCycleDetected" << endl;
		fout << "=================" << endl;
		fout << "=================" << endl;
		PrintError(NegativeCycleDetected);
		fout << "=================" << endl;
		return Success;
	}
	else
		m_graph.Printwashal(fout); // else? print result.

	fout << "=================" << endl;

	fout << "=================" << endl;
	PrintError(Success);
	fout << "=================" << endl;
	return Success;


}
