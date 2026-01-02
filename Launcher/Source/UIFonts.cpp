#include "pch.h"
#include "UIFonts.h"

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

ImFont* UIFonts::Main18 = TNULL;
ImFont* UIFonts::Main20 = TNULL;
ImFont* UIFonts::Main28 = TNULL;

void UIFonts::Create()
{
	ImGuiIO& io = ImGui::GetIO();

	constexpr const char* pchFontFile = "C:\\Windows\\Fonts\\segoeui.ttf";

	// Main Font
	Main20 = io.Fonts->AddFontFromFileTTF( pchFontFile, 20.0f, TNULL, io.Fonts->GetGlyphRangesDefault() );
	Main18 = io.Fonts->AddFontFromFileTTF( pchFontFile, 18.0f, TNULL, io.Fonts->GetGlyphRangesDefault() );
	Main28 = io.Fonts->AddFontFromFileTTF( pchFontFile, 28.0f, TNULL, io.Fonts->GetGlyphRangesDefault() );
}
