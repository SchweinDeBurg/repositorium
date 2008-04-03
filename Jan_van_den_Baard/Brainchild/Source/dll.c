/*
 *	dll.c
 *
 *	(C) Copyright 1999-2005 Jan van den Baard.
 *	    All Rights Reserved.
 * Enchancements (c) 2008 by Elijah Zarezky.
 *
 *	DLL entry point.
 */

#include "stdafx.h"

#include "defs.h"

/*
 *	DllGetVersion function...
 */
HRESULT DllGetVersion( DLLVERSIONINFO *dllver )
{
	dllver->dwMajorVersion = BCVERSION;
	dllver->dwMinorVersion = BCREVISION;
	dllver->dwBuildNumber  = 0;
	dllver->dwPlatformID   = DLLVER_PLATFORM_WINDOWS | DLLVER_PLATFORM_NT;

	return ( NOERROR );
}

/*
 *	Module handle.
 */
HMODULE		hDLL;

/*
 *	This message is sent to all top level windows
 *	using HWND_BROADCAST when the settings are
 *	changed.
 *
 *	The DLL creates one single, invisible, window
 *	it uses to receive and process this message.
 */
UINT		uRegisteredMsg;

/*
 *	Main entry point.
 */
BOOL WINAPI DllMain( HINSTANCE hModule, DWORD dwReason, LPVOID lpReserved )
{
	/*
	 *	Why are we here?
	 */
	switch ( dwReason )
	{
		case	DLL_PROCESS_ATTACH:
			{
				WNDCLASS	wndClass;

				/*
				 *	Initialize the common
				 *	controls library.
				 */
				InitCommonControls();

				/*
				 *	Save off handle.
				 */
				hDLL = hModule;

				/*
				 *	Setup IPC class.
				 */
				wndClass.style		= 0;
				wndClass.lpfnWndProc	= ( WNDPROC )ComProc;
				wndClass.cbClsExtra	= 0;
				wndClass.cbWndExtra	= 0;
				wndClass.hInstance	= hModule;
				wndClass.hIcon		= NULL;
				wndClass.hCursor	= NULL;
				wndClass.hbrBackground	= NULL;
				wndClass.lpszMenuName	= ( LPCTSTR )NULL;
				wndClass.lpszClassName  = ( LPCTSTR )IPCCLASS;

				/*
				 *	Register it.
				 */
				if ( ! RegisterClass( &wndClass ))
					return FALSE;

				/*
				 *	Register the window message.
				 */
				uRegisteredMsg = RegisterWindowMessage( BRAINCHILD_CLASS );
				if ( uRegisteredMsg < 0xC000 && uRegisteredMsg > 0xFFFF )
				{
					UnregisterClass( IPCCLASS, hModule );
					return FALSE;
				}

				/*
				 *	Setup a new process.
				 */
				if ( SetupProcess() == FALSE )
				{
					UnregisterClass( IPCCLASS, hModule );
					return FALSE;
				}

				/*
				 *	Setup our class.
				 */
				wndClass.style		= CS_HREDRAW | CS_VREDRAW | CS_GLOBALCLASS | CS_DBLCLKS;
				wndClass.lpfnWndProc	= ( WNDPROC )Dispatch;
				wndClass.cbClsExtra	= 0;
				wndClass.cbWndExtra	= 4;
				wndClass.hInstance	= hModule;
				wndClass.hIcon		= NULL;
				wndClass.hCursor	= NULL;
				wndClass.hbrBackground	= NULL;
				wndClass.lpszMenuName	= ( LPCTSTR )NULL;
				wndClass.lpszClassName  = ( LPCTSTR )BRAINCHILD_CLASS;

				/*
				 *	Register it.
				 */
				if ( ! RegisterClass( &wndClass ))
				{
					KillProcess();
					UnregisterClass( IPCCLASS, hModule );
					return FALSE;
				}

				/*
				 *	Setup OLE libraries.
				 */
				if ( FAILED( OleInitialize( NULL )))
				{
					KillProcess();
					UnregisterClass( IPCCLASS, hModule );
					UnregisterClass( BRAINCHILD_CLASS, hModule );
					return FALSE;
				}

				break;
			}

		case	DLL_PROCESS_DETACH:
			/*
			 *	Free process data.
			 */
			KillProcess();

			/*
			 *	Free Ole libraries.
			 */
			OleUninitialize();

			/*
			 *	Unregister our classes.
			 */
			UnregisterClass( BRAINCHILD_CLASS, hModule );
			UnregisterClass( IPCCLASS, hModule );

			/*
			 *	Memory leaks?
			 */
#ifdef _DEBUG
			if ( _CrtDumpMemoryLeaks())
			{
				if (( 1 == _CrtDbgReport( _CRT_ERROR, __FILE__, __LINE__, NULL, "Memory leak detected!", 0 )))
					_CrtDbgBreak();
			}
#endif
			break;
	}
	return TRUE;
}
