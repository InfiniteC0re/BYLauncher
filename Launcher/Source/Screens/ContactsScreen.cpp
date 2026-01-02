#include "pch.h"
#include "ContactsScreen.h"
#include "UIScreenControl.h"
#include "MainScreen.h"
#include "UIFonts.h"
#include "Platform/Utils.h"

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

ContactsScreen::ContactsScreen()
{
}

ContactsScreen::~ContactsScreen()
{
}

void ContactsScreen::Render()
{
	IUIScreen::Render();

	ImVec2 region = ImGui::GetContentRegionAvail();
	TFLOAT flFontSize = ImGui::GetFontSize();

	ImGuiStyle& style = ImGui::GetStyle();
	style.Alpha = GetAnimProgress();

	ImGui::SetCursorPos( ImVec2( 0, 100 ) );

	ImGui::PushFont( UIFonts::Main28 );
	ImGui::Text( "Contacts" );
	ImGui::PopFont();

	ImGui::PushStyleColor( ImGuiCol_Text, ImVec4( 1.0f, 1.0f, 1.0f, 0.5f ) );
	ImGui::Text( "You can contact me on Discord: @infc0re" );
	ImGui::PopStyleColor(); // ImGuiCol_Text

	ImGui::SetCursorPosY( ImGui::GetCursorPosY() + 8.0f );

	ImVec2 vecResourcesCursor = ImGui::GetCursorPos();
	ImGui::Text( "Various Resources" );

	if ( ImGui::Button( "My GitHub", ImVec2( 200, 0 ) ) )
		Platform_OpenInShell( "https://github.com/InfiniteC0re" );

	if ( ImGui::Button( "OpenBarnyard Repository", ImVec2( 200, 0 ) ) )
		Platform_OpenInShell( "https://github.com/InfiniteC0re/OpenBarnyard" );

	if ( ImGui::Button( "Launcher Repository", ImVec2( 200, 0 ) ) )
		Platform_OpenInShell( "https://github.com/InfiniteC0re/BYLauncher" );

	constexpr TFLOAT flResourcesColMargin = 208.0f;
	ImGui::SetCursorPos( ImVec2( vecResourcesCursor.x + flResourcesColMargin, vecResourcesCursor.y ) );
	ImGui::Text( "Discord Servers" );

	ImGui::SetCursorPosX( vecResourcesCursor.x + flResourcesColMargin );
	if ( ImGui::Button( "OpenTOSHI", ImVec2( 200, 0 ) ) )
		Platform_OpenInShell( "https://discord.gg/ZYngJwhrsn" );

	ImGui::SetCursorPosX( vecResourcesCursor.x + flResourcesColMargin );
	if ( ImGui::Button( "Barnyard Speedrunning", ImVec2( 200, 0 ) ) )
		Platform_OpenInShell( "https://discord.gg/BrVpSPfgT8" );

	ImGui::PushStyleVar( ImGuiStyleVar_FramePadding, ImVec2( 28.0f, 8.0f ) );
	ImGui::SetCursorPos( ImVec2( 0, region.y - ( flFontSize + style.FramePadding.y * 2 ) ) );

	if ( ImGui::Button( "BACK" ) )
	{
		g_oUIControl.ShowScreen( new MainScreen() );
	}
	ImGui::PopStyleVar();
}
