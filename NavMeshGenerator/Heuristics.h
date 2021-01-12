#pragma once
#include "Node.h"
namespace Heuristics
{
	float GetDistance(Node* lhs, Node* rhs)
	{
		return Vector2f{ lhs->GetPosition(), rhs->GetPosition() }.Length();
	}
}