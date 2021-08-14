#pragma once
#include <vector>


class Obstacle
{
public:
	Obstacle();
	Obstacle(const std::vector<Point2f>& points, const Color4f& color);
	virtual ~Obstacle() = default;

	//------COPY CONSTRUCTORS------
	Obstacle(const Obstacle&) = delete;
	Obstacle(Obstacle&&) = delete;
	//------ASSIGNMENT OPERATORS------
	Obstacle operator=(const Obstacle&) = delete;
	Obstacle& operator=(Obstacle&&) = delete;

	void Draw() const;

	const std::vector<Point2f>& GetPoints() const { return m_Points; };
	const std::vector<Point2f>& GetInsettedPoints() const { return m_InsettedPoints; };

	void SetInsettedPoints(const std::vector<Point2f>& insettedVertices) { m_InsettedPoints = insettedVertices; };
	void PushBackPoint(const Point2f& point);
	void CompleteObstacle();

	void SetColor(const Color4f& color) { m_Color = color; };
	Color4f GetColor() const { return m_Color; };

	bool IsCompleted() const { return m_IsCompleted; }

	void FlipVertexOrder();
	float GetDeterminant();
private:
	std::vector<Point2f> m_Points;
	std::vector<Point2f> m_InsettedPoints;
	Color4f m_Color;
	bool m_IsCompleted;
};
