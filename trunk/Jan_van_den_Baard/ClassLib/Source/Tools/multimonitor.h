#ifndef _MULTIMONITOR_H
#define _MULTIMONITOR_H
//
// multimonitor.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../collectors/array.h"

// API type definitions. I do not link to these
// APIs so the code will also run on operating systems
// which do not support these API's
typedef BOOL ( CALLBACK *ENUMDISPLAYMONITORS )( HDC, LPCRECT, MONITORENUMPROC, LPARAM );
typedef BOOL ( CALLBACK *GETMONITORINFO )( HMONITOR, LPMONITORINFO );
typedef HMONITOR ( CALLBACK *MONITORFROMPOINT )( POINT, DWORD );
typedef HMONITOR ( CALLBACK *MONITORFROMRECT )( LPCRECT, DWORD );
typedef HMONITOR ( CALLBACK *MONITORFROMWINDOW )( HWND, DWORD );

// Special array type. Used to store the
// monitor handles the system reports.
typedef ClsArray< HMONITOR > ClsMonArray;

// A class which to make handling multiple display
// monitors easy. On operating systems which do not
// support multiple display monitors the class always
// uses the primary display monitor.
class ClsMultiMon
{
	_NO_COPY( ClsMultiMon );
public:
	// Construction.
	ClsMultiMon()
	{
		// Initialize multi-monitor support.
		LinkAPIs();
		SetupMonitorArray();
	}

	// Destruction.
	virtual ~ClsMultiMon() {;}

	// Obtain the number of display monitors present
	// in the system.
	inline int NumDisplayMonitors() 
	{
		// When there is no multi-monitor support we always
		// return 1 (primary display monitor)
		return ( m_bMultiMonSupport ? m_aMonitors.GetSize() : 1 ); 
	}

	// OS support for multiple monitors?
	inline BOOL OSMultiMonSupport()
	{
		// Return wether or not we have OS support for multiple
		// monitors.
		return m_bMultiMonSupport;
	}

	// Get the bounding rectangle of the specified display monitor.
	BOOL GetMonitorRect( int nMonitor, LPRECT pRect, BOOL bWork = FALSE )
	{
		_ASSERT( nMonitor >= 0 || nMonitor < NumDisplayMonitors()); // Must be in range.
		_ASSERT_VALID( pRect ); // Must be valid.

		// Multi-monitor support?
		if ( m_bMultiMonSupport )
		{
			// Get information of the specified monitor.
			MONITORINFO mi;
			mi.cbSize = sizeof( mi );
			GETMONITORINFO fnc = ( GETMONITORINFO )m_MonFuncTable[ 1 ];
			if (( *fnc )( m_aMonitors[ nMonitor ], &mi ))
			{
				// Copy rectangle and return success.
				if ( bWork ) *pRect = mi.rcWork;
				else	     *pRect = mi.rcMonitor;
				return TRUE;
			}
			return FALSE;
		}


		// Total or work-area.
		if ( bWork )
			::SystemParametersInfo( SPI_GETWORKAREA, 0, pRect, 0 );
		else
		{
			// Simple rectange of the primary monitor.
			pRect->left = pRect->top = 0;
			pRect->right = ::GetSystemMetrics( SM_CXSCREEN );
			pRect->bottom = ::GetSystemMetrics( SM_CYSCREEN );
		}
		return TRUE;
	}

	// Get the monitor number in which the specified point
	// is located.
	BOOL MonitorNumberFromPoint( POINT pt, DWORD dwFlags, int &nMonitor )
	{
		// Default is the primary monitor.
		nMonitor = 0;

		// Multi-monitor support?
		if ( m_bMultiMonSupport )
		{
			// Call the system API to get the handle.
			MONITORFROMPOINT fnc = ( MONITORFROMPOINT )m_MonFuncTable[ 2 ];
			HMONITOR hMonitor = ( *fnc )( pt, dwFlags );
			if ( hMonitor )
			{
				// Look for the monitor in the array.
				nMonitor = GetMonitorNumber( hMonitor );
				return nMonitor == -1 ? FALSE : TRUE;
			}
		}
		return TRUE;
	}

	// Get the monitor number which intersects with the largest area
	// of the rectangle.
	BOOL MonitorNumberFromRect( LPCRECT pRect, DWORD dwFlags, int &nMonitor )
	{
		// Default is the primary monitor.
		nMonitor = 0;

		// Multi-monitor support?
		if ( m_bMultiMonSupport )
		{
			// Call the system API to get the handle.
			MONITORFROMRECT fnc = ( MONITORFROMRECT )m_MonFuncTable[ 3 ];
			HMONITOR hMonitor = ( *fnc )( pRect, dwFlags );
			if ( hMonitor )
			{
				// Look for the monitor in the array.
				nMonitor = GetMonitorNumber( hMonitor );
				return nMonitor == -1 ? FALSE : TRUE;
			}
		}
		return TRUE;
	}

	// Get the monitor number which intersects with the largest area
	// of the bounding rectangle of the window.
	BOOL MonitorNumberFromWindow( HWND hWnd, DWORD dwFlags, int &nMonitor )
	{
		// Default is the primary monitor.
		nMonitor = 0;

		// Multi-monitor support?
		if ( m_bMultiMonSupport )
		{
			// Call the system API to get the handle.
			MONITORFROMWINDOW fnc = ( MONITORFROMWINDOW )m_MonFuncTable[ 4 ];
			HMONITOR hMonitor = ( *fnc )( hWnd, dwFlags );
			if ( hMonitor )
			{
				// Look for the monitor in the array.
				nMonitor = GetMonitorNumber( hMonitor );
				return nMonitor == -1 ? FALSE : TRUE;
			}
		}
		return TRUE;
	}

private:
	// Get the monitor number of the handle.
	int GetMonitorNumber( HMONITOR hMonitor )
	{
		// Iterate the table.
		for ( int i = 0; i < m_aMonitors.GetSize(); i++ )
		{
			// Is this the one?
			if ( m_aMonitors[ i ] == hMonitor )
				return i;
		}
		// Not found...
		return -1;
	}

	// Call-back procedure which stores the monitor handles.
	static BOOL CALLBACK MonitorInfoProc( HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData )
	{
		// Add the handle to the array.
		ClsMonArray *pMonitorArray = reinterpret_cast< ClsMonArray * >( dwData );
		pMonitorArray->Add( hMonitor );
		return TRUE;
	}

	// Setup the monitor array.
	void SetupMonitorArray()
	{
		// Is there multi-monitor support?
		if ( m_bMultiMonSupport )
		{
			// Empty the array.
			m_aMonitors.RemoveAll();

			// Call the EnumDisplayMonitors() API with a call back function which
			// stores the information of display monitor in the system.
			ENUMDISPLAYMONITORS fnc = ( ENUMDISPLAYMONITORS )m_MonFuncTable[ 0 ];
			( *fnc )( NULL, NULL, MonitorInfoProc, reinterpret_cast< LPARAM >( &m_aMonitors ));
		}
	}

	// Runtime-linking of APIs.
	void LinkAPIs()
	{
		// No multi-monitor support by default.
		m_bMultiMonSupport = FALSE;

		// Get the handle of the User32.dll.
		HMODULE hUserLib = ::GetModuleHandle( _T( "user32.dll" ));
		if ( hUserLib != NULL )
		{
			// Link APIs.
			m_MonFuncTable[ 0 ] = GetProcAddress( hUserLib, "EnumDisplayMonitors" );
#ifdef _UNICODE
			m_MonFuncTable[ 1 ] = GetProcAddress( hUserLib, "GetMonitorInfoW" );
#else
			m_MonFuncTable[ 1 ] = GetProcAddress( hUserLib, "GetMonitorInfoA" );
#endif
			m_MonFuncTable[ 2 ] = GetProcAddress( hUserLib, "MonitorFromPoint" );
			m_MonFuncTable[ 3 ] = GetProcAddress( hUserLib, "MonitorFromRect" );
			m_MonFuncTable[ 4 ] = GetProcAddress( hUserLib, "MonitorFromWindow" );

			// OK?
			for ( int i = 0; i < 5; i++ )
			{
				if ( m_MonFuncTable[ i ] == NULL )
					return;
			}
			
			// Multi-monitor support OK.
			m_bMultiMonSupport = TRUE;
		}

		return;
	}

	// Data.
	ClsMonArray	m_aMonitors;
	FARPROC		m_MonFuncTable[ 5 ];
	BOOL		m_bMultiMonSupport;
};

#endif // _MULTIMONITOR_H