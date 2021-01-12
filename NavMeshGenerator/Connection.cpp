#include "pch.h"
#include "Connection.h"
#include "utils.h"
#include "Vector2f.h"

Connection::Connection(Node* from, Node* to)
	:Connection{from,to,Color4f{}}
{
}

Connection::Connection(Node* from, Node* to, Color4f color) 
	:m_pFromNode{ from }
	, m_pToNode{ to }
	,m_Color{color}
{
	SetCostToDistance(1.f);
}

void Connection::Draw() const
{
	utils::SetColor(m_Color);
	utils::DrawLine(m_pFromNode->GetPosition(), m_pToNode->GetPosition(),3);
}

Node* Connection::GetOther(Node* pNode) const
{
	if (m_pFromNode == pNode)
	{
		return m_pToNode;
	}
	return m_pFromNode;
}

void Connection::SetCostToDistance(float scalar)
{
	m_pCost = Vector2f{ m_pFromNode->GetPosition(), m_pToNode->GetPosition()}.Length() * scalar;
}
