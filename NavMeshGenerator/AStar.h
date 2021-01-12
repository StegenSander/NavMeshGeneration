#pragma once
#include <vector>
class Graph;
class Node;
class Connection;

struct NodeRecord
{
	Node* pNode;
	Connection* pConnection;
	float currentCost;
	float estimatedCost;
	bool operator< (const NodeRecord& other) const
	{
		return currentCost + estimatedCost < other.currentCost + other.estimatedCost;
	}
	bool operator== (const NodeRecord& other) const
	{
		return pNode == other.pNode && pConnection == other.pConnection;
	}
};


class AStar
{
public:
	AStar() = default;
	AStar(Graph* pGraph);
	~AStar() = default;

	std::vector<Node*> FindPath(Node* startNode, Node* endNode);

	AStar& SetGraph(Graph* graph) {
		m_pGraph = graph;
		return *this;
	}
private:
	Graph* m_pGraph{ nullptr };
};

