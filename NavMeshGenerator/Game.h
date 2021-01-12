#pragma once
#include "Graph.h"
#include "Camera.h"
#include "Obstacle.h"
#include "AStar.h"

#include "NavigationMesh.h"

class Game
{
public:
	explicit Game( const Window& window );
	Game(const Game& other) = delete;
	Game& operator=(const Game& other) = delete;
	Game( Game&& other) = delete;
	Game& operator=(Game&& other) = delete;
	~Game();

	void Update( float elapsedSec );
	void Draw( ) const;

	// Event handling
	void ProcessKeyDownEvent( const SDL_KeyboardEvent& e );
	void ProcessKeyUpEvent( const SDL_KeyboardEvent& e );
	void ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e );
	void ProcessMouseDownEvent( const SDL_MouseButtonEvent& e );
	void ProcessMouseUpEvent( const SDL_MouseButtonEvent& e );

private:
	// DATA MEMBERS
	const Window m_Window;

	float m_LatestDeltaTime;
	Vector2f m_LatestMousePos;

	Graph* m_pGraph;
	NavigationMesh* m_pNavMesh;

	Camera m_Camera;

	bool m_IsMiddleMouseDown;
	Node* m_pStartConnectionNode = nullptr;

	std::vector<Obstacle*> m_Obstacles;

	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
	void ClearBackground( ) const;

	void HandleGraphLeftClick(const Point2f& worldPos);
	void HandleGraphRightClick(const Point2f& worldPos);

	void PrintInformation() const;
};