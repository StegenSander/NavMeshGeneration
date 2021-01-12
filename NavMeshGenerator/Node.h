#pragma once
#include "Vector2f.h"
class Node
{
public:
	Node();
	Node(const Point2f& pos, const Color4f& color);
	virtual ~Node() = default;

	void Draw() const;

	void SetPosition(const Point2f& pos) { m_Position = pos; };
	Point2f GetPosition() const { return m_Position; };

	void SetColor(const Color4f& color) { m_Color = color; };
	Color4f GetColor() const { return m_Color; };
private:
	Point2f m_Position;
	Color4f m_Color;
};

