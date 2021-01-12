#pragma once

class Core
{
public:
	explicit Core( const Window& window );
	Core( const Core& other ) = delete;
	Core& operator=( const Core& other ) = delete;
	Core(Core&& other) = delete;
	Core& operator=(Core&& other) = delete;
	~Core( );

	void Run( );

private:
	// DATA MEMBERS
	// The window properties
	const Window m_Window;
	// The window we render to
	SDL_Window* m_pWindow;
	// OpenGL context
	SDL_GLContext m_pContext;
	// Init info
	bool m_Initialized;
	// Prevent timing jumps when debugging
	const float m_MaxElapsedSeconds;
	
	// FUNCTIONS
	void Initialize( );
	void Cleanup( );
};
