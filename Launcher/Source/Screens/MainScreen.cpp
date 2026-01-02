#include "pch.h"
#include "MainScreen.h"
#include "ModsScreen.h"
#include "SettingsScreen.h"
#include "ContactsScreen.h"
#include "Application.h"
#include "UIScreenControl.h"
#include "GameSettings.h"
#include "Platform/Process.h"

#include <Toshi/T2String.h>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

MainScreen::MainScreen()
{
	EnableGameButtons( g_oTheApp.HasGame() );
}

MainScreen::~MainScreen()
{
}

void MainScreen::Render()
{
	IUIScreen::Render();

	ImGuiStyle& style = ImGui::GetStyle();

	ImGui::BeginDisabled( !m_bEnableGameButtons );
	{
		// Draw Control buttons
		ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 28.0f, 8.0f ) );

		ImVec2 region         = ImGui::GetContentRegionAvail();
		TFLOAT flFontSize     = ImGui::GetFontSize();
		TFLOAT flAnimProgress = GetAnimProgress();

		style.Alpha = TMath::Max(flAnimProgress, 0.001f);
		region.y -= (1.0f - flAnimProgress) * 64;

		ImGui::SetCursorPos( ImVec2( 0, region.y - ( flFontSize + style.FramePadding.y * 2 ) - flFontSize * 1.4f ) );

		ImGui::PushStyleColor( ImGuiCol_Text, m_bEnableGameButtons ? ImVec4( 1.0f, 1.0f, 1.0f, 0.4f ) : ImVec4( 1.0f, 0.72f, 0.1f, 1.0f ) );
		ImGui::Text( m_bEnableGameButtons ? "Ready to play!" : "No game found!" );
		ImGui::PopStyleColor();

		ImGui::SetCursorPos( ImVec2( 0, region.y - ( flFontSize + style.FramePadding.y * 2 ) ) );

		if ( ImGui::Button( "PLAY" ) )
			Button_PlayGame();

		ImGui::SameLine();
		if ( ImGui::Button( "SETTINGS" ) )
			Button_Settings();

		ImGui::SameLine();
		if ( ImGui::Button( "MANAGE MODS" ) )
			Button_Mods();

		ImGui::SameLine();
		if ( ImGui::Button( "CONTACTS" ) )
			Button_Contacts();
		
		ImGui::PopStyleVar(); // ImGuiStyleVar_FramePadding
		ImGui::EndDisabled();
	}
}

void MainScreen::Update( TFLOAT flDeltaTime )
{
	IUIScreen::Update( flDeltaTime );
}

void MainScreen::EnableGameButtons( TBOOL bEnable )
{
	m_bEnableGameButtons = bEnable;
}

void MainScreen::Button_PlayGame()
{
	// Create a string with all launch parameters
	T2FormatWString512 strStartParams;
	{
		// Resolution
		strStartParams.Append( g_oSettings.bWindowed ? L"-windowed " : L"-fullscreen " );

		//const TString8& strResolution = g_oTheApp.GetScreenResolutions()[ 0 ];
		//TINT            iDivider      = strResolution.Find( 'x' );

		//if ( iDivider != -1 )
		{
			/*TString8 strWidth  = strResolution.Mid( 0, iDivider );
			TString8 strHeight = strResolution.Right( iDivider + 1 );

			TINT iWidth  = T2String8::StringToInt( strWidth );
			TINT iHeight = T2String8::StringToInt( strHeight );*/

			T2FormatWString128 resolutionParams;
			resolutionParams.Format( L"-width %d -height %d ", g_oSettings.iWidth, g_oSettings.iHeight );

			strStartParams.Append( resolutionParams.Get() );
		}

		// Other parameters
		/*if ( g_oSettings.bExperimental )
			strStartParams.Append( L"-experimental " );

		if ( g_oSettings.bFun )
			strStartParams.Append( L"-fun " );*/
	}

	TINFO( "Starting the game with '%s' parameters\n", strStartParams.Get() );

	Process gameProcess;
	gameProcess.Create(
	    L".\\BYardModLoader.exe",
	    strStartParams.Get(),
	    L".\\",
		Process::Priority_Normal,
		-1
	);
}

void MainScreen::Button_Settings()
{
	g_oUIControl.ShowScreen( new SettingsScreen() );
}

void MainScreen::Button_Mods()
{
	g_oUIControl.ShowScreen( new ModsScreen() );
}

void MainScreen::Button_Contacts()
{
	g_oUIControl.ShowScreen( new ContactsScreen() );
}
