#pragma once
#include <list>
#include <vector>
#include "Graph.h"
#include "Obstacle.h"
#include <memory>


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
	virtual ~NavigationMesh();

	//------COPY CONSTRUCTORS------
	NavigationMesh(const NavigationMesh&) = delete;
	NavigationMesh(NavigationMesh&&) = delete;
	//------ASSIGNMENT OPERATORS------
	NavigationMesh operator=(const NavigationMesh&) = delete;
	NavigationMesh& operator=(NavigationMesh&&) = delete;

	/// <summary>
	/// Delete all previously stored data
	/// </summary>
	void CleanUpNavMesh();

	/// <summary>
	/// Set the obstacles of the navMesh
	/// </summary>
	/// <param name="obstacles"></param>
	void SetObstacles(const std::vector<Obstacle*> obstacles);
	/// <summary>
	/// Get the latest calculated NavMesh
	/// </summary>
	/// <returns>return a pointer to a navigation graph</returns>
	std::shared_ptr<Graph> GetNavMesh() const { return m_NavMesh; };

	void Draw();
	void CalculateNavMesh();

private:
	//-----Private Functions-----
	
	/// <summary>
	/// Create a rectangle based on the screen size and pushes the vertices to the buffer
	/// </summary>
	void InitializeVertices();

	/// <summary>
	/// Insets a buffer of vertices
	/// </summary>
	/// <param name="vertices">vertices to inset</param>
	/// <returns>return the insetted vertices</returns>
	std::vector<Point2f> InsetVertices(const std::vector<Point2f>& vertices);

	/// <summary>
	/// Sort all obstalce based on their x value, since we insert the obstacle with the biggest x value first
	/// </summary>
	void SortObstacles();
	/// <summary>
	/// Filter out the cases we cannot handle
	/// like: Overlapping obstacles, obstacles out of bounds, etc...
	/// </summary>
	void HandleInvalidObstacles();

	/// <summary>
	/// Loop over all obstacles and insert them at the right location in our buffer
	/// </summary>
	/// <param name="pObstacle"></param>
	void InsertObstacleVertices(Obstacle* pObstacle);

	/// <summary>
	/// Generate the navigation graph from the collected triangles
	/// </summary>
	void CreateNodesAndGraph();

	/// <summary>
	/// Start the ear clipping algorithm
	/// </summary>
	void StartEarClipping();
	/// <summary>
	/// Check if a triangle is a clippable ear
	/// </summary>
	/// <param name="prevIndex">index of the first point of a trianlge</param>
	/// <param name="targetIndex">index of the second point of a trianlge</param>
	/// <param name="nextIndex">index of the third point of a trianlge</param>
	/// <returns></returns>
	bool CanClipEar(int prevIndex, int targetIndex, int nextIndex);

	//-----Data members------
	Rectf m_Boundaries;

	std::vector<Point2f> m_Vertices;
	std::vector<Node*> m_pNodes;
	std::vector<Triangle> m_Triangles;
	std::vector<Obstacle*> m_Obstacles;

	std::shared_ptr<Graph> m_NavMesh;

	float m_AgentRadius{ 30.f };
	bool m_MustInsetVertices{ true };
};

class TemplateName
{
public:
	//------CONSTRUCTOR/DESTRUCTOR------

	//------PUBLIC FUNCTIONS------

	//------PUBLIC VARIABLES------
protected:
	//------PROTECTED FUNCTIONS------

	//------PROTECTED VARIABLES------	
private:
	//------PRIVATE FUNCTIONS------

	//------PRIVATE VARIABLES------	
};
