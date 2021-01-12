#include "pch.h"
#include "Graph.h"
#include <iostream>

Graph::Graph()
{
}

Graph::~Graph()
{
	for (Node*& pNode : m_Nodes)
	{
		if (pNode) delete pNode;
	}

	for (Connection*& pConnection : m_Connections)
	{
		if (pConnection) delete pConnection;
	}
}

void Graph::Draw(bool drawNode, bool drawConnections) const
{
	if (drawNode)
	{
		for (Node* const& pNode : m_Nodes)
		{
			pNode->Draw();
		}
	}

	if (drawConnections)
	{
		for (Connection* const& pConnection : m_Connections)
		{
			pConnection->Draw();
		}
	}
}

void Graph::AddNode(Node* pNode)
{
	m_Nodes.push_back(pNode);
}

Node* Graph::AddNode(const Point2f& pos, const Color4f& color)
{
	Node* pNode = new Node{ pos,color };
	AddNode(pNode);
	return pNode;
}

void Graph::AddConnection(Connection* pConnection)
{
	m_Connections.push_back(pConnection);
}

Connection* Graph::AddConnection(Node* from, Node* to, Color4f color)
{
	Connection* pConnection = new Connection(from, to, color);
	AddConnection(pConnection);
	return pConnection;
}

Node* Graph::GetNodeInRange(const Point2f& pos, float maxDistance)
{
	auto pNode = std::find_if(m_Nodes.begin(), m_Nodes.end(),
		[pos, maxDistance](Node* pNode)
		{
			return (pNode->GetPosition() - pos).SquaredLength() < (maxDistance*maxDistance);
		});

	if (pNode == m_Nodes.end())
	{
		return nullptr;
	}

	return *pNode;
}

void Graph::RemoveNode(Node* pNode)
{
	std::_Erase_remove_if(m_Nodes, [pNode](Node* node){
	if (node == pNode)
	{
		delete node;
		return true;
	}
	return false; });

	std::_Erase_remove_if(m_Connections, [pNode](Connection* connection) {
		if (connection->GetFrom() == pNode || connection->GetTo() == pNode)
		{
			delete connection;
			return true;
		}
		return false;
		});
}

void Graph::SetColorOFAllNode(const Color4f& color)
{
	for (Node* pNode : m_Nodes)
	{
		pNode->SetColor(color);
	}
}

std::vector<Connection*> Graph::GetConnectionsOfNode(Node* pNode) const
{
	std::vector<Connection*> result{};

	auto It = m_Connections.begin();
	while (It != m_Connections.end())
	{
		It = std::find_if(It, m_Connections.end(), [pNode](Connection* connection) {
			return connection->GetFrom() == pNode || connection->GetTo() == pNode;
			});

		if (It != m_Connections.end())
		{
			result.push_back(*It);
			++It;
		}

	}


	return result;
}

void Graph::ChangeNode(Node* targetNode, Node* newNode)
{
	std::vector<Node*>::iterator nodeIt{ std::find(m_Nodes.begin(), m_Nodes.end(), targetNode) };
	while (nodeIt != m_Nodes.end())
	{
		m_Nodes.erase(nodeIt);
		nodeIt = std::find(m_Nodes.begin(), m_Nodes.end(), targetNode);
	}

	std::vector<Connection*> connections = GetConnectionsOfNode(targetNode);
	for (Connection* pConnection : connections)
	{
		if (pConnection->GetFrom() == targetNode)pConnection->SetFrom(newNode);
		if (pConnection->GetTo() == targetNode) pConnection->SetTo(newNode);
	}

}
