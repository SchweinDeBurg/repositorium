#ifndef _DIRBROWSER_H_
#define _DIRBROWSER_H_
//
// dirbrowser.h
//
// (C) Copyright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "browser.h"
#include "../../strings/string.h"
#include "../../tools/multimonitor.h"
#include "../../shell/shellmalloc.h"

// A ClsEdit derived class which enables the user
// to enter text and push a selection button for
// directory selection.
class ClsDirBrowser : public ClsBrowser
{
	_NO_COPY( ClsDirBrowser );
public:
	// Constructor.
	ClsDirBrowser() {;}

	// Destructor.
	virtual ~ClsDirBrowser() {;}

	// Implementation.
	inline ClsString& Title()
	{ return ( ClsString& )m_sTitle; }
protected:
	// Structure for the data passed to the callback.
	struct BFFData
	{
		LPCTSTR		pszPath;
		ClsWindow      *pParent;
	};

	// SHBrowseForFolder() callback routine.
	static int CALLBACK BrowseDirectoryCallback( HWND hWnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
	{
		// Evaluate message.
		switch ( uMsg ) 
		{
			case	BFFM_INITIALIZED:
			{
				// Setup path and status text.
				::SendMessage( hWnd, BFFM_SETSELECTION,  TRUE, ( LPARAM )(( BFFData * )lpData )->pszPath ); 
				::SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0,    ( LPARAM )(( BFFData * )lpData )->pszPath );

				// Position dialog.
				ClsRect	rc = (( BFFData * )lpData )->pParent->GetWindowRect();

				// Get screen bounds.
				ClsMultiMon mon;
				ClsRect rcmon;
				int nMonitor;
				mon.MonitorNumberFromWindow( hWnd, MONITOR_DEFAULTTONEAREST, nMonitor );
				mon.GetMonitorRect( nMonitor, rcmon, TRUE );

				// Get the position where to place the
				// dialog.
				int x = rc.Left();
				int y = rc.Bottom();

				// Get the dialog bounds.
				::GetWindowRect( hWnd, rc );

				// Make sure the whole dialog remains visible.
				if (( x + rc.Width())  > rcmon.Right())  x = rcmon.Right() - rc.Width();
				else if ( x < rcmon.Left())	         x = rcmon.Left();
				if (( y + rc.Height()) > rcmon.Bottom()) y = rcmon.Bottom() - rc.Height();
				else if ( y < rcmon.Top())	         y = rcmon.Top();

				// Position dialog...
				::SetWindowPos( hWnd, NULL, x, y, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
				break;
			}

			case	BFFM_SELCHANGED:
			{
				TCHAR		szBuffer[ MAX_PATH ];

				// Obtain path from the passed ITEMIDLIST.
				if ( ::SHGetPathFromIDList(( LPITEMIDLIST )lParam, szBuffer ))
					// Setup the static control.
					::SendMessage( hWnd, BFFM_SETSTATUSTEXT, 0, ( LPARAM )szBuffer );
				break;
			}
		}
		return 0;
	}

	// Open the directory browser.
	void OnBrowserClicked()
	{
		// The docs pertaining to the SHBrowseForFolder() API say
		// we must do this so here we go.
		if ( SUCCEEDED( ::CoInitialize( NULL )))
		{
			// Get allocator.
			ClsShellMalloc sa;

			// OK?
			if ( sa.IsValid())
			{
				// Allocate a buffer to receive browse information.
				LPTSTR	pszBuffer = ( LPTSTR )sa->Alloc( MAX_PATH * sizeof( TCHAR ));
				if ( pszBuffer == NULL )
					return;

				// Copy the current contents.
				GetWindowText( pszBuffer, MAX_PATH );

				// Fill in the data structure.
				BFFData bd;
				bd.pszPath = pszBuffer;
				bd.pParent = this;

				// Fill in the BROWSEINFO structure.
				BROWSEINFO bi;
				bi.hwndOwner      = GetSafeHWND();
				bi.pidlRoot       = NULL;
				bi.pszDisplayName = pszBuffer;
				bi.lpszTitle      = m_sTitle;;
				bi.ulFlags        = BIF_STATUSTEXT;
				bi.lpfn           = BrowseDirectoryCallback;
				bi.lParam         = ( LPARAM )&bd;

				// Browse for a folder and return its PIDL.
				LPITEMIDLIST pidlBrowse = ::SHBrowseForFolder( &bi );

				// OK?
				if ( pidlBrowse != NULL ) 
				{
					// Get selected path.
					if ( ::SHGetPathFromIDList( pidlBrowse, pszBuffer )) 
						// Set the selected directory.
						SetWindowText( pszBuffer );

					// Free the PIDL returned by SHBrowseForFolder.
					sa->Free( pidlBrowse );
				}
				// Clean up.
				sa->Free( pszBuffer );
			}
			// Uninitialize COM.
			::CoUninitialize();
		}
	}

	// File dropped...
	virtual void OnFileDropped( HDROP hDrop )
	{
		// Get the number of files dropped. If there is more than
		// one we use the first.
		if ( UINT nFiles = ::DragQueryFile( hDrop, 0xFFFFFFFF, NULL, 0 ))
		{
			// Get the name of the first file. NOTE: This does
			// not resolve links or shortcuts.
			TCHAR szFile[ MAX_PATH ];
			for ( UINT i = 0; i < nFiles; i++ )
			{
				if ( ::DragQueryFile( hDrop, i, szFile, MAX_PATH ))
				{
					// Is it a directory?
					if (( ::GetFileAttributes( szFile ) & FILE_ATTRIBUTE_DIRECTORY ) == FILE_ATTRIBUTE_DIRECTORY )
					{
						// Set the name.
						SetWindowText( szFile );
						return;
					}
				}
			}
			// Get the name of the first drop and strip it's
			// file component.
			::DragQueryFile( hDrop, 0, szFile, MAX_PATH );
			
			// Strip the file part from the name.
			LPTSTR pFile = ::PathFindFileName( szFile );
			if ( pFile ) *pFile = _T( '\0' );

			// Set the name in the control.
			SetWindowText( szFile );
		}
	}

	// Data
	ClsString	m_sTitle;
};

#endif // _DIRBROWSER_H_