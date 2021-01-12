#include "pch.h"
#include "Core.h"
#include <iostream>
#include <algorithm>
#include <chrono>
#include "Game.h"

Core::Core( const Window& window )
	: m_Window{window}
	, m_Initialized{false}
	, m_pWindow{nullptr}
	, m_pContext{nullptr}
	,m_MaxElapsedSeconds{ 0.1f }
{
	Initialize( );
}

Core::~Core( )
{
	Cleanup( );
}

void Core::Initialize( )
{
	// Initialize SDL
	if ( SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) < 0 )
	{
		std::cerr << "Core::Initialize( ), error when calling SDL_Init: " << SDL_GetError( ) << std::endl;
		return;
	}

	// Use OpenGL 2.1
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MAJOR_VERSION, 2 );
	SDL_GL_SetAttribute( SDL_GL_CONTEXT_MINOR_VERSION, 1 );

	// Create window
	m_pWindow = SDL_CreateWindow(
		m_Window.title.c_str( ),
		SDL_WINDOWPOS_CENTERED,
		SDL_WINDOWPOS_CENTERED,
		int( m_Window.width ),
		int( m_Window.height ),
		SDL_WINDOW_OPENGL );
	if ( m_pWindow == nullptr )
	{
		std::cerr << "Core::Initialize( ), error when calling SDL_CreateWindow: " << SDL_GetError( ) << std::endl;
		return;
	}

	// Create OpenGL context 
	m_pContext = SDL_GL_CreateContext( m_pWindow );
	if ( m_pContext == nullptr )
	{
		std::cerr << "Core::Initialize( ), error when calling SDL_GL_CreateContext: " << SDL_GetError( ) << std::endl;
		return;
	}

	// Set the swap interval for the current OpenGL context,
	// synchronize it with the vertical retrace
	if ( m_Window.isVSyncOn )
	{
		if ( SDL_GL_SetSwapInterval( 1 ) < 0 )
		{
			std::cerr << "Core::Initialize( ), error when calling SDL_GL_SetSwapInterval: " << SDL_GetError( ) << std::endl;
			return;
		}
	}
	else
	{
		SDL_GL_SetSwapInterval( 0 );
	}
	
	// Set the Projection matrix to the identity matrix
	glMatrixMode( GL_PROJECTION ); 
	glLoadIdentity( );

	// Set up a two-dimensional orthographic viewing region.
	gluOrtho2D( 0, m_Window.width, 0, m_Window.height ); // y from bottom to top

	// Set the viewport to the client window area
	// The viewport is the rectangular region of the window where the image is drawn.
	glViewport( 0, 0, int( m_Window.width ), int( m_Window.height ) );

	// Set the Modelview matrix to the identity matrix
	glMatrixMode( GL_MODELVIEW );
	glLoadIdentity( );

	// Enable color blending and use alpha blending
	glEnable( GL_BLEND );
	glBlendFunc( GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA );

	// Initialize PNG loading
	int imgFlags = IMG_INIT_PNG;
	if ( !( IMG_Init( imgFlags ) & imgFlags ) )
	{
		std::cerr << "Core::Initialize( ), error when calling IMG_Init: " << IMG_GetError( ) << std::endl;
		return;
	}

	// Initialize SDL_ttf
	if ( TTF_Init( ) == -1 )
	{
		std::cerr << "Core::Initialize( ), error when calling TTF_Init: " << TTF_GetError( ) << std::endl;
		return;
	}

	//Initialize SDL_mixer
	if (Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 2048) < 0)
	{
		std::cerr << "Core::Initialize( ), error when calling Mix_OpenAudio: " << Mix_GetError() << std::endl;
		return;
	}

	m_Initialized = true;
}

void Core::Run( )
{
	if ( !m_Initialized )
	{
		std::cerr << "Core::Run( ), Core not correctly initialized, unable to run the game\n";
		std::cin.get( );
		return;
	}

	// Create the Game object
	Game game{ m_Window };

	// Main loop flag
	bool quit{ false };

	// Set start time
	std::chrono::steady_clock::time_point t1 = std::chrono::steady_clock::now();

	//The event loop
	SDL_Event e{};
	while ( !quit )
	{
		// Poll next event from queue
		while ( SDL_PollEvent( &e ) != 0 )
		{
			// Handle the polled event
			switch ( e.type )
			{
			case SDL_QUIT:
				quit = true;
				break;
			case SDL_KEYDOWN:
				game.ProcessKeyDownEvent( e.key );
				break;
			case SDL_KEYUP:
				game.ProcessKeyUpEvent( e.key );
				break;
			case SDL_MOUSEMOTION:
				game.ProcessMouseMotionEvent( e.motion );
				break;
			case SDL_MOUSEBUTTONDOWN:
				game.ProcessMouseDownEvent( e.button );
				break;
			case SDL_MOUSEBUTTONUP:
				game.ProcessMouseUpEvent( e.button );
				break;
			}
		}

		if ( !quit )
		{
			// Get current time
			std::chrono::steady_clock::time_point t2 = std::chrono::steady_clock::now();

			// Calculate elapsed time
			float elapsedSeconds = std::chrono::duration<float>(t2 - t1).count();

			// Update current time
			t1 = t2;

			// Prevent jumps in time caused by break points
			elapsedSeconds = std::min(elapsedSeconds, m_MaxElapsedSeconds);

			// Call the Game object 's Update function, using time in seconds (!)
			game.Update(elapsedSeconds);

			// Draw in the back buffer
			game.Draw( );

			// Update screen: swap back and front buffer
			SDL_GL_SwapWindow( m_pWindow );
		}
	}
}

void Core::Cleanup( )
{
	SDL_GL_DeleteContext( m_pContext );

	SDL_DestroyWindow( m_pWindow );
	m_pWindow = nullptr;

	//Quit SDL subsystems
	Mix_Quit( );
	TTF_Quit( );
	IMG_Quit( );
	SDL_Quit( );
}
