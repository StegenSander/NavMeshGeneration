#pragma once
#include "Vector2f.h"
class Camera
{
public:
	Camera(Point2f pos);
	virtual ~Camera();

	void ResetCameraPos();
	void MoveCamera(Vector2f amount,bool mustMultiplySpeed);

	Point2f GetPosition() const { return m_Position; };
private:
	Point2f m_InitialPosition;
	Point2f m_Position;

	float m_Speed{ 100 };
};

