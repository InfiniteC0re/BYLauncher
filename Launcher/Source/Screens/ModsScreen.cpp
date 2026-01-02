#include "pch.h"
#include "ModsScreen.h"
#include "ModManager.h"
#include "MainScreen.h"
#include "UIScreenControl.h"
#include "UIFonts.h"

#include <imgui.h>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

ModsScreen::ModsScreen()
{
}

ModsScreen::~ModsScreen()
{
}

void ModsScreen::Render()
{
	IUIScreen::Render();

	ImVec2 region     = ImGui::GetContentRegionAvail();
	TFLOAT flFontSize = ImGui::GetFontSize();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha       = GetAnimProgress();

	ImGui::SetCursorPos( ImVec2( 0, 100 ) );

	ImGui::PushFont( UIFonts::Main28 );
	ImGui::Text( "Mod Manager" );
	ImGui::PopFont();

	ImGui::Text( "List of all mods" );
	if ( ImGui::BeginTable( "Mods", 3 ) )
	{
		auto& vecMods = ModManager::GetMods();

		ImGui::TableNextRow();
		ImGui::TableNextColumn();
		ImGui::Text( "Name" );
		ImGui::TableNextColumn();
		ImGui::Text( "Version" );
		ImGui::TableNextColumn();
		ImGui::Text( "Path" );

		for ( TINT i = 0; i < vecMods.Size(); i++ )
		{
			ModManager::Mod& mod = vecMods[ i ];
			ImGui::TableNextRow();
			ImGui::TableNextColumn();
			ImGui::Text( mod.strName.GetString() );
			ImGui::TableNextColumn();
			ImGui::Text( "v%d.%d", mod.uiVersion.Parts.Major, mod.uiVersion.Parts.Minor );
			ImGui::TableNextColumn();
			ImGui::Text( mod.strDllPath );
		}
		ImGui::EndTable();
	}

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 28.0f, 8.0f ) );
	ImGui::SetCursorPos( ImVec2( 0, region.y - ( flFontSize + style.FramePadding.y * 2 ) ) );

	if ( ImGui::Button( "BACK" ) )
	{
		g_oUIControl.ShowScreen( new MainScreen() );
	}
	ImGui::PopStyleVar();
}

void ModsScreen::Update( TFLOAT flDeltaTime )
{
    IUIScreen::Update( flDeltaTime );
}
