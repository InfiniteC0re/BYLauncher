#include "pch.h"
#include "SettingsScreen.h"
#include "GameSettings.h"
#include "MainScreen.h"
#include "UIScreenControl.h"
#include "Application.h"
#include "Hash.h"
#include "UIFonts.h"

#include <Toshi/T2Map.h>
#include <SDL/SDL.h>

#include <imgui.h>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

SettingsScreen::SettingsScreen()
{
	TINT iDisplayIndex    = 0;
	TINT iNumDisplayModes = SDL_GetNumDisplayModes( iDisplayIndex );

	T2Map<TUINT32, TBOOL> mapResolutions;
	for ( TINT i = 0; i < iNumDisplayModes; i++ )
	{
		SDL_DisplayMode displayMode;
		SDL_GetDisplayMode( iDisplayIndex, i, &displayMode );

		TUINT32 uiHash = Hash_Vec2i( displayMode.w, displayMode.h );
		if ( mapResolutions.Find( uiHash ) == mapResolutions.End() )
		{
			TINFO( "Found new screen resolution (%dx%d)\n", displayMode.w, displayMode.h );

			if ( displayMode.w == g_oSettings.iWidth && displayMode.h == g_oSettings.iHeight )
				m_iSelectedResolution = m_vecResolutions.Size();

			m_vecResolutions.PushBack( std::move( TString8::VarArgs( "%dx%d", displayMode.w, displayMode.h ) ) );
			mapResolutions.Insert( uiHash, TTRUE );
		}
	}
}

SettingsScreen::~SettingsScreen()
{
}

void SettingsScreen::Render()
{
	IUIScreen::Render();

	ImVec2 region     = ImGui::GetContentRegionAvail();
	TFLOAT flFontSize = ImGui::GetFontSize();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha       = GetAnimProgress();

	ImGui::SetCursorPos( ImVec2( 0, 100 ) );

	ImGui::PushFont( UIFonts::Main28 );
	ImGui::Text( "Settings" );
	ImGui::PopFont();

	ImGui::Text( "Game" );

	if ( ImGui::BeginCombo( "Resolution", m_vecResolutions[ m_iSelectedResolution ] ) )
	{
		T2_FOREACH( m_vecResolutions, it )
		{
			TBOOL bSelected = ( it.Index() == m_iSelectedResolution );

			if ( ImGui::Selectable( it.Get()->GetString(), &bSelected ) )
			{
				m_iSelectedResolution = it.Index();
				const TString8& strResolution = *it;
				TINT            iDivider      = strResolution.Find( 'x' );

				if ( iDivider != -1 )
				{
					TINT iWidth  = T2String8::StringToInt( strResolution.Mid( 0, iDivider ) );
					TINT iHeight = T2String8::StringToInt( strResolution.Right( iDivider + 1 ) );

					g_oSettings.iWidth  = iWidth;
					g_oSettings.iHeight = iHeight;
					g_oSettings.Save();
				}
			}

			if ( bSelected )
				ImGui::SetItemDefaultFocus();
		}

		ImGui::EndCombo();
	}

	ImGui::SetCursorPosY( ImGui::GetCursorPosY() - 3.0f );
	ImGui::PushFont( UIFonts::Main18 );
	ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ) );
	ImGui::Text( "Notice that some high screen resolutions are not supported!" );
	ImGui::PopStyleColor(); // ImGuiCol_Text
	ImGui::PopFont();

	ImGui::Checkbox( "Windowed Mode", &g_oSettings.bWindowed );
	ImGui::Checkbox( "Controller Support", &g_oSettings.bEnableController );
//	ImGui::Separator();

// 	ImGui::Text( "Modloader" );
// 	ImGui::Checkbox( "Experimental Mode", &g_oSettings.bExperimental );
// 	ImGui::Checkbox( "Fun%", &g_oSettings.bFun );
 
 	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 28.0f, 8.0f ) );
 	ImGui::SetCursorPos( ImVec2( 0, region.y - ( flFontSize + style.FramePadding.y * 2 ) ) );

	if ( ImGui::Button( "BACK" ) )
	{
		g_oUIControl.ShowScreen( new MainScreen() );

		g_oSettings.Save();
	}

	ImGui::PopStyleVar();
}

void SettingsScreen::Update( TFLOAT flDeltaTime )
{
	IUIScreen::Update( flDeltaTime );
}
