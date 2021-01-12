#include "pch.h"
#include "NavigationMesh.h"
#include <algorithm>
#include <iostream>
#include "utils.h"

NavigationMesh::NavigationMesh(const Rectf& boundaries)
	:m_Boundaries(boundaries)
	,m_NavMesh(new Graph())
{

}

NavigationMesh::~NavigationMesh()
{
	if (m_NavMesh) delete m_NavMesh;
}

void NavigationMesh::CleanUpNavMesh()
{
	m_Vertices.clear();
	m_pNodes.clear();
	m_Triangles.clear();
	if (m_NavMesh) delete m_NavMesh;
	m_NavMesh = new Graph();
}

void NavigationMesh::SetObstacles(const std::vector<Obstacle*> obstacles)
{
	m_Obstacles = obstacles;
	for (Obstacle* pObstalce : obstacles)
	{
		pObstalce->SetInsettedPoints(InsetVertices(pObstalce->GetPoints()));
	}
}


void NavigationMesh::Draw()
{
	utils::SetColor(Color4f{ 0.f,0.6f,0.f,0.5f });
	utils::DrawPoints(m_Vertices.data(), m_Vertices.size(),10.f);

	m_NavMesh->Draw(true,true);

	utils::SetColor(Color4f{ 0.f,0.6f,0.f,0.2f });
	for (const Triangle& triangle : m_Triangles)
	{
		utils::FillPolygon(triangle);
	}
}

void NavigationMesh::CalculateNavMesh()
{
	CleanUpNavMesh(); //Clean up previous data

	InitializeVertices(); //Push the boundaries as vertices

	//Inset

	SortObstacles(); //Sort obstacles based on there x value
	HandleInvalidObstacles(); //Remove obstacles that will cause a crash

	std::cout << "\n---------Inserting Obstacle--------\n";
	for (Obstacle* pObstacle : m_Obstacles)
	{
		InsertObstacleVertices(pObstacle);
	}
	std::cout << "---------End of inserting obstacle--------\n";

	CreateNodesAndGraph();

	StartEarClipping();




}

void NavigationMesh::InitializeVertices()
{
	m_Vertices.push_back(Point2f{m_Boundaries.left + m_Boundaries.width, m_Boundaries.bottom});
	m_Vertices.push_back(Point2f{ m_Boundaries.left + m_Boundaries.width, m_Boundaries.bottom + m_Boundaries.height });
	m_Vertices.push_back(Point2f{ m_Boundaries.left + m_Boundaries.width - m_Boundaries.width, m_Boundaries.bottom + m_Boundaries.height });
	m_Vertices.push_back(Point2f{ m_Boundaries.left, m_Boundaries.bottom + m_Boundaries.height - m_Boundaries.height });
}

std::vector<Point2f> NavigationMesh::InsetVertices(const std::vector<Point2f>& vertices)
{
	if (!m_MustInsertVertices) return vertices;

	//Make a copy, since the original may be altered
	std::vector<Point2f> insettedVertices{};
	std::copy(vertices.cbegin(), vertices.cend(),std::back_inserter(insettedVertices));

	int indexAdder{};
	for (int i = 0; i < vertices.size(); i++)
	{
		//Get the Necesarry Indices
		int prevIndex{ i - 1 };
		if (prevIndex < 0) prevIndex += vertices.size();
		int nextIndex{ (i + 1) % int(vertices.size()) };

		//Get The necesarry Vectors/Normals
		Vector2f prevVector = Vector2f{ vertices[prevIndex],vertices[i] }.Normalized();
		Vector2f prevNormal = prevVector.Orthogonal().Normalized();

		Vector2f nextVector = Vector2f{ vertices[i],vertices[nextIndex] }.Normalized();
		Vector2f nextNormal = nextVector.Orthogonal().Normalized();


		Vector2f totalNormal{ };
		float cross = prevVector.CrossProduct(nextVector);
		float dot = prevVector.DotProduct(nextVector);


		//max To One Total Normal
		totalNormal = nextNormal + prevNormal;
		if (abs(totalNormal.x) > 1.f) totalNormal.x = utils::sign(totalNormal.x) * 1.f;
		if (abs(totalNormal.y) > 1.f) totalNormal.y = utils::sign(totalNormal.y) * 1.f;


		//Handle 3 Different Cases when Insetting
		if (cross >= 0)
		{
			//Case1: Cross > 0 --> Angle < 180Deg
			insettedVertices[i + indexAdder] = vertices[i] + m_AgentRadius * totalNormal;
		}
		else
		{
			//Case2: cross < 0 --> Angle >180 Deg

			if (abs(dot) < 0.0001f) //dot == 0 means right angle easy to handle
			{
				insettedVertices[i + indexAdder] = vertices[i] + m_AgentRadius * totalNormal;
			}
			else //Sharp angles can return in very stretched navmeshes, we prevent this by adding and extra vertex
			{
				if (dot < 0.f)
				{
				//Recalculate the distance to the obstacle
				//Adjust the original point along the sides
				//Add an extra point 
				//float radius = sqrt(2 * pow(m_AgentRadius, 2)); 
					Point2f point1 = vertices[i] + m_AgentRadius * prevVector + m_AgentRadius * prevNormal;
					Point2f point2 = vertices[i] + m_AgentRadius * (-nextVector) + m_AgentRadius * nextNormal;

					if (Vector2f{ point2 - point1 }.SquaredLength() > pow(m_AgentRadius, 2.f))
					{
						insettedVertices[i + indexAdder] = point1;
						insettedVertices.insert(insettedVertices.begin() + i + indexAdder + 1, point2);
						indexAdder++;
					}
					else
					{
						insettedVertices[i + indexAdder] = Point2f{ (point1.x + point2.x) / 2, (point1.y + point2.y) / 2 };
					}
				}
				else
				{
					insettedVertices[i + indexAdder] = vertices[i] + m_AgentRadius * totalNormal;
				}
				
			}
		}
	}
	return insettedVertices;
}

void NavigationMesh::SortObstacles()
{
	std::sort(m_Obstacles.begin(), m_Obstacles.end(), [](Obstacle* lhs, Obstacle* rhs)
		{
			Point2f biggestLhs = *std::max_element(lhs->GetInsettedPoints().begin(), lhs->GetInsettedPoints().end(), [](const Point2f& lhs, const Point2f& rhs)
				{
					return lhs.x < rhs.x;
				}
			);

			Point2f biggestRhs = *std::max_element(rhs->GetInsettedPoints().begin(), rhs->GetInsettedPoints().end(), [](const Point2f& lhs, const Point2f& rhs)
				{
					return lhs.x < rhs.x;
				}
			);

			return biggestLhs.x > biggestRhs.x;
		});
}

void NavigationMesh::HandleInvalidObstacles()
{
	std::vector<Obstacle*> invalidObstacle{};

	std::cout << "\n---------Checking Invalid Obstacles--------\n";


	for (Obstacle* pObstacle : m_Obstacles)
	{

		const std::vector<Point2f>& obstaclePoints = pObstacle->GetInsettedPoints();
		if (obstaclePoints.size() < 3)
		{
			invalidObstacle.push_back(pObstacle);
			std::cout << "INVALID OBSTACLE: Not enough vertices\n";
			goto nextObstacle;
		}

		for (const Point2f& point : obstaclePoints)
		{
			if (!utils::IsPointInRect(point, m_Boundaries))
			{
				invalidObstacle.push_back(pObstacle);
				std::cout << "INVALID OBSTACLE: Not in level boundaries\n";
				goto nextObstacle;
			}

			for (Obstacle* pObstacle2 : m_Obstacles)
			{
				if (pObstacle != pObstacle2)
				{
					const std::vector<Point2f>& obstacle2Points = pObstacle2->GetInsettedPoints();

					if (utils::IsPointInPolygon(point, obstacle2Points))
					{
						invalidObstacle.push_back(pObstacle);
						std::cout << "INVALID OBSTACLE: Overlapping with another polygon\n";
						goto nextObstacle;
					}
				}
			}
		}

nextObstacle:;
	}

	std::cout << invalidObstacle.size() << " Invalid obstacles being removed\n";
	for (Obstacle* pObstacle : invalidObstacle)
	{
		m_Obstacles.erase(std::find(m_Obstacles.begin(), m_Obstacles.end(), pObstacle));
	}

	std::cout << "---------End of Checking Invalid--------\n\n";

}

void NavigationMesh::InsertObstacleVertices(Obstacle* pObstacle)
{
	std::vector<Point2f> obstacleVertices = pObstacle->GetInsettedPoints();

	//Find the most right point
	auto mostRightPointIt = std::max_element(obstacleVertices.begin(), obstacleVertices.end(), [](const Point2f& lhs, const Point2f& rhs)
		{
			return lhs.x < rhs.x;
		}
	);

	utils::HitInfo hitInfo{};
	//Raycast to the right and check where it hits the already existing vertices
	//Raycast should always hit due the the cleaning up of invalid Obstacles
	if (!utils::Raycast(m_Vertices, *mostRightPointIt, (*mostRightPointIt) + Vector2f{ m_Boundaries.width,0 }, hitInfo))
	{
		std::cout << "Polygon not in NavMesh, Skipping polygon\n";
		return;
	}

	//Now we have to point which we want to connect our obstacle to
	Point2f connectToPos = hitInfo.firstPointOfIntersectedLine;

	//We want to make sure that our connection is not obstructed, so check if we can reach the point
	utils::HitInfo hitInfo2{};
	bool visible{ false };
	while (!visible)
	{
		if (utils::Raycast(m_Vertices, *mostRightPointIt, connectToPos, hitInfo2))
		{
			if (hitInfo2.intersectPoint == connectToPos) //If we intersect with the point the point is visible
			{
				visible = true; //We found a visible point exit loop
			}
			else //If we intersect with something else, we're going to attach ourselves to that point
			{
				hitInfo = hitInfo2;
				connectToPos = hitInfo.secondPointOfIntersectedLine;
				std::cout << "Polygon can't directly connect to target, finding new connection point\n";
			}
		}
		else
		{
			visible = true;
		}
	}


	//We found the point we want to connect to
	//It is possible that the point in multiple times already in the vertices vector
	//So now find the one we actually wan to connect to

	//We create a vector of all the Locations where this point is part of our vector
	std::vector<std::vector<Point2f>::iterator> pointIterators{}; 
	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		if (m_Vertices[i] == connectToPos)
		{
			pointIterators.push_back(m_Vertices.begin() + i);
		}
	}

	//Once the next point has a bigger y than our point, we want to connect
	std::vector<Point2f>::iterator connectToIt{};
	auto nodeItIt = std::find_if(pointIterators.begin(), pointIterators.end(), 
		[mostRightPointIt](const std::vector<Point2f>::iterator& nodeIt)
		{
			return (*(nodeIt + 1)).y > (*mostRightPointIt).y;
		});

	if (nodeItIt == pointIterators.end()) connectToIt = pointIterators.back();
	else connectToIt = *nodeItIt;

	//Connect the obstacle
		//Shift the element to the right point is first
	std::rotate(obstacleVertices.begin(), mostRightPointIt, obstacleVertices.end());

	//We push duplicate to complete the connection
	obstacleVertices.push_back(obstacleVertices[0]);
	obstacleVertices.push_back(*connectToIt);

	m_Vertices.insert(connectToIt + 1, obstacleVertices.begin(), obstacleVertices.end());

}

void NavigationMesh::CreateNodesAndGraph()
{

	//Add and connect all the current nodes in the Graph
	Node* previousNode{ nullptr };
	Node* firstNode{ nullptr };
	Node* currentNode{ nullptr };
	for (const Point2f& vertex : m_Vertices)
	{
		currentNode = m_NavMesh->GetNodeInRange(vertex, 1.f);
		if (!currentNode)
		{
			currentNode = new Node(vertex, Color4f{ 0.f,0.f,1.f,1.f });
			m_NavMesh->AddNode(currentNode);
		}
		m_pNodes.push_back(currentNode);

		if (previousNode)m_NavMesh->AddConnection(previousNode, currentNode);
		else firstNode = currentNode;
		previousNode = currentNode;
	}
	m_NavMesh->AddConnection(firstNode, currentNode);
}

void NavigationMesh::StartEarClipping()
{
	std::cout << "\n---------Clipping Ears--------\n";
	//Loop over the Nodes and check which nodes can be connected using earclipping
	bool clippedEar{ false };
	while (m_Vertices.size() >= 3)
	{
		std::cout << "New loop, Remaining points: " << m_Vertices.size() << "\n";
		for (int i = 0; i < m_Vertices.size(); i++)
		{
			//Calculate the indices
			int prevIndex{ i - 1 };
			if (prevIndex < 0) prevIndex += m_Vertices.size();
			int nextIndex{ (i + 1) % int(m_Vertices.size()) };

			if (CanClipEar(prevIndex, i, nextIndex))
			{
				m_NavMesh->AddConnection(m_pNodes[prevIndex], m_pNodes[nextIndex]);
				m_Triangles.push_back(Triangle{ m_pNodes[prevIndex], m_pNodes[i], m_pNodes[nextIndex] });

				m_pNodes.erase(m_pNodes.begin() + i);
				m_Vertices.erase(m_Vertices.begin() + i);
				clippedEar = true;
			}
		}

		if (!clippedEar)
		{
			std::cout << "ERROR: EAR CLIPPING FAILED, STOPPING LOOP\n";
			return;
		}
		clippedEar = false;
	}
	std::cout << "---------Ears clipped--------\n";
}

bool NavigationMesh::CanClipEar(int prevIndex, int targetIndex, int nextIndex)
{
	//These 3 indices represent a triangle
	std::vector<Point2f> triangle{ m_Vertices[prevIndex],m_Vertices[targetIndex],m_Vertices[nextIndex]};
	Vector2f vector1{ triangle[0], triangle[1] };
	Vector2f vector2{ triangle[1], triangle[2] };

	//Cant clip a triangle if the angle > 180 (This crossproduct < 0)
	float cross = vector1.CrossProduct(vector2);
	if (cross < 0.f)
	{
		std::cout << "Could not clip, bad cross\n";
		return false;

	}

	//Loop over all vertices and check if there's a vertex in current triangle
	for (size_t i = 0; i < m_Vertices.size(); i++)
	{
		if (m_Vertices[i] != m_Vertices[prevIndex]
			&& m_Vertices[i] != m_Vertices[targetIndex]
			&& m_Vertices[i] != m_Vertices[nextIndex])
		{
			if (utils::IsPointInPolygon(m_Vertices[i], triangle))
			{
				std::cout << "Could not clip, point obstructing: " << m_Vertices[i].x << ", " << m_Vertices[i].y << std::endl;
				return false;
			}
		}
	}

	std::cout << "Clipped: " << prevIndex << ", " << targetIndex << ", " << nextIndex << std::endl;
	return true;
}
