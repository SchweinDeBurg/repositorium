#ifndef _FILEBROWSER_H_
#define _FILEBROWSER_H_
//
// filebrowser.h
//
// (C) Copyright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "browser.h"
#include "../common dialogs/filedialog.h"
#include "../../strings/string.h"

// A ClsEdit derived class which enables the user
// to enter text and push a selection button for
// file selection.
class ClsFileBrowser : public ClsBrowser
{
	_NO_COPY( ClsFileBrowser );
public:
	// Constructor.
	ClsFileBrowser() {;}

	// Destructor.
	virtual ~ClsFileBrowser() {;}

	// Implementation.
	inline ClsString& Title() { return ( ClsString& )m_sTitle; }
	inline ClsString& Filters() { return ( ClsString& )m_sFilters; }

protected:
	// Overidables.
	virtual void OnBrowserClicked()
	{
		ClsFileDialog	fd;
		ClsString	sPath( *this ), sFile;

		// Find the last backslash.
		int nPos = sPath.ReverseFind(_T('\\'));
		if ( nPos >= 0 )
		{
			// Copy the file part.
			sFile = sPath.Right( sPath.GetStringLength() - ( nPos + 1 ));

			// Truncate the path part.
			sPath.SetStringLength( nPos + 1 );
		}
		else
		{
			// The whole thing is a file name.
			sFile = sPath;
			sPath = _T( "" );
		}

		// Setup the dialog title and filter string.
		fd.Caption() = m_sTitle;
		fd.Filters() = m_sFilters;

		// Convert filters.
		for ( int i = 0; i < m_sFilters.GetStringLength(); i++ )
		{
			// Replace newlines with null's.
			if ( m_sFilters[ i ] == _T( '\n' ))
				m_sFilters[ i ] = _T( '\0' );
		}

		// Get the position of the control.
		ClsRect	rc = GetWindowRect();

		// Open the file dialog just beneath it.
		if ( fd.DoModal( this, sPath, sFile, TRUE, OFN_ENABLESIZING, rc.Left(), rc.Bottom()))
		{
			// Get the file name and the selected
			// directory.
			ClsString fname, dname = fd.Dir();

			// Get the name of the selected file.
			if ( fd.GetName( 0, fname ))
			{
				// Append a backslash to the directory
				// if it is not present.
				if ( dname[ dname.GetStringLength() - 1 ] != _T( '\\' ))
					dname += _T( '\\' );

				// Append file name to the directory name.
				dname += fname;

				// Set full path.
				SetWindowText( dname );
			}
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

			// Get the first file.
			for ( UINT i = 0; i < nFiles; i++ )
			{
				if ( ::DragQueryFile( hDrop, i, szFile, MAX_PATH ))
				{
					// Is it a directory?
					if (( ::GetFileAttributes( szFile ) & FILE_ATTRIBUTE_DIRECTORY ) != FILE_ATTRIBUTE_DIRECTORY )
					{
						// Set the file name in the control.
						SetWindowText( szFile );
						return;
					}
				}
			}
			// No files dropped.
			MessageBeep( 0xFFFFFFFF );
		}
	}

	// Data.
	ClsString	m_sTitle, m_sFilters;
};

#endif // _FILEBROWSER_H_