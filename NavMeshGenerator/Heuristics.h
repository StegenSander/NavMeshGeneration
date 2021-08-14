#pragma once
#include "Node.h"
namespace Heuristics
{
	inline float GetDistance(Node* lhs, Node* rhs)
	{
		return Vector2f{ lhs->GetPosition(), rhs->GetPosition() }.Length();
	}
}