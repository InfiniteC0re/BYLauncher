#pragma once
#include <Toshi/TString8.h>
#include <Toshi/TString16.h>
#include <Toshi/T2String.h>
#include <Toshi/TVersion.h>
#include <curl/curl.h>

namespace UpdateManager
{

struct VersionInfo
{
	Toshi::TVersion uiVersion;
	Toshi::TString8 strUpdateUrl;
};

void Initialise();

TBOOL CheckVersion( Toshi::T2StringView strUpdateInfoUrl, Toshi::TVersion uiCurrentVersion, VersionInfo* pOutVersionInfo );
TBOOL DownloadAndInstallUpdate( Toshi::T2StringView strUpdateArchiveUrl );
void ClearTempFiles();

} // namespace UpdateManager
