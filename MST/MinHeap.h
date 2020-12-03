#ifndef MIN_HEAP_H
#define MIN_HEAP_H

#include <utility>
#include <vector>
#include<string>
using namespace std;
template<typename TKey, typename TValue>
class MinHeap
{
private:
	// array for the elements which should be heap-sorted
	std::vector<std::pair<TKey, TValue>> m_vec;
public:
	MinHeap() {}

	// push function, push the node and make_pair function, heap sorting
	void Push(TKey key, TValue value)
	{
		m_vec.push_back(make_pair(key,value));
		Heapify(m_vec.size());
	}
	// pop function, first node pop and swap function, heap sorting
	void Pop()
	{
		m_vec.at(0).swap(m_vec.back());
		m_vec.pop_back();
		Heapify(m_vec.size());
	}
	// get top node
	std::pair<TKey, TValue> Top()
	{
		return m_vec.at(0);
	}
	// check heap is empty?
	bool IsEmpty()
	{
		if (m_vec.empty())
			return true;
		else
			false;
	}

private:
	// heap sort
	void Heapify(int index)
	{
		std::vector<std::pair<int, int>> temp; // get vertex.
		temp = m_vec; // set temp is root
		for (int i = 1; i < index; i++) // start node is 1. parent is 0.
		{
			int child = i; // set child is i.
			while (child > 0)
			{
				int change = child / 2; // get parent node
				if (temp.at(change).second > temp.at(child).second) // if child is larger than parent?? swap data
					temp.at(change).swap(temp.at(child));
				child = change;
			}
		}
		m_vec = temp; // set rootnode
	}
};

#endif
