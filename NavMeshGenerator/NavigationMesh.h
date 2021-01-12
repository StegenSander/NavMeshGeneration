#pragma once
#include <list>
#include <vector>
#include "Graph.h"
#include "Obstacle.h"


struct Triangle
{
	Node* point1;
	Node* point2;
	Node* point3;
};

class NavigationMesh
{
public:
	NavigationMesh(const Rectf& boundaries);

	~NavigationMesh();
	void CleanUpNavMesh();

	void SetObstacles(const std::vector<Obstacle*> obstacles);
	Graph* GetNavMesh() const { return m_NavMesh; };

	void Draw();
	void CalculateNavMesh();

private:
	//-----Private Functions-----
	void InitializeVertices();

	std::vector<Point2f> InsetVertices(const std::vector<Point2f>& vertices);

	void SortObstacles();
	void HandleInvalidObstacles();

	void InsertObstacleVertices(Obstacle* pObstacle);

	void CreateNodesAndGraph();

	void StartEarClipping();
	bool CanClipEar(int prevIndex, int targetIndex, int nextIndex);

	//-----Data members------
	Rectf m_Boundaries;

	std::vector<Point2f> m_Vertices;
	std::vector<Node*> m_pNodes;
	std::vector<Triangle> m_Triangles;
	std::vector<Obstacle*> m_Obstacles;

	Graph* m_NavMesh;

	float m_AgentRadius{ 30.f };
	bool m_MustInsertVertices{ true };
};

