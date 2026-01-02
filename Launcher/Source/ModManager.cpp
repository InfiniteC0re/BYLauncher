#include "pch.h"
#include "ModManager.h"
#include "UpdateManager.h"

// We don't need any other header files of the ModCore to be in the list of includes
#include <../../OpenBarnyard/SDK/BYModCore/Include/ModLoader.h>

#include <ToshiTools/json.hpp>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

static T2DynamicVector<ModManager::Mod> s_vecMods;

void ModManager::Initialise()
{
	
}

void ModManager::ScanForMods()
{
	for ( const auto& entry : std::filesystem::directory_iterator( "Mods\\" ) )
	{
		if ( entry.path().extension().compare( L".dll" ) == 0 )
		{
			HMODULE hModule               = LoadLibraryExW( entry.path().native().c_str(), NULL, DONT_RESOLVE_DLL_REFERENCES );
			auto    fnGetModAutoUpdateURL = TREINTERPRETCAST( t_GetModAutoUpdateURL, GetProcAddress( hModule, "GetModAutoUpdateURL" ) );
			auto    fnGetVersion          = TREINTERPRETCAST( t_GetModVersion, GetProcAddress( hModule, "GetModVersion" ) );
			auto    fnGetName             = TREINTERPRETCAST( t_GetModName, GetProcAddress( hModule, "GetModName" ) );

			// Obtain the update url
			TString8 updateInfoURL = ( fnGetModAutoUpdateURL ) ? fnGetModAutoUpdateURL() : "";
			TString8 strModName    = ( fnGetName ) ? fnGetName() : "Unknown Mod";
			TVersion modVersion    = ( fnGetVersion ) ? fnGetVersion() : TVERSION( 1, 0 );

			// We don't need the dll to be attached anymore
			FreeLibrary( hModule );

			// Add info about the mod
			s_vecMods.PushBack();
			auto mod              = s_vecMods.Back();
			mod->strName          = strModName;
			mod->strAutoUpdateURL = updateInfoURL;
			mod->strDllPath       = Toshi::Platform_UnicodeToUTF8( entry.path().native().c_str() );
			mod->uiVersion        = modVersion;
			mod->bAutoUpdates     = updateInfoURL && fnGetVersion;

			//UpdateManager::VersionInfo versionInfo;
			//TBOOL                      bOutDated = UpdateManager::CheckVersion( updateInfoURL.GetString(), modVersion, &versionInfo );

			//if ( bOutDated )
			//{
			//	TINFO( "New version (v%u.%u) of the '%s' (v%u.%u) mod is available\n", versionInfo.uiVersion.Parts.Major, versionInfo.uiVersion.Parts.Minor, strModName.GetString(), modVersion.Parts.Major, modVersion.Parts.Minor );

			//	TString8 strDescription = TString8::VarArgs(
			//	    "New version of the '%s' mod is available!\n\n"
			//	    "Current version: v%u.%u\n"
			//	    "Latest version:  v%u.%u\n\n"
			//	    "Would you like to update now?",
			//	    strModName.GetString(),
			//	    modVersion.Parts.Major,
			//	    modVersion.Parts.Minor,
			//	    versionInfo.uiVersion.Parts.Major,
			//	    versionInfo.uiVersion.Parts.Minor
			//	);

			//	const TINT msgBoxResult = MessageBoxA( NULL, strDescription.GetString(), "Barnyard ModLoader", MB_YESNO );

			//	if ( msgBoxResult == IDYES )
			//	{
			//		TINFO( "User agreed to update mod\n" );

			//		AHTTPClient httpClient;
			//		httpClient.Create( versionInfo.strUpdateUrl.GetString() );
			//		httplib::Result downloadResult = httpClient.Get();

			//		// Redirect once
			//		if ( downloadResult->get_header_value_count( "location" ) )
			//		{
			//			httpClient.Create( downloadResult->get_header_value( "location" ).c_str() );
			//			downloadResult = httpClient.Get();
			//		}

			//		if ( downloadResult.error() != httplib::Error::Success ||
			//		     downloadResult->status != 200 )
			//		{
			//			TERROR( "Unable to download new version of the mod\n" );
			//			TASSERT( TFALSE );
			//			MessageBoxA( NULL, "Unable to download new version of the mod! Canceling...", "Barnyard ModLoader", MB_OK );
			//			continue;
			//		}

			//		TINFO( "Downloaded new version of the mod!\n" );

			//		HANDLE hFile = ::CreateFileW( entry.path().native().c_str(), GENERIC_WRITE, FILE_SHARE_WRITE, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );

			//		if ( hFile == INVALID_HANDLE_VALUE )
			//		{
			//			TERROR( "Unable to open file for writing. Error: %u\n", GetLastError() );
			//			TASSERT( TFALSE );
			//			MessageBoxA( NULL, "Unable to open file for writing! Canceling...", "Barnyard ModLoader", MB_OK );
			//			continue;
			//		}

			//		DWORD dwNumWritten;
			//		BOOL  bWritten = WriteFile( hFile, downloadResult->body.c_str(), downloadResult->body.size(), &dwNumWritten, NULL );

			//		if ( !bWritten || dwNumWritten != downloadResult->body.size() )
			//		{
			//			TERROR( "An unknown error has occured during updating the mod. Error: %u\n", GetLastError() );
			//			TASSERT( TFALSE );
			//			MessageBoxA( NULL, "An unknown error has occured during updating the mod.", "Barnyard ModLoader", MB_OK );
			//		}

			//		::CloseHandle( hFile );

			//		TINFO( "Successfully updated the mod!\n" );
			//		MessageBoxA( NULL, "The mod was successfully updated!", "Barnyard ModLoader", MB_OK );
			//	}
			//}
		}
	}
}

void ModManager::CheckForUpdates()
{
	TINFO( "Ckecking for updates...\n" );

	T2_FOREACH( s_vecMods, it )
	{
		if ( !it->bAutoUpdates )
			continue;
		
		UpdateManager::VersionInfo versionInfo;
		UpdateManager::CheckVersion( it->strAutoUpdateURL.GetString(), it->uiVersion, &versionInfo );
	}
}

T2DynamicVector<ModManager::Mod>& ModManager::GetMods()
{
	return s_vecMods;
}
