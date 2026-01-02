#pragma once
#include <Toshi/Toshi.h>
#include <Toshi/TApplication.h>
#include <Toshi/TString8.h>

#include <Render/T2Render.h>
#include <Platform/GL/T2GLTexture_GL.h>

class Application
    : public Toshi::TApplication
    , public Toshi::T2Window::EventListener
{
public:
	//-----------------------------------------------------------------------------
	// T2Window::EventListener
	//-----------------------------------------------------------------------------
	virtual TBOOL OnEvent( const SDL_Event& event ) OVERRIDE;

	//-----------------------------------------------------------------------------
	// Toshi::TApplication
	//-----------------------------------------------------------------------------
	virtual TBOOL OnCreate( TINT argc, TCHAR** argv ) OVERRIDE;
	virtual TBOOL OnUpdate( TFLOAT flDeltaTime ) OVERRIDE;

	void LoadResources();

	TBOOL HasGame() const { return m_bHasGame; }

private:
	Toshi::T2Render::WindowParams m_oWindowParams;

	TBOOL m_bHasGame           = TFALSE;
	TBOOL m_bCreatedMainScreen = TFALSE;

	Toshi::T2GLTexture m_BackgroundTexture;

};

extern Application g_oTheApp;
