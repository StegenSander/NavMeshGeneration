#include "pch.h"
#include "Obstacle.h"
#include "utils.h"
#include <cassert>

Obstacle::Obstacle()
	:m_Points{}
	, m_Color{ Color4f{1.f,0.f,0.f,1.f} }
	,m_IsCompleted {false}
{
}

Obstacle::Obstacle(const std::vector<Point2f>& points, const Color4f& color)
	:m_Points{points}
	,m_Color {color}
	,m_IsCompleted {true}
{
}

void Obstacle::Draw() const
{
	utils::SetColor(m_Color);
	//utils::FillPolygon(m_Points);
	utils::DrawPolygon(m_Points,4.f);

	utils::DrawPoints(m_Points.data(), m_Points.size(),10.f);
}

void Obstacle::PushBackPoint(const Point2f& point)
{
	
	if (m_Points.size() > 0)
	{
		if (Vector2f{ point - m_Points[0] }.Length() < 10.f)
		{
			CompleteObstacle();
			return;
		}
	}
	m_Points.push_back(point);
}

void Obstacle::CompleteObstacle()
{
	m_IsCompleted = true;
	if (GetDeterminant() > 0)
	{
		FlipVertexOrder();
	}
}

void Obstacle::FlipVertexOrder()
{
	std::reverse(m_Points.begin(), m_Points.end());
}

//https://en.wikipedia.org/wiki/Curve_orientation
float Obstacle::GetDeterminant()
{
	if (m_Points.size() < 3) return 0;
	
	float determinant{};
	int countNegative{};
	int countPositive{};
	for (size_t i = 0; i < m_Points.size()-2; i++)
	{
		Point2f A = m_Points[i];
		Point2f B = m_Points[i+1];
		Point2f C = m_Points[i+2];

		float f1 = B.x * C.y + A.x * B.y + C.x * A.y;
		float f2 = A.y * B.x + B.y * C.x + C.y * A.x;
		determinant = f1 - f2;
		if (determinant < 0) countNegative++;
		if (determinant > 0) countPositive++;
	}
	if (determinant < 0 && countPositive > countNegative)
	{
		return -determinant;
	}
	if (determinant > 0 && countPositive < countNegative)
	{
		return -determinant;
	}

	return determinant;
}
