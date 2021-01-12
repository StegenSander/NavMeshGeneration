#pragma once
#include "Connection.h"

#include <list>
#include <vector>

struct NodeConnections
{
	Node* pNode;
	std::list<Connection*> connectionList;
};

class Graph
{
public:
	Graph();
	~Graph();

	void Draw(bool drawNode, bool drawConnections) const;

	void AddNode(Node* pNode);
	Node* AddNode(const Point2f& pos, const Color4f& color = {});

	void AddConnection(Connection* pConnection);
	Connection* AddConnection(Node* from, Node* to, Color4f color = {});

	Node* GetNodeInRange(const Point2f& pos, float maxDistance = 15.f);
	void RemoveNode(Node* pNode);

	void SetColorOFAllNode(const Color4f& color);

	std::vector<Connection*> GetConnectionsOfNode(Node* pNode) const;

	//Functions will not delete the Node
	void ChangeNode(Node* targetNode, Node* newNode);

private:
	std::vector<Node*> m_Nodes;
	std::vector<Connection*> m_Connections;
};

