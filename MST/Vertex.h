#ifndef VERTEX_H
#define VERTEX_H

#ifndef NULL
#define NULL	0
#endif

#include "Edge.h"

class Vertex
{
private:
	// the key of this vertex
	int m_key;
	// the number of the edges from this vertex to others
	int m_size;
	// the head pointer for the linked list of the edges
	Edge* m_pEHead;
	// the next pointer for the linked list of the vertics
	Vertex* m_pNext;

public:
	Vertex() // constructor
	{
	}
	Vertex(int key) // copy constructor set four value
	{
		m_pEHead = NULL;
		m_size = 0;
		m_pNext = NULL;
		m_key = key;
	}
	~Vertex()
	{}
	//set next function
	void SetNext(Vertex* pNext)
	{
		m_pNext = pNext;
	}
	//get key function
	int GetKey() const
	{
		return m_key;
	}
	//get next function
	Vertex* GetNext() const
	{
		return m_pNext;
	}
	//size function
	int Size() const
	{
		return m_size;
	}
	//add edge function, two variable
	void AddEdge(int edgeKey, int weight)
	{
		Edge *Enew = new Edge(edgeKey,weight); // set edge
		Edge*E = new Edge; // startedge
		E = m_pEHead; // edge = head
		if (m_pEHead == NULL) // head edge is null?? new edge is head
			m_pEHead = Enew;
		else
		{
			while (E->GetNext()) // go to the next node until next node is null.
				E = E->GetNext();
			E->SetNext(Enew); // set next node is new node
		}
		m_size++; // node size increase

	}
	//get headnode
	Edge* GetHeadOfEdge() const
	{
		return m_pEHead;
	}

	void Clear()
	{}
};

#endif
