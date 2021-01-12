#include "pch.h"
#include "Node.h"
#include "utils.h"

Node::Node()
	:Node(Point2f{}, Color4f{})
{
}

Node::Node(const Point2f& pos, const Color4f& color)
	:m_Position{pos}
	,m_Color {color}
{}

void Node::Draw() const
{
	utils::SetColor(m_Color);
	utils::DrawPoint(m_Position, 10.f);
}
