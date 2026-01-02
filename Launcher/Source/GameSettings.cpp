#include "pch.h"
#include "GameSettings.h"
#include "vdf.hpp"

#include <Toshi/T2String.h>
#include <Plugins/PTRB.h>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

using namespace tyti;

TOSHI_NAMESPACE_USING

GameSettings::GameSettings()
{
}

GameSettings::~GameSettings()
{
}

void GameSettings::Save()
{
	// First of all save settings to the VDF file
	std::ofstream fileStream( "BYLauncher.vdf" );

	vdf::object settings;
	settings.set_name( "BYardLauncher" );
	settings.attribs[ "experimental" ] = vdf::toString( bExperimental );
	settings.attribs[ "fun" ]          = vdf::toString( bFun );
	settings.attribs[ "dxvk" ]         = vdf::toString( bDXVK );
	settings.attribs[ "windowed" ]     = vdf::toString( bWindowed );
	settings.attribs[ "width" ]        = vdf::toString( iWidth );
	settings.attribs[ "height" ]       = vdf::toString( iHeight );

	vdf::write( fileStream, settings );

	// Now, save settings to the regedit, so the original game can access them too
	HKEY hKey;
	if ( ERROR_SUCCESS == RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\THQ\\Barnyard", 0, KEY_WRITE, &hKey ) )
	{
		DWORD dwWindowed = bWindowed ? 1 : 0;
		DWORD dwEnableController = bEnableController ? 1 : 0;

		HRESULT hRes;
		hRes = RegSetValueExA( hKey, "RealForcedWindowed", NULL, REG_DWORD, (LPBYTE)&dwWindowed, sizeof( dwWindowed ) );
		hRes = RegSetValueExA( hKey, "Width", NULL, REG_DWORD, (LPBYTE)&iWidth, sizeof( iWidth ) );
		hRes = RegSetValueExA( hKey, "Height", NULL, REG_DWORD, (LPBYTE)&iHeight, sizeof( iHeight ) );
		hRes = RegSetValueExA( hKey, "ControllerEnabled", NULL, REG_DWORD, (LPBYTE)&dwEnableController, sizeof( dwEnableController ) );
		
		RegCloseKey( hKey );
	}

	TINFO( "Saved core settings!\n" );
}

void GameSettings::Load()
{
	// First of all read settings of the original game from the regedit
	HKEY hKey;
	if ( ERROR_SUCCESS == RegOpenKeyExA( HKEY_CURRENT_USER, "Software\\THQ\\Barnyard", 0, KEY_READ, &hKey ) )
	{
		DWORD dwType = REG_NONE;
		DWORD dwData = 0;
		DWORD dwSize = sizeof( dwData );

		if ( ERROR_SUCCESS == RegQueryValueExA( hKey, "RealForcedWindowed", NULL, &dwType, (LPBYTE)&dwData, &dwSize ) &&
			 dwType == REG_DWORD )
		{
			bWindowed = dwData != FALSE;
		}

		dwSize = sizeof( iWidth );
		if ( ERROR_SUCCESS == RegQueryValueExA( hKey, "Width", NULL, &dwType, (LPBYTE)&dwData, &dwSize ) &&
			 dwType == REG_DWORD )
		{
			iWidth = dwData;
		}

		dwSize = sizeof( iHeight );
		if ( ERROR_SUCCESS == RegQueryValueExA( hKey, "Height", NULL, &dwType, (LPBYTE)&dwData, &dwSize ) &&
			 dwType == REG_DWORD )
		{
			iHeight = dwData;
		}

		dwSize = sizeof( dwData );
		if ( ERROR_SUCCESS == RegQueryValueExA( hKey, "ControllerEnabled", NULL, &dwType, (LPBYTE)&dwData, &dwSize ) &&
			 dwType == REG_DWORD )
		{
			bEnableController = dwData != FALSE;
		}

		RegCloseKey( hKey );
	}

	// Read from the VDF file
	std::ifstream fileStream( "BYLauncher.vdf" );

	if ( !fileStream.is_open() )
		return;

	vdf::object settings = vdf::read( fileStream );

	if ( settings.name != "BYardLauncher" )
		return;

	bExperimental = vdf::getBool( settings, "experimental", TFALSE );
	bFun          = vdf::getBool( settings, "fun", TFALSE );
	bDXVK         = vdf::getBool( settings, "dxvk", TTRUE );
	bWindowed     = vdf::getBool( settings, "windowed", TFALSE );
	iWidth        = vdf::getInt( settings, "width", 800 );
	iHeight       = vdf::getInt( settings, "height", 600 );

	Apply();
}

void GameSettings::Apply()
{
}
