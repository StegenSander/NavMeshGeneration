#include "pch.h"
#include "Game.h"
#include <iostream>

Game::Game( const Window& window ) 
	:m_Window{ window }
	, m_pGraph{new Graph()}
	, m_Camera{ Point2f{0,0} }
	, m_pNavMesh{ new NavigationMesh{Rectf{50,50,window.width-100,window.height-100}}}
{
	Initialize( );
	PrintInformation();
}

Game::~Game( )
{
	Cleanup( );
}

void Game::Initialize( )
{
	/*m_pStartNode = new Node(Point2f(0,0), Color4f(1.f, 0.f, 1.f, 1.f));
	m_pEndNode = new Node(Point2f(0, 0), Color4f(1.f, 0.f, 1.f, 1.f));
	m_pGraph->AddNode(m_pStartNode);
	m_pGraph->AddNode(m_pEndNode);*/


	//m_Obstacles.push_back(new Obstacle{ {Point2f(430,70)
	//	,Point2f(445,400)
	//	,Point2f(570,100)}, Color4f{1.f,0.f,0.f,.5f } });

	//m_Obstacles.push_back(new Obstacle{ {Point2f(30,50)
	//	,Point2f(270,300)
	//	,Point2f(370,100)}, Color4f{1.f,0.f,0.f,.5f } });

	//m_Obstacles.push_back(new Obstacle{ {Point2f(530,400)
	//	,Point2f(530,450)
	//	,Point2f(700,450)
	//	,Point2f(700,400)}, Color4f{1.f,0.f,0.f,.5f } });

	//m_Obstacles.push_back(new Obstacle{ {Point2f(50,50)
	//	,Point2f(200,200)
	//	,Point2f(80,370)
	//	,Point2f(300,385)
	//	,Point2f(500,100)}, Color4f{1.f,0.f,0.f,.5f } });

	m_pNavMesh->SetObstacles(m_Obstacles);
	m_pNavMesh->CalculateNavMesh();
}

void Game::Cleanup( )
{
	delete m_pGraph;
	for (const Obstacle* pObstacle : m_Obstacles)
	{
		delete pObstacle;
	}

	if (m_pNavMesh) delete m_pNavMesh;
}

void Game::Update( float elapsedSec )
{
	m_LatestDeltaTime = elapsedSec;
	// Check keyboard state
	const Uint8 *pStates = SDL_GetKeyboardState( nullptr );
	if ( pStates[SDL_SCANCODE_RIGHT] )
	{
		m_Camera.MoveCamera(Vector2f{ elapsedSec, 0 }, true);
	}
	if ( pStates[SDL_SCANCODE_LEFT])
	{
		m_Camera.MoveCamera(Vector2f{ -elapsedSec,0 }, true);
	}
	if (pStates[SDL_SCANCODE_UP])
	{
		m_Camera.MoveCamera(Vector2f{ 0,elapsedSec }, true);
	}
	if (pStates[SDL_SCANCODE_DOWN])
	{
		m_Camera.MoveCamera(Vector2f{ 0,-elapsedSec }, true);
	}

	m_pGraph->GetConnectionsOfNode(nullptr);
}

void Game::Draw( ) const
{
	ClearBackground( );

	Point2f cameraPos = m_Camera.GetPosition();
	glPushMatrix();
	glTranslatef(-cameraPos.x, -cameraPos.y, 0);
	m_pGraph->Draw(true,true);
	for (const Obstacle* pObstacle : m_Obstacles)
	{
		pObstacle->Draw();
	}
	m_pNavMesh->Draw();
	glPopMatrix();

}

void Game::ProcessKeyDownEvent( const SDL_KeyboardEvent & e )
{
	//std::cout << "KEYDOWN event: " << e.keysym.sym << std::endl;
}

void Game::ProcessKeyUpEvent( const SDL_KeyboardEvent& e )
{
	switch ( e.keysym.sym )
	{
	case SDLK_i:
	{
		PrintInformation();
	}
		break;
	case SDLK_r: //recalculate NavmHes
		if (m_Obstacles.size() >0)
		{
			m_Obstacles.back()->CompleteObstacle();
		}
		try
		{
			m_pNavMesh->CleanUpNavMesh();
			m_pNavMesh->SetObstacles(m_Obstacles);
			m_pNavMesh->CalculateNavMesh();
		}
		catch (const std::exception& ex)
		{
			std::cout << "Something went wrong: " << ex.what() << std::endl;
		}
		break;
	case SDLK_q: //Clear obstacles
		for (const Obstacle* pObstacle : m_Obstacles)
		{
			delete pObstacle;
		}
		m_Obstacles.clear();
		break;
	case SDLK_UP:
		break;
	case SDLK_DOWN:
		break;
	case SDLK_SPACE:
		std::cout << "Space key released\n";
		m_Camera.ResetCameraPos();
		break;
	}
}

void Game::ProcessMouseMotionEvent( const SDL_MouseMotionEvent& e )
{
	Vector2f mousePos{ float(e.x), m_Window.height - float(e.y) };
	//std::cout << "MOUSEMOTION event: " << e.x << ", " << e.y << std::endl;
	if (m_IsMiddleMouseDown)
	{
		m_Camera.MoveCamera(m_LatestMousePos -  mousePos, false);
	}

	m_LatestMousePos = mousePos;
}

void Game::ProcessMouseDownEvent( const SDL_MouseButtonEvent& e )
{
	//std::cout << "MOUSEBUTTONDOWN event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		//std::cout << " left button " << std::endl;
		break;
	case SDL_BUTTON_RIGHT:
		//std::cout << " right button " << std::endl;
		break;
	case SDL_BUTTON_MIDDLE:
		//std::cout << " middle button " << std::endl;
		m_IsMiddleMouseDown = true;
		break;
	}
}

void Game::ProcessMouseUpEvent( const SDL_MouseButtonEvent& e )
{
	Point2f mousePos{ float(e.x), m_Window.height - float(e.y) };
	Point2f worldPos{ m_Camera.GetPosition() + mousePos };
	//std::cout << "MOUSEBUTTONUP event: ";
	switch ( e.button )
	{
	case SDL_BUTTON_LEFT:
		//HandleGraphLeftClick(worldPos);
		if (m_Obstacles.size() == 0)
		{
			m_Obstacles.push_back(new Obstacle());
			std::cout << "new Obstacle created\n";
		}
		if (m_Obstacles.back()->IsCompleted())
		{
			m_Obstacles.push_back(new Obstacle());
			std::cout << "new Obstacle created\n";
		}

		m_Obstacles.back()->PushBackPoint(worldPos);

		std::cout << "Point added\n";

		break;
	case SDL_BUTTON_RIGHT:
	{
		//HandleGraphRightClick(worldPos);
	}
		break;
	case SDL_BUTTON_MIDDLE:
		m_IsMiddleMouseDown = false;
		break;
	}
}

void Game::ClearBackground( ) const
{
	glClearColor( 0.9f, 0.9f, 0.9f, 1.0f );
	glClear( GL_COLOR_BUFFER_BIT );
}

void Game::HandleGraphLeftClick(const Point2f& worldPos)
{
	Node* pNode = m_pGraph->GetNodeInRange(worldPos);
	if (m_pStartConnectionNode)
	{
		if (pNode)
		{
			if (pNode != m_pStartConnectionNode)
			{
				m_pGraph->AddConnection(m_pStartConnectionNode, pNode);
			}
		}
		else
		{
			Node* newNode = new Node(worldPos, Color4f{ 1.f,0.f,0.f ,1.f });
			m_pGraph->AddNode(newNode);
			m_pGraph->AddConnection(m_pStartConnectionNode, newNode);
		}

		m_pStartConnectionNode->SetColor(Color4f{ 1.f,0.f,0.f ,1.f });
		m_pStartConnectionNode = nullptr;
	}
	else
	{
		if (pNode)
		{
			m_pStartConnectionNode = pNode;
			pNode->SetColor(Color4f{ 0.f,1.f,0.f,1.f });
			std::cout << "hit\n";
		}
		else m_pGraph->AddNode(new Node{ worldPos,Color4f{1.f,0.f,0.f ,1.f} });
	}
}
void Game::HandleGraphRightClick(const Point2f& worldPos)
{

	//if (m_pStartConnectionNode)
	//{
	//	m_pStartConnectionNode->SetColor(Color4f{ 1.f,0.f,0.f ,1.f });
	//	m_pStartConnectionNode = nullptr;
	//}
	//Node* pNode = m_pGraph->GetNodeInRange(worldPos);
	//if (pNode)
	//{
	//	if (pNode != m_pStartNode && pNode != m_pEndNode)
	//	{
	//		m_pGraph->RemoveNode(pNode);
	//	}
	//}
}

void Game::PrintInformation() const
{
	std::cout << "\n---CONTROLS---\n";
	std::cout << "I: Print controls\n";
	std::cout << "Q: Delete all obstacles\n";
	std::cout << "R: Recalculate navmesh\n";

	std::cout << "\n---Creating Obstacles---\n";
	std::cout << "Clicking anywhere in the level with add a point to the current obstacle\n";
	std::cout << "Clicking on the first node of the obstacle will complete it and allow a new obstacle to be created\n";
	std::cout << "IMPORTANT: All Obstacle should be defined in COUNTER CLOCKWISE order\n";
}
