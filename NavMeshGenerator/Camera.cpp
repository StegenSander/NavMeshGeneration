#include "pch.h"
#include "Camera.h"

Camera::Camera(Point2f pos)
	:m_InitialPosition{pos}
	,m_Position {pos}
{
}

Camera::~Camera()
{
}

void Camera::ResetCameraPos()
{
	m_Position = m_InitialPosition;
}

void Camera::MoveCamera(Vector2f amount, bool mustMultiplySpeed)
{
	if (mustMultiplySpeed)
	{
		amount *= m_Speed;
	}

	m_Position += amount;
}
