#ifndef EDGE_H
#define EDGE_H

#ifndef NULL
#define NULL	0
#endif

class Edge
{
private:
	// the key of this edge
	int m_key;
	// the weight of this edge
	int m_weight;
	// the next pointer for the linked list of the edges
	Edge* m_pNext;

public:
	// constructor. key,weight is 0, next node is null
	Edge()
	{
		m_key = 0;
		m_weight = 0;
		m_pNext = NULL;
	}

	// copy constructor, if get key, weight, make node with variable
	Edge(int key, int weight)
	{
		m_key = key;
		m_weight = weight;
	}

	//set next function
	void SetNext(Edge* pNext)
	{
		m_pNext = pNext;
	}

	//get key function
	int GetKey() const
	{
		return m_key;
	}

	//get weight function
	int GetWeight() const
	{
		return m_weight;
	}

	//get next function
	Edge* GetNext() const
	{
		return m_pNext;
	}
};

#endif
