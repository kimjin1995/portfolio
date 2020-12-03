#include "Graph.h"
#include "Stack.h"
#include "MinHeap.h"
#include<string.h>
#include <set>

#define DFS_FIRST_PATH

Graph::Graph()
{
	m_pVHead = NULL;
	m_vSize = 0;
}
Graph::~Graph()
{
	Clear();
}

int Partition(int arr[], int p, int r) // partition option need to be prove result.
{
	int index = r;
	swap(arr[r], arr[index]); // swap two number. Quick sort have to have random pivot. so, arr[p] is being random pivot.
	int x = arr[r]; // set last array.
	int i = p - 1; // it is start to p-1.
	for (int j = p; j < r; j++) // for statement. it start p to r.
	{
		if (arr[j] <= x) // compare two number. if left side number is bigger than right side number?? swap two number.
		{
			i++; // add i, and swap two number, because left sude number must be lower than x
			if (arr[i] != arr[j])
			{
				swap(arr[i], arr[j]);
			}
		}
	}
	i++; // plus i, and swap two array.
	swap(arr[i], arr[r]);
	return i;

}
void insertionsort(int arr[], int p, int r)
{
	int i, j, key = 0;
	for (i = p + 1; i <= r; i++) {
		key = arr[i];// get key
		for (j = i - 1; j >= p && arr[j] > key; j--) // if key is smaller than arr[j]?
			arr[j + 1] = arr[j]; // j+1 = j

		arr[j + 1] = key; // j+1 = key
	}
}
void Quicksort(int arr[], int p, int r) // quick sort function
{
	int q;
	if (p < r) // p must lower than r.
	{
		if (r - p + 1 <= 6) // if there is six node?? selection sort.
		{
			insertionsort(arr, p, r);

		}
		else
		{
			q = Partition(arr, p, r); // partition function. return partition, and recall Quicksort function to recursive.
			Quicksort(arr, p, q - 1);
			Quicksort(arr, q + 1, r); // except arr[q], because arr[q] is criteria.
		}
	}
}


// add vertex function
void Graph::AddVertex(int vertexKey)
{
	Vertex*V = new Vertex(vertexKey); // set new vertex
	Vertex*T = m_pVHead; // get headvertex
	if (m_pVHead == NULL) // if head is null?? head is V.
		m_pVHead = V;
	else // else?
	{
		while (T->GetNext()) // go to the next node until node is null.
		{
			T = T->GetNext();
		}
		T->SetNext(V); // setnext
	}
}

//addEdge function
void Graph::AddEdge(int startVertexKey, int endVertexKey, int weight)
{
	if (weight != 0) // if weight is not 0?? addEdge.
	{
		Vertex *T = m_pVHead; // get vertex
		while (T->GetNext()) // move vertex until vertex is null
			if (T->GetKey() == startVertexKey) // if vertex key is startvertexkey?? break
				break;
			else
				T = T->GetNext(); // else? move
		T->AddEdge(endVertexKey, weight); // addedge
	}
}

//findvertex function
Vertex* Graph::FindVertex(int key)
{
	Vertex * T = m_pVHead; // get headvertex

	while (T->GetNext()) // move vertex until get key
		if (T->GetKey() == key) // find key?? break
			break;
		else // else? move node
			T = T->GetNext();


	return T; // return t.
}

//get size function
int Graph::Size(int size)
{
	m_vSize = size; // set size
	return 0;
}

//memory clear function
void Graph::Clear() 
{
	Vertex * T = m_pVHead,*TB; // get T,TB. both node is headvertex
	Edge*E,*B; // E,B is headedge
	TB = T;
	while (T) // move vertex
	{
		E = T->GetHeadOfEdge(); // get headnode
		while (E) // E is exist??
		{
			B = E;// get edge
			E = E->GetNext(); // mode edget
			delete B; // remove edge
		}
		TB = T; // move vertex
		T = T->GetNext(); // move vertex
		delete TB; // remove vertex
	}
}

//printgraph function
void Graph::Print(std::ofstream& fout)
{
	for (int i = 0; i < m_vSize; i++) // print result
	{
		for (int j = 0; j < m_vSize; j++)
			fout << shape[i][j] << "\t";
		fout << endl;
	}
	return;
}


bool Graph::IsNegativeEdge()
{
	//result is negetive?? return 0.
	return 0;
}


/// find the shortest path from startVertexKey to endVertexKey with Dijkstra using std::set
// FindShortestPathDijkstraUsingSet function
std::vector<int> Graph::FindShortestPathDijkstraUsingSet(int startVertexKey, int endVertexKey)
{
	std::vector<int> v; // set vector
	for (int i = 0; i < m_vSize; i++) // scan all data
		for (int j = 0; j < m_vSize; j++)
			if (shape[i][j] < 0) // if shape is negative value?? return fail.
			{
				v.push_back(0);
				return v;
			}



	memset(sortresult, 0, 100);
	memset(resultD, -1, 100);
	memset(resultarray[0], 0, 100);
	memset(resultarray[1], 0, 100);
	memset(before, -1, 100);
	//memory set

	set<int> D; // set function
	set<int> ::iterator it; //set iterator

	for (int i = 0; i < m_vSize; i++) // insert data
		D.insert(i);

	for (int i = 0; i < 2; i++)
		for (int j = 0; j < m_vSize; j++)
			if (i == 0)
				resultarray[i][j] = j; // key node index
			else
				resultarray[i][j] = IN_FINITY; // infinite, start value is infinity

	for (int i = 0; i < 100; i++) // set before array to 0.
		before[i] = -1;

	resultarray[1][startVertexKey] = 0; // startvertex is 0.



	Vertex* T = new Vertex;
	Vertex* T1 = new Vertex;
	Vertex* T2 = new Vertex;
	Edge * E = new Edge;
	Edge * Temp = new Edge;
	//setvariable, using dynamic allocation

	T1 = FindVertex(startVertexKey); // find vertexnode
	T2 = FindVertex(endVertexKey); // find vertexnode
	if (T1->GetKey() != startVertexKey || T2->GetKey() != endVertexKey) // if there is no vertex??
	{
		v.push_back(2); // error print.
		return v;
	}

	T = FindVertex(startVertexKey); // find vertexnode
	E = T->GetHeadOfEdge(); // get headedge of vertex
	Temp = E;
	for (; Temp; Temp = Temp->GetNext()) // get all of edge
		if (Temp->GetWeight() < resultarray[1][Temp->GetKey()]) // edge length is more smaller than resultarray weight??
		{
			resultarray[1][Temp->GetKey()] = Temp->GetWeight(); // change data, change before node.
			before[Temp->GetKey()] = startVertexKey;
		}
	int move = IN_FINITY; // move variable
	for (int i = 0; i < m_vSize; i++)
		if (resultarray[1][i] < move && i != startVertexKey) // find smallest value, which is move.
			move = i;
		else if (move != IN_FINITY && resultarray[1][i] < resultarray[1][move]) // if move is not infinity and result is more smaller than resultarray?
			move = i;

	D.erase(startVertexKey); // erase vertex array
	while (!D.empty()) // find set STL. if D is null?? break.
	{
		Dijkstra(move); // Dijkstra function
		D.erase(move); // erase function
		move = IN_FINITY; // find move
		for (int i = 0; i < m_vSize; i++) // find smallese value,which is move.
		{
			it = D.find(i); // find value
			if (it != D.end()) // value is exist and value is smallse?? change move node.
				if (move == IN_FINITY)
				{
					move = i;
				}
				else if (resultarray[1][i] < resultarray[1][move])
					move = i;
		}
	}
	int R = resultarray[0][endVertexKey]; // result is R[0][endvertexKey]
	int i = 0;
	while (1)
	{
		if (R != startVertexKey) // if R is not the startVertex?? input the result
		{
			resultD[i++] = R;
		}
		else // R is startVertex?? input and break
		{
			resultD[i++] = R;
			break;
		}
		R = before[R]; // move R before.

	}
	memcpy(sortresult, resultD, 100); // momory copy
	Quicksort(sortresult, 0, i - 1); // sorting

	

	v.push_back(1); // success code
	return v;
}
int Graph::Dijkstra(int start)
{

	Vertex* T = new Vertex;
	Edge * E = new Edge;
	Edge * Temp = new Edge; // three variable
	T = FindVertex(start); // find vertexnode

	E = T->GetHeadOfEdge(); // get headedge of vertex
	Temp = E;
	for (; Temp; Temp = Temp->GetNext()) // if there is smallest edge? change
		if (resultarray[1][start] + Temp->GetWeight() < resultarray[1][Temp->GetKey()])
		{
			//resultarray[1][Temp->GetKey()] < resultarray[1][start] + Temp->GetWeight()? change
			resultarray[1][Temp->GetKey()] = resultarray[1][start] + Temp->GetWeight();
			before[Temp->GetKey()] = start;
		}


	return 0;
}
void Graph::Printgraph(std::ofstream& fout,int vertex) // graph print method
{
	int k = 0;

	for (; resultD[k++] != -1; k); // count result
	fout << "shortest path : ";
	for (int i = k-2; i >= 0; i--)
		fout << resultD[i] << " "; // print resultD

	fout << endl;
	fout << "sorted  nodes : "; // print sort result
	for (int i = 0; i < k-1; i++)
		fout << sortresult[i] << " ";

	fout << endl;
	fout << "path lenght : " << resultarray[1][vertex] << endl; // path result is??

	return;
}

/// find the shortest path from startVertexKey to endVertexKey with Dijkstra using MinHeap
//FindShortestPathDijkstraUsingMinHeap function
std::vector<int> Graph::FindShortestPathDijkstraUsingMinHeap(int startVertexKey, int endVertexKey)
{
	std::vector<int> v; // set vertex
	for (int i = 0; i < m_vSize; i++) // find negative edge. if exist?? break and  failcode return.
		for (int j = 0; j < m_vSize; j++)
			if (shape[i][j] < 0)
			{
				v.push_back(0);
				return v;
			}

	memset(sortresult, 0, 100);
	memset(resultD, -1, 100);
	memset(resultarray[0], 0, 100);
	memset(resultarray[1], 0, 100);
	memset(before, -1, 100);
	// set variable
	std::pair<int, int> x;  // x is pair variable
	MinHeap<int, int> M; // minheap function

	for (int i = 0; i < m_vSize; i++) 
		if (i != startVertexKey) // if not startvertexkey?? input infinity
			M.Push(i, IN_FINITY);
		else // else?? input 0
			M.Push(i, 0);


	Vertex* T = new Vertex;
	Vertex* T1 = new Vertex;
	Vertex* T2 = new Vertex;
	Edge * E = new Edge;
	Edge * Temp = new Edge;
	// set variable
	T1 = FindVertex(startVertexKey); // find vertexnode
	T2 = FindVertex(endVertexKey); // find vertexnode
	if (T1->GetKey() != startVertexKey || T2->GetKey() != endVertexKey) // if there is no vertex??
	{
		// return errorcode
		v.push_back(2);
		return v;
	}

	//initialization
	for (int i = 0; i < m_vSize; i++)
	{
		resultarray[0][i] = i;
		resultarray[1][i] = IN_FINITY;
		before[i] = -1;
	}

	//edge is m_vsize -1
	for(int i = 0; i <m_vSize-1; i++)
	{
		x = M.Top(); // get topnode
		M.Pop(); // popfunction
		T = FindVertex(x.first); // find vertexnode
		if (resultarray[1][before[x.first]] + x.second <= resultarray[1][x.first]) // if resultarray[1][before[x.first]] + x.second <= resultarray[1][x.first]?
		{
			resultarray[1][x.first] = resultarray[1][before[x.first]] + x.second; // change value
			E = T->GetHeadOfEdge(); // get headedge
			Temp = E;

			for (; Temp; Temp = Temp->GetNext()) // get nextedge
					if (resultarray[1][before[Temp->GetKey()]] + Temp->GetWeight() < resultarray[1][Temp->GetKey()])
						//if edge is more shortest than existing value?? change it.
					{
						before[Temp->GetKey()] = x.first; // change beforenode
						resultarray[1][Temp->GetKey()] = resultarray[1][before[Temp->GetKey()]] + Temp->GetWeight();// change value
						M.Push(Temp->GetKey(), Temp->GetWeight()); // update??
					}

		}
	}

	int R = resultarray[0][endVertexKey]; // R is result value.
	int i = 0;
	while (1)
	{
		if (R != startVertexKey) // if r is not startVertexKEy?
		{
			resultD[i++] = R; // print result
		}
		else // else?
		{
			resultD[i++] = R; // print result and break
			break;
		}
		R = before[R]; // R move to before

	}
	memcpy(sortresult, resultD, 100); // memcpy
	Quicksort(sortresult, 0, i - 1); // quick sort result.

	v.push_back(1); // resutn success code
	return v;
}

/// find the shortest path from startVertexKey to endVertexKey with Bellman-Ford
// FindShortestPathBellmanFord function
std::vector<int>Graph::FindShortestPathBellmanFord(int startVertexKey, int endVertexKey)
{
	std::vector<int> v;
	memset(sortresult, 0, 100);
	memset(resultD, -1, 100);
	memset(resultarray[0], 0, 100);
	memset(resultarray[1], 0, 100);
	memset(changed, 0, 100);
	memset(tempchanged, 0, 100);
	memset(before, -1, 100);
	// set varlable using memset



	bool loop; // loop finc variable

	//initialization
	for (int i = 0; i < m_vSize; i++)
	{
		resultarray[0][i] = i;
		resultarray[1][i] = IN_FINITY;
		before[i] = -1;
	}
	//startvalue is 0.
	resultarray[1][startVertexKey] = 0;

	Vertex* T = new Vertex;
	Vertex* T1 = new Vertex;
	Vertex* T2 = new Vertex;
	Edge * E = new Edge;
	Edge * Temp = new Edge;
	//set variable
	T1 = FindVertex(startVertexKey); // find vertexnode
	T2 = FindVertex(endVertexKey); // find vertexnode
	if (T1->GetKey() != startVertexKey || T2->GetKey() != endVertexKey) // if vertex is not exist?? resutrn fail value.
	{
		v.push_back(2);
		return v;
	}

	T = FindVertex(startVertexKey); // get vertex
	E = T->GetHeadOfEdge(); // get headedge
	Temp = E;
	int J = startVertexKey; // get startvertex
	for (int i = 0; i < m_vSize; i++) // bellmanford. minus cycle
	{
		loop = false; // loop is false
		if (J == startVertexKey) // if j is start????
		{
			for (; Temp; Temp = Temp->GetNext()) // read edge
				if (Temp->GetWeight() < resultarray[1][Temp->GetKey()])
				{
					resultarray[1][Temp->GetKey()] = Temp->GetWeight(); // set result
					changed[Temp->GetKey()] = true; // set changenode
					before[Temp->GetKey()] = startVertexKey; // set beforenode
					loop = true; // loop is true
				}
			J = -1; // J is -1.
		}
		else // J is -1.
		{
			for (int z = 0; z < m_vSize; z++) // copy the changed array
				tempchanged[z] = changed[z];
			memset(changed, 0, 100); // memset function
			for (int k = 0; k < m_vSize; k++)
			{
				if (k != startVertexKey && tempchanged[k] == true)// changed vertex and it is not startvertex??
				{
					T = FindVertex(k); // finc vertex
					E = T->GetHeadOfEdge(); // find edge
					Temp = E;
					{
						for (; Temp; Temp = Temp->GetNext()) // find edge
						{
							if (resultarray[1][T->GetKey()] + Temp->GetWeight() < resultarray[1][Temp->GetKey()])
								// if there is exist more smaller path??? change result
							{
								resultarray[1][Temp->GetKey()] = resultarray[1][T->GetKey()] + Temp->GetWeight(); // change result.
								changed[Temp->GetKey()] = true; // vertex is changed.
								before[Temp->GetKey()] = k; // change beforenode.
								loop = true; // loop is true.
							}
						}
					}
				}
			}
		}
	}

	if (loop == true) // loop is true??
	{
		v.push_back(0);
		return v;
	}

	int R = resultarray[0][endVertexKey]; // R is vertex
	int i = 0;
	while (1)
	{
		if (R != startVertexKey) //  R is not startvertexKEy??
		{
			resultD[i++] = R; // print result
		}
		else
		{
			resultD[i++] = R; // print result and break
			break;
		}
		R = before[R]; // move to beforemode

	}
	memcpy(sortresult, resultD, 100); // memcpyfunction
	Quicksort(sortresult, 0, i - 1); // quick sorting

	v.push_back(1); // return success code
	return v;
}

//FindShortestPathFloyd function
std::vector<vector<int> >Graph::FindShortestPathFloyd()
{
	/*memory setting, array size is 100*/
	memset(sortresult, 0, 100);
	memset(resultD, -1, 100);
	memset(resultarray[0], 0, 100);
	memset(resultarray[1], 0, 100);
	memset(before, -1, 100);


	std::vector<vector<int>> v; // set vector
	std::vector<int> q; // set vector
	for (int i = 0; i < 100; i++) // set washal table to 0.
		for (int j = 0; j < 100; j++)
			washal[i][j] = 0;

	for (int i = 0; i < m_vSize; i++) // setting walsh table, copy the result
		for (int j = 0; j < m_vSize; j++)
			if (i != j) // if i != j?? copy result
				if (shape[i][j] != 0) // is ther is value?? copy
					washal[i][j] = shape[i][j];
				else // is value is 0?? set infinity
					washal[i][j] = IN_FINITY;
			else if (i == j) // if i == j?? value is 0.
				washal[i][j] = 0;

	for(int k = 0; k < m_vSize; k++) // walsh table time complexity is n^3
		for (int i = 0; i < m_vSize; i++)
			for (int j = 0; j < m_vSize; j++)
			{
				if(washal[i][j] > washal[i][k] + washal[k][j] && i != j && washal[i][k] != IN_FINITY && washal[k][j] != IN_FINITY) // result is not infinity and there is more smaller result?? change result
				washal[i][j] = washal[i][k] + washal[k][j];
			}

	bool compare = true; // compare function
	int washalcompare[100][100];
	for (int i = 0; i < m_vSize; i++) // copy data to washalcompare array
		for (int j = 0; j < m_vSize; j++)
			washalcompare[i][j] = washal[i][j];

	for (int k = 0; k < m_vSize; k++) // one more cycle
		for (int i = 0; i < m_vSize; i++)
			for (int j = 0; j < m_vSize; j++)
			{
				if (washal[i][j] > washal[i][k] + washal[k][j] && i != j && washal[i][k] != IN_FINITY && washal[k][j] != IN_FINITY) // result is not infinity and there is more smaller result?? change result
					washal[i][j] = washal[i][k] + washal[k][j];
			}

	for (int i = 0; i < m_vSize; i++)
		for (int j = 0; j < m_vSize; j++)
			if (washalcompare[i][j] != washal[i][j]) // compare two array. there is changed result?? cycle detected. return false.
			{
				compare = false;
				break;
			}
	if (compare == true) // cycle detected?? print result.
		q.push_back(1);
	else // else?? print success code.
		q.push_back(0);

	v.push_back(q); // return value

	return v;
}

//printwashal function
void Graph::Printwashal(std::ofstream& fout)
{
	for (int i = 0; i < m_vSize; i++)
	{
		for (int j = 0; j < m_vSize; j++) // print result
		{
			fout << washal[i][j] << "\t";
		}
		fout << endl;
	}
	return;
}
