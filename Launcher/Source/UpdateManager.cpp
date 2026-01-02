#include "pch.h"
#include "UpdateManager.h"

#include <ToshiTools/json.hpp>

#include <mz.h>
#include <mz_zip.h>
#include <mz_strm.h>
#include <mz_strm_os.h>
#include <mz_zip_rw.h>

#include <filesystem>

//-----------------------------------------------------------------------------
// Enables memory debugging.
// Note: Should be the last include!
//-----------------------------------------------------------------------------
#include <Core/TMemoryDebugOn.h>

TOSHI_NAMESPACE_USING

static CURL* s_pCurl;
static size_t CURLWriteCallback( void* contents, size_t size, size_t nmemb, void* userp )
{
	( (std::string*)userp )->append( (char*)contents, size * nmemb );
	return size * nmemb;
}

void UpdateManager::Initialise()
{
	curl_global_init( CURL_GLOBAL_DEFAULT );

	s_pCurl = curl_easy_init();

	if ( !s_pCurl )
	{
		TERROR( "curl_easy_init() returned null pointer\n" );
		return;
	}

	curl_easy_setopt( s_pCurl, CURLOPT_SSL_VERIFYPEER, 0L );
	curl_easy_setopt( s_pCurl, CURLOPT_SSL_VERIFYHOST, 0L );
	curl_easy_setopt( s_pCurl, CURLOPT_CA_CACHE_TIMEOUT, 604800L );
	curl_easy_setopt( s_pCurl, CURLOPT_WRITEFUNCTION, CURLWriteCallback );
	curl_easy_setopt( s_pCurl, CURLOPT_FOLLOWLOCATION, TRUE );
}

TBOOL UpdateManager::CheckVersion( Toshi::T2StringView strUpdateInfoUrl, Toshi::TVersion uiCurrentVersion, VersionInfo* pOutVersionInfo )
{
	std::string responseBuffer;
	curl_easy_setopt( s_pCurl, CURLOPT_WRITEDATA, &responseBuffer );
	curl_easy_setopt( s_pCurl, CURLOPT_URL, strUpdateInfoUrl.Get() );

	// Do the request
	CURLcode res = curl_easy_perform( s_pCurl );
	if ( res != CURLE_OK ) return TFALSE;

	// Parse json
	nlohmann::json json;
	try
	{
		json = json.parse( responseBuffer );
	}
	catch ( ... )
	{
		TERROR( "Error parsing JSON response from URL: %s\n", strUpdateInfoUrl.Get() );
		return TFALSE;
	}

	if ( json.is_null() || !json.is_object() )
		return TFALSE;

	auto version = json.find( "version" );
	auto latest = json.find( "latest" );

	if ( version == json.end() || latest == json.end() ) return TFALSE;
	if ( !version->is_array() || !latest->is_string() ) return TFALSE;

	if ( version->size() != 2 || !version->at( 0 ).is_number_integer() || !version->at( 1 ).is_number_integer() )
		return TFALSE;

	TVersion latestVersion = TVERSION( version->at( 0 ).get<TINT>(), version->at( 1 ).get<TINT>() );

	if ( pOutVersionInfo )
	{
		pOutVersionInfo->uiVersion = latestVersion;
		pOutVersionInfo->strUpdateUrl = latest->get<std::string>().c_str();
	}

	return ( uiCurrentVersion.Parts.Major < latestVersion.Parts.Major || uiCurrentVersion.Parts.Minor < latestVersion.Parts.Minor );
}

TBOOL UpdateManager::DownloadAndInstallUpdate( Toshi::T2StringView strUpdateArchiveUrl )
{
	std::string responseBuffer;
	curl_easy_setopt( s_pCurl, CURLOPT_WRITEDATA, &responseBuffer );
	curl_easy_setopt( s_pCurl, CURLOPT_URL, strUpdateArchiveUrl.Get() );

	// Do the request
	CURLcode res = curl_easy_perform( s_pCurl );
	if ( res != CURLE_OK ) return TFALSE;

	{
		void* pZIP = mz_zip_reader_create();

		TINT32 iZipOpenRes = mz_zip_reader_open_buffer( pZIP, (uint8_t*)responseBuffer.data(), responseBuffer.size(), 0 );

		if ( iZipOpenRes != MZ_OK )
		{
			TERROR( "Unable to open ZIP file (Result: %d)\n", iZipOpenRes );
			return TFALSE;
		}

		TNativeFileInfo oDirInfo;
		TFileManager::GetSingleton()->GetFileInfo( ".", oDirInfo );
		TINFO( "Updating... (url: %s; workdir: %s)\n", strUpdateArchiveUrl.Get(), oDirInfo.InternalPath.GetString() );

		T2Map<TPString8, TString8, TPString8::Comparator> mapRenamedFiles;

		TINT32 err = mz_zip_reader_goto_first_entry( pZIP );
		while ( err == MZ_OK )
		{
			mz_zip_file* file_info = NULL;

			// Get info about each of the files
			if ( mz_zip_reader_entry_get_info( pZIP, &file_info ) == MZ_OK )
			{
				if ( mz_zip_reader_entry_is_dir( pZIP ) != MZ_OK )
				{
					TString8 strUpdateFilePath = TString8::VarArgs( "%s%s", oDirInfo.InternalPath.GetString(), file_info->filename );
					FixPathSlashes( strUpdateFilePath );

					TINFO( "File in the update: %s (size: %lld)\n", file_info->filename, file_info->uncompressed_size );
					
					// We found a file in the archive, try renaming it in the directory
					// This is needed to make the update apply
					// Temp files will be removed later
					TString8 strUpdateFilePathNew = strUpdateFilePath + ".TEMP";
					BOOL bRenameResult = MoveFileA( strUpdateFilePath, strUpdateFilePathNew );

					if ( bRenameResult == TRUE )
					{
						TINFO("File got renamed (path: %s)!\n", strUpdateFilePathNew.GetString() );
						mapRenamedFiles.Insert( TPS8D( strUpdateFilePath.GetString() ), strUpdateFilePathNew );
					}
				}
			}

			err = mz_zip_reader_goto_next_entry( pZIP );
		}

		// Extract files
		TINT32 iSaveErr = mz_zip_reader_save_all( pZIP, oDirInfo.InternalPath.GetString() );

		// If succeeded, update was done. If not, revert the file renamings!!!
		if ( iSaveErr != MZ_OK )
		{
			TERROR( "An error has occured while extracting files... Reverting renaming\n" );

			// Revert renaming
			T2_FOREACH( mapRenamedFiles, it )
			{
				BOOL bRenameResult = MoveFileA( it->second, it->first );

				if ( bRenameResult == FALSE )
				{
					TERROR("Critical: error while reverting file rename!!! (old: %s; new: %s)\n", it->first.GetString(), it->second.GetString() );
				}
			}
		}

		// Delete the .TEMP files we created within this update
		T2_FOREACH( mapRenamedFiles, it )
		{
			DeleteFileA( it->second );
		}

		mz_zip_reader_close( pZIP );
		mz_zip_reader_delete( &pZIP );
	}

	return TTRUE;
}

void UpdateManager::ClearTempFiles()
{
	TNativeFileInfo oDirInfo;
	TFileManager::GetSingleton()->GetFileInfo( ".", oDirInfo );

	TINFO( "Clearing TEMP files... (workdir: %s)\n", oDirInfo.InternalPath.GetString() );

	try
	{
		// Iterate recursively through the directory
		for ( const auto& entry : std::filesystem::recursive_directory_iterator( oDirInfo.InternalPath.GetString() ) )
		{
			// Check if the entry is a regular file
			if ( entry.is_regular_file() )
			{
				// Compare the file's extension with the target extension (case-insensitive comparison might be needed for cross-platform robustness)
				if ( entry.path().extension() == ".TEMP" )
				{
					DeleteFileW( entry.path().native().c_str() );
				}
			}
		}
	}
	catch ( const std::filesystem::filesystem_error& e )
	{
		TERROR( "Filesystem error: %s\n", e.what() );
	}
}
