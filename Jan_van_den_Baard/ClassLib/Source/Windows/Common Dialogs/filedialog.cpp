//
// filedialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "filedialog.h"
#include "../../application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// For Windows ME and 2000 the OPENFILENAME structure has been extended
// with the fields defined below. To get the "Places" bar in the dialog
// we need to set the "lStructSize" field to the size of this extended
// structure.
//
// Problem with this is that the code will fail on systems which do not
// support this larger structure like Windows 98.
//
// To circumvent this problem two new constants are defined below:
//
// OFN_SIZEOF     - This will always be the old structure size.
// OFN_SIZEOF_EXT - This will always be the new structure size.
//
// In the code the structure used will always have the new, extended,
// size but the "lStructSize" field will be set to OFN_SIZEOF or
// OFN_SIZEOF_EXT depending on the version of the OS the code runs on.
//
// Normally when an old-sized structure is supplied you will automatically
// get the new "Places" bar except when the OFN_ENABLEHOOK flag is
// used. Since a hook is used in the code  this approach is choosen.
//
// See: http://msdn.microsoft.com/library/periodic/period00/c0400.htm
//      for more information on this topic.
//
typedef struct tagEx
{
	void *pvReserved;
	DWORD dwReserved;
	DWORD dwFlagsEx;
} OFNEXT;

#if ( _WIN32_WINNT < 0x0500 )
	// Extend the old structure with the new fields.
	//
	// The code in the class will use the OPENFILENAMEEX structure.
	typedef struct tagOFNEx
	{
		OPENFILENAME	ofn;
		OFNEXT		ext;
	} OPENFILENAMEEX, *LPOPENFILENAMEEX;

	// Define the structure size constants.
	#define OFN_SIZEOF sizeof( OPENFILENAME )
	#define OFN_SIZEOF_EXT sizeof( OPENFILENAMEEX )
#else
	// The code in the class will use the OPENFILENAMEEX structure.
	typedef struct tagOFNEx
	{
		OPENFILENAME	ofn;
	} OPENFILENAMEEX, *LPOPENFILENAMEEX;

	// Define the structure size constants.
	#define OFN_SIZEOF sizeof( OPENFILENAME ) - sizeof( OFNEXT )
	#define OFN_SIZEOF_EXT sizeof( OPENFILENAME )
#endif

// Get the file name. When the dialog is a multi-select dialog
// you should call this routine until it returns 0 to get all
// names. I.E.
//
// ClsFileDialog dlg;
//
// if ( dlg.DoModal( ... ))
// {
//	ClsString name;
//	int nIndex = dlg.GetName( 0, name );
//	if ( nIndex )
//	{
//		do
//		{
//			// Do something with the name.
//		} while (( nIndex = dlg.GetName( nIndex, name )) > 0 );
//	}
// }
//
//
// For a single select dialog a simple call to GetName() is enough
// to get the selected file name although the above code will also
// work for a single select dialog.
//
int ClsFileDialog::GetName( int nIndex, ClsString& str ) const
{
	// Copy the string part.
	return m_strFile.GetDelimitedPart( _T( '\t' ), nIndex, str );
}

// Popup the dialog.
BOOL ClsFileDialog::DoModal( ClsWindow *parent, LPCTSTR pszDir, LPCTSTR pszFile, BOOL bOpenDialog /* = TRUE */, DWORD dwFlags /* = 0 */, int x /* = CW_USEDEFAULT */, int y /* = CW_USEDEFAULT */ )
{
	// Setup location.
	m_ptPos.X() = x;
	m_ptPos.Y() = y;

	// We need a lot of room in case of a multi-selection
	// dialog, 8KChars should suffice...
	TCHAR		sz[ 8192 ];

	// Was a directory passed?
	if ( pszDir == NULL )
	{
		// No. Do we have one left from the
		// previous call?
		if ( m_strDir.GetStringLength() == 0 )
		{
			// No. Use the current directory...
			GetCurrentDirectory( 8192, sz );
			m_strDir = sz;
		}
	}
	else
		// Copy the dir.
		m_strDir = pszDir;

	// Was a file name passed?
	if ( pszFile == NULL )
	{
		// No. Do we have one left from the
		// previous call?
		if ( m_strFile.GetStringLength() == 0 )
			// No file...
			sz[ 0 ] = _T( '\0' );
		else
		{
			// Copy previous name unless it was a
			// multi-select file name.
			if ( ! _tcschr( m_strFile, _T( '\t' )))
				_tcsncpy( sz, ( LPCTSTR )m_strFile, 8192 );
			else
				sz[ 0 ] = _T( '\0' );
		}
	}
	else
		// Copy the file name.
		_tcscpy( sz, pszFile );

	// Open or save?
	if ( ! bOpenDialog )
	{
		dwFlags &= ~OFN_ALLOWMULTISELECT;
		dwFlags |= OFN_OVERWRITEPROMPT;
	}

	// Determine which structure size we should use. We
	// default to the old size.
	DWORD dwStructSize = OFN_SIZEOF;

	// If we are running ME or an OS with a version of 5 or
	// higher we use the new structure size.
	if ( ClsGetApp()->GetMajorVersion() >= 5 ||
		( ClsGetApp()->GetMajorVersion() == 4 && ClsGetApp()->GetMinorVersion() == 90 ))
		// Use the new structure size.
		dwStructSize = OFN_SIZEOF_EXT;

	// Setup the OPENFILENAMEEX structure.
	OPENFILENAMEEX ofn;
	memset(( void * )&ofn, 0, sizeof( ofn ));
	ofn.ofn.lStructSize	= dwStructSize;
	ofn.ofn.hwndOwner	= parent ? parent->GetSafeHWND() : NULL;
	ofn.ofn.lpstrFilter	= m_pszFilters;
	ofn.ofn.nFilterIndex	= m_nFilterIndex;
	ofn.ofn.lpstrFile	= sz;
	ofn.ofn.nMaxFile	= 8192;
	ofn.ofn.lpstrInitialDir	= m_strDir;
	ofn.ofn.lpstrTitle	= m_strCaption;
	ofn.ofn.Flags		= dwFlags | OFN_ENABLEHOOK | OFN_EXPLORER;
	ofn.ofn.lpstrDefExt	= m_strDefExt;
	ofn.ofn.lCustData	= ( DWORD )this;
	ofn.ofn.lpfnHook	= HookProc;

	// Call the PreShowDialog() override.
	if ( PreShowDialog( &ofn.ofn ))
	{
		// Open the dialog.
		if (( bOpenDialog ? GetOpenFileName : GetSaveFileName )( &ofn.ofn ))
		{
			// Save the filter index.
			m_nFilterIndex = ofn.ofn.nFilterIndex;

			// Was it multi-select?
			if ( dwFlags & OFN_ALLOWMULTISELECT )
			{
				// Was only one file selected?
				HANDLE	hFile = ::CreateFile( sz,
							      GENERIC_READ,
							      FILE_SHARE_READ,
							      NULL,
							      OPEN_EXISTING,
							      FILE_ATTRIBUTE_NORMAL | FILE_FLAG_SEQUENTIAL_SCAN,
							      NULL );

				// If the file could be opened it means that
				// there was only a single selection.
				if ( hFile != INVALID_HANDLE_VALUE )
				{
					// Close the handle.
					CloseHandle( hFile );

					// Save the file name.
					m_strFile = &sz[ ofn.ofn.nFileOffset ];
					sz[ ofn.ofn.nFileOffset - 1 ] = _T( '\0' );
					m_strDir = sz;
				}
				else
				{
					// First we copy the path.
					m_strDir = sz;
					m_strFile.Empty();

					// We replace all 0-bytes with a tab with the
					// exception of the last.
					TCHAR	*psz = &sz[ ofn.ofn.nFileOffset ];

					// Iterate the string.
					for ( ;; )
					{
						// Is it a 0-byte?
						if ( *psz == _T( '\0' ))
						{
							// Change it into a tab.
							*psz = _T( '\t' );
							psz++;
						}

						// If we get another 0-byte can exit the loop.
						if ( *psz == _T( '\0' ))
							break;

						// Next...
						psz = _tcsinc( psz );
					}

					// Now we simply copy the string to the file
					// string. You can use the GetName() routine
					// to access the names.
					m_strFile = &sz[ ofn.ofn.nFileOffset ];
				}
			}
			else
			{
				// Save the file name.
				m_strFile = &sz[ ofn.ofn.nFileOffset ];
				sz[ ofn.ofn.nFileOffset - 1 ] = _T( '\0' );
				m_strDir = sz;
			}
			return TRUE;
		}
		return FALSE;
	}
	return FALSE;
}

// Hook procedure. This will attach the dialog
// to the object.
UINT CALLBACK ClsFileDialog::HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// The lParam parameter of the WM_INITDIALOG message holds a pointer
	// to the OPENFILENAME structure. This structure holds in the "lCustData"
	// field the pointer to the object we need to get attached to.
	if ( uMsg == WM_INITDIALOG )
	{
		_ASSERT_VALID( lParam ); // Must be valid.
		ClsFileDialog *pFileDialog = ( ClsFileDialog * )(( LPOPENFILENAME )lParam )->lCustData;
		_ASSERT_VALID( pFileDialog ); // Must be valid.

		// Attach us to the object.
		pFileDialog->Attach( hWnd );
	}
	return 0;
}

// WM_NOTIFY message handler. Dispatches to the virtual
// notification handlers.
LRESULT ClsFileDialog::OnNotify( LPNMHDR pNMHDR )
{
	// Evaluate message code.
	switch ( pNMHDR->code )
	{
		case	CDN_FILEOK:	  return OnFileOK(( LPOFNOTIFY )pNMHDR );
		case	CDN_FOLDERCHANGE: return OnFolderChange(( LPOFNOTIFY )pNMHDR );
		case	CDN_HELP:	  return OnHelp(( LPOFNOTIFY )pNMHDR );
		case	CDN_INITDONE:
		{
			// Re-position the dialog if necessary.
			RePositionDialog( GetParent() );
			return OnInitDone(( LPOFNOTIFY )pNMHDR );
		}

		case	CDN_SHAREVIOLATION:
		{
			// Call the virtual handler.
			LRESULT rc = OnShareViolation(( LPOFNOTIFY )pNMHDR );

			// Anything other than OFN_SHAREWARN?
			if ( rc != OFN_SHAREWARN )
			{
				// Set the dialog result.
				SetWindowLong64( DWL_MSGRESULT, ( LONG_PTR )rc );
				return TRUE;
			}
			return FALSE;
		}

		case	CDN_TYPECHANGE: return OnTypeChange(( LPOFNOTIFY )pNMHDR );
	}
	// Call the base class.
	return ClsCommonDialog::OnNotify( pNMHDR );
}