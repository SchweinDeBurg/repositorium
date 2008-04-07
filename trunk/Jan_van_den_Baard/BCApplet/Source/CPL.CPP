//
//	cpl.cpp
//
//	(C) Copyright 1999-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Control panel applet entry point.
//  
//

#include "stdafx.h"

#include "bcc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// DllGetVersion function.
HRESULT CALLBACK DllGetVersion( DLLVERSIONINFO *dllver )
{
	dllver->dwMajorVersion = BCC_VERSION;
	dllver->dwMinorVersion = BCC_REVISION;
	dllver->dwBuildNumber  = 0;
	dllver->dwPlatformID   = DLLVER_PLATFORM_WINDOWS | DLLVER_PLATFORM_NT;
	return ( NOERROR );
}

// DLL entry point.
BOOL WINAPI DllMain( HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved )
{
	// Why are we called?
	switch ( dwReason )
	{
		case	DLL_PROCESS_ATTACH:
			// Setup the application.
			return ClsGetApp()->Setup( hModule, NULL, SW_NORMAL, MAKEINTRESOURCE( IDI_MAIN ), MAKEINTRESOURCE( IDS_APP_TITLE ));

		case	DLL_PROCESS_DETACH:
			break;
	}
	return TRUE;
}

// Data made global for easier access.
static HMODULE	hBrainchild;
POOL		pParserPool;
PARSERLIST	plParsers;
SettingsDialog *pSettings;

// Control Panel callback function.
LONG WINAPI CALLBACK CPlApplet( HWND hWnd, UINT uMsg, LPARAM lParam1, LPARAM lParam2 )
{
	// What do they want...
	switch ( uMsg )
	{
		case	CPL_INIT:
			// Load brainchild.dll
			if (( hBrainchild = ::LoadLibrary( _T( "Brainchild.dll" ))) != NULL )
			{
				// Create memory pool.
				if (( pParserPool = ::Brainchild_GetMemoryPool( 4096L )) != NULL )
				{
					// Initialize lists.
					NewList(( LPLIST )&plParsers );

					// Loadup parser files.
					if ( ::Brainchild_LoadParserFiles( pParserPool, &plParsers ) == TRUE )
						return TRUE;
					else
						MessageBox( hWnd, ClsString( MAKEINTRESOURCE( IDS_NO_SETTINGS )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );

					// Free the memory pool.
					::Brainchild_FreeMemoryPool( pParserPool, TRUE );
				}
				else
					MessageBox( hWnd, ClsString( MAKEINTRESOURCE( IDS_NO_MEMORY )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
				
				// Unload the library.
				::FreeLibrary( hBrainchild );
			}
			else
				MessageBox( hWnd, ClsString( MAKEINTRESOURCE( IDS_NO_BRAINCHILD )), ClsGetApp()->GetAppTitle(), MB_ICONERROR | MB_OK );
			return FALSE;

		case	CPL_GETCOUNT:
			// We only support one dialog...
			return 1;

		case	CPL_INQUIRE:
		{
			LPCPLINFO	lpCPlInfo = ( LPCPLINFO )lParam2;

			// Setup the Control Panel
			// information...
			lpCPlInfo->idIcon = IDI_MAIN;
			lpCPlInfo->idInfo = IDS_CPL_INFO;
			lpCPlInfo->idName = IDS_CPL_NAME;
			break;
		}

		case	CPL_DBLCLK:
		{
			// Create a dialog object.
			pSettings = new SettingsDialog;

			// Launch dialog box.
			if ( hWnd )
			{
				ClsWindow Wnd;
				Wnd.Attach( hWnd );
				pSettings->DoModal( Wnd, IDD_MAIN );
			}
			else
				// Popup the dialog.
				pSettings->DoModal( IDD_MAIN );
			break;
		}

		case	CPL_STOP:
			// Free the parser list.
			::Brainchild_FreeParserList( pParserPool, &plParsers );

			// Free the memory pool.
			Brainchild_FreeMemoryPool( pParserPool, TRUE );

			// Unload the library.
			FreeLibrary( hBrainchild );

			// Free the dialog object.
			delete pSettings;
			break;

		case	CPL_EXIT:
			break;

		default:
			break;
	}
	return 0;
}
