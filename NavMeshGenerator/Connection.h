#pragma once
#include "Node.h"
class Connection
{
public:	
	Connection(Node* from, Node* to);
	Connection(Node* from, Node* to, Color4f color);
	virtual ~Connection() = default;

	void Draw() const;

	void SetColor(const Color4f& color) { m_Color = color; };
	Color4f GetColor() const { return m_Color; };

	Node* GetFrom() const { return m_pFromNode; };
	void SetFrom(Node* pNode) { m_pFromNode = pNode; }
	Node* GetTo() const { return m_pToNode; };
	void SetTo(Node* pNode) { m_pToNode = pNode; }
	Node* GetOther(Node* pNode) const;

	float GetCost() const{ return m_pCost; }
	void SetCostToDistance(float scalar);

private:
	Color4f m_Color;
	Node* m_pFromNode;
	Node* m_pToNode;

	float m_pCost;
};

