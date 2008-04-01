//
// filedirtree.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../../stdafx.h"

#include "filedirtree.h"
#include "../../gdi/paintdc.h"
#include "../../gdi/bufferdc.h"
#include "../../gdi/getdc.h"
#include "../../gdi/selector.h"
#include "../../tools/findfile.h"
#include "../../tools/waitcursor.h"
#include "../../collectors/stringarray.h"
#include "../../exceptions/memoryexception.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Setup defaults.
ClsFileDirTree::ClsFileDirTree()
{
	// Setup defaults.
	m_dwDontShow	 = FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_TEMPORARY | FILE_ATTRIBUTE_OFFLINE;
	m_bShowFiles	 = TRUE;
	m_bDoubleBuffer  = TRUE;
	m_bSilent	 = FALSE;
	m_strFilter	 = _T( "*.*" );
	m_strLoading	 = _T( "Loading..." );
	m_crLoadingColor = CLR_DEFAULT;
}

// Destructor. Does nothing.
ClsFileDirTree::~ClsFileDirTree()
{
}

// Show the tree.
BOOL ClsFileDirTree::SetupTree( LPCTSTR pszRoot /* = NULL */ )
{
	_ASSERT( GetSafeHWND() ); // Must be valid.

	// Make sure the tree is empty...
	if ( ! m_bSilent ) SetRedraw( FALSE );
	SelectItem( NULL );
	DeleteAllItems();
	if ( ! m_bSilent ) SetRedraw();
	
	// Do we need to set the images?
	if ( GetImageList( TVSIL_NORMAL ) != ClsGetApp()->GetSystemImageList())
		SetImageList( ClsGetApp()->GetSystemImageList(), TVSIL_NORMAL );

	// Make sure we are the LVS_EDITLABELS style is not
	// specified.
	ModifyStyle( LVS_EDITLABELS, 0 );

	// Use a specific root?
	if ( pszRoot && _tcslen( pszRoot ))
	{
		// Copy initial root.
		m_strInitial = pszRoot;

		// Make sure the backslash is present.
		ClsString str = m_strInitial;
		if ( str.GetStringLength() && str[ str.GetStringLength() - 1 ] != _T( '\\' )) str += _T( '\\' );

		// Add the root item.
		HTREEITEM hItem = AddTreeItem( TVI_ROOT, str, FALSE );
		if ( hItem == NULL )
			return FALSE;

		// Does it have any files/dirs?
		if ( HasEntries( hItem ))
		{
			// Add a single empty item.
			if ( AddTreeItem( hItem, _T( "" )) == NULL )
				return FALSE;
		}
		return TRUE;
	}

	// No specific root. Add all logical drives.
	return DisplayDrives();
}

// Turn the showing of files on or off.
void ClsFileDirTree::ShowFiles( BOOL bShow /* = TRUE */ )
{
	// We must have contents.
	if ( GetRoot() == NULL ) 
	{
		// Simply set the value.
		m_bShowFiles = bShow;
		return;
	}

	// Did the mode change?
	if ( bShow != m_bShowFiles )
	{
		// Turn of rendering.
		SetRedraw( FALSE );
		m_bSilent = TRUE;
			
		// Set new value.
		m_bShowFiles = bShow;
		
		// Get current selection.
		ClsString path;
		GetSelectedPath( path );

		// Reset the contents.
		SetupTree( m_strInitial );

		// Select the path.
		SelectPath( path );

		// Re-render.
		m_bSilent = FALSE;
		SetRedraw( TRUE );
		UpdateWindow();
	}
}

// Obtain the full path of the selected entry.
BOOL ClsFileDirTree::GetSelectedPath( ClsString& strPath ) const
{
	_ASSERT( GetSafeHWND() ); // Must be valid.

	// Get info about the selected entry.
	TVITEM tvi;
	tvi.mask = TVIF_STATE;
	tvi.hItem = GetSelection();

	if ( GetItem( tvi ))
	{
		// Obtain it's path.
		strPath = GetPath( tvi.hItem );

		// Is it a drive?
		if ( tvi.lParam )
			return TRUE;

		// Can we get it's attributes?
		DWORD dwAttr = ::GetFileAttributes( strPath );
		if ( dwAttr == 0xFFFFFFFF || ( ! ( dwAttr & FILE_ATTRIBUTE_DIRECTORY )))
		{
			// Does it have a pending backslash?
			if ( strPath[ strPath.GetStringLength() - 1 ] == _T( '\\' )) 
			{
				// Strip it and try again.
				strPath.SetStringLength( strPath.GetStringLength() - 1 );
				if (( dwAttr = ::GetFileAttributes( strPath )) == 0xFFFFFFFF )
					return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}

// Select and display the path.
BOOL ClsFileDirTree::SelectPath( LPCTSTR pszPath )
{
	return SelectPath( ClsString( pszPath ));
}

BOOL ClsFileDirTree::SelectPath( const ClsString& strPath )
{
	_ASSERT( GetSafeHWND() ); // Must be valid.

	// 4KChars max.
	TCHAR	szBuffer[ 4096 ];

	// Setup the structure to obtain the item
	// text.
	TVITEM	tvi;
	tvi.mask	= TVIF_TEXT | TVIF_PARAM;
	tvi.pszText	= szBuffer;

	// We start at the root.
	HTREEITEM hParent = TVI_ROOT;

	// Maximum index into the path.
	int nMax = strPath.GetStringLength() - 1;

	// Setup data.
	int nPos = 0;
	ClsString strComp;
	HTREEITEM hSelected = NULL;

	// Iterate the path to get it's components.
	for ( ;; )
	{
		// Get the index of the next backslash.
		int nPos2 = strPath.Find( _T( '\\' ), nPos );

		// Extract the path component.
		strComp = strPath.Mid( nPos, nPos2 == -1 ? nPos2 : nPos2 - nPos );

		// Obtain the first child item.
		HTREEITEM hChild = GetChild( hParent );
		if ( hChild )
		{
			// Iterate children.
			do
			{
				// Setup structure.
				tvi.hItem      = hChild;
				tvi.cchTextMax = 4096;

				// Obtain item name.
				if ( GetItem( tvi ))
				{
					// Is it a drive name?
					if ( tvi.lParam )
					{
						// Build "real" path string.
						_tcscpy( szBuffer, _T( " :" ));

						// Add drive letter.
						szBuffer[ 0 ] = ( TCHAR )( tvi.lParam );
					}

					// Is this the item we are looking for?
					if ( _tcsicmp( szBuffer, strComp ) == 0 )
					{
						// Expand the item.
						Expand( hChild, TVE_EXPAND );

						// Make this child the new parent.
						hParent = hChild;

						// Were we processing the final
						// part of the path?
						if ( nPos2 == -1 || nPos2 == nMax )
						{
							// Mark the selected item.
							hSelected = hChild;

							// Yes. Select the item.
							SelectItem( hChild );
						}
						break;
					}
					// Get the next child.
					hChild = GetNextSibling( hChild );
				}
				else
					// Failure...
					break;
			} while ( hChild );
		}

		// Did we process the last part of the path?
		if ( nPos2 == -1 || nPos2 == nMax )
		{
			// Did we select an item?
			if ( hSelected == NULL )
			{
				// No. Simply select the parent if it is
				// not the root.
				if ( hParent != TVI_ROOT )
					SelectItem( hParent );
			}
			break;
		}

		// Skip to the next part of the path.
		nPos = nPos2 + 1;
	}
	return TRUE;
}

// Determine the type of the selection
BOOL ClsFileDirTree::TypeOfSelection() const
{
	_ASSERT( GetSafeHWND() ); // Must be valid.

	// Any selection?
	if ( GetSelection())
	{
		// Get the path.
		ClsString strPath;
		if ( GetSelectedPath( strPath ))
		{
			// Determine wether it is a file or a directory.
			DWORD dwAttr = ::GetFileAttributes( strPath );
			if ( dwAttr == 0xFFFFFFFF )
			{
				// Does it have a pending backslash?
				if ( strPath[ strPath.GetStringLength() - 1 ] == _T( '\\' )) 
				{
					// Strip it and try again.
					strPath.SetStringLength( strPath.GetStringLength() - 1 );
					if (( dwAttr = ::GetFileAttributes( strPath )) == 0xFFFFFFFF )
						return STYPE_UNKNOWN;
				}
			}

			// Is it a directory?
			if ( dwAttr & FILE_ATTRIBUTE_DIRECTORY ) return STYPE_DIRECTORY;
			else					 return STYPE_FILE;
		}
	}
	return STYPE_NONE;
}

// Add a treeview item.
HTREEITEM ClsFileDirTree::AddTreeItem( HTREEITEM hParent, LPCTSTR pszPath, BOOL bParse /* = TRUE */, LPCTSTR pszDriveName /* = NULL */ )
{
	TVINSERTSTRUCT tvi;
	SHFILEINFO fi;

	// Get the index of the normal icon.
	if ( ::SHGetFileInfo( pszPath, 0, &fi, sizeof( fi ), SHGFI_ICON | SHGFI_SMALLICON ) == 0 )
		return NULL;
	
	// Store the index.
	tvi.item.iImage = fi.iIcon;
	::DestroyIcon( fi.hIcon );

	// Get the index of the open icon.
	if ( ::SHGetFileInfo( pszPath, 0, &fi, sizeof( fi ), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_OPENICON ) == 0 )
		return NULL;

	// Store the index.
	tvi.item.iSelectedImage = fi.iIcon;
	::DestroyIcon( fi.hIcon );

	// Is the path valid?
	ClsString strPath = pszPath;
	if ( bParse )
	{
		int nLen = strPath.GetStringLength();
		if ( nLen )
		{
			// If the path ends with a back-slash we remove
			// it.
			if ( strPath[ nLen - 1 ] == _T( '\\' ))
				strPath.SetStringLength( nLen - 1 );

			// Now we look for the right-most backslash. If it exists
			// we copy the string right from that back slash leaving us
			// with the last part of the path.
			//
			// If it does not exist we are left with the drive name. In this
			// case we append a backslash to it.
			int nPos = strPath.ReverseFind( _T( '\\' ));
			if ( nPos != -1 ) strPath = strPath.Mid( nPos + 1 );
			else		  strPath += _T( '\\' );
		}
	}

	// Did we get a drive name?
	if ( pszDriveName )
	{
		// Bits 24 through 31 represent the drive letter.
		tvi.item.lParam = strPath[ 0 ];

		// Simply let the drive name be the name
		// shownd to the user.
		strPath = pszDriveName;
	}
	else
		tvi.item.lParam = 0;

	// Setup the rest of the structure.
	tvi.hParent	 = hParent;
	tvi.hInsertAfter = NULL;
	tvi.item.mask	 = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
	tvi.item.pszText = ( LPTSTR )strPath;

	// Insert the item.
	return InsertItem( tvi );
}

// Add the drive entries.
BOOL ClsFileDirTree::DisplayDrives()
{
	// Delete current contents.
	DeleteAllItems();

	// Get the available logical drives.
	DWORD dwDrives = ::GetLogicalDrives(), dwMask = 1;
	for ( int i = 0; i < 32; i++, dwMask <<= 1 )
	{
		// Drive available?
		if ( dwDrives & dwMask )
		{
			// Create drive string.
			ClsString drive;
			drive.Format( _T( "%c:\\" ), _T( 'A' ) + i );

			// We do not want to see error dialogs...
			DWORD dwOldMode = SetErrorMode( SEM_FAILCRITICALERRORS );

			// Obtain display name.
			SHFILEINFO sfi;
			SHGetFileInfo( drive, 0, &sfi, sizeof( sfi ), SHGFI_DISPLAYNAME );

			// Reset error mode.
			SetErrorMode( dwOldMode );

			// Add the drive to the list.
			HTREEITEM hDrive;
			if (( hDrive = AddTreeItem( TVI_ROOT, drive, TRUE, sfi.szDisplayName )) == NULL )
				return FALSE;

			// Add an empty item so we get the LVN_ITEMEXPANDED
			// notification message which we use to load or
			// remove the directory entries.
			if ( AddTreeItem( hDrive, _T( "" )) == NULL )
				return FALSE;
		}
	}
	return TRUE;
}

// Obtain the full path of the item.
ClsString ClsFileDirTree::GetPath( HTREEITEM hItem ) const
{
	// 4KChars max.
	TCHAR	szBuffer[ 4096 ];

	// Setup the structure to obtain the item
	// text.
	TVITEM	tvi;
	tvi.mask	= TVIF_TEXT | TVIF_PARAM;
	tvi.pszText	= szBuffer;

	// Iterate the items until we reached the
	// root.
	ClsString strPath = "";
	while ( hItem )
	{
		// Set item.
		tvi.hItem	= hItem;
		tvi.cchTextMax  = 4096;

		// Get item name.
		if ( GetItem( tvi ))
		{
			// Is it a drive name?
			if ( tvi.lParam )
			{
				// Build "real" path from the drive letter.
				_tcscpy( szBuffer, _T( " :\\" ));

				// Add the drive letter.
				szBuffer[ 0 ] = ( TCHAR )( tvi.lParam );
			} 
			else 
				// Append the backslash if necessary.
				if ( szBuffer[ _tcslen( szBuffer ) - 1 ] != _T( '\\' ))
					_tcsncat( szBuffer, _T( "\\" ), 4096 );

			// Insert the name of the item at the start
			// of the path string.
			strPath.Insert( 0, szBuffer );

			// Get the parent item
			hItem = GetParentItem( hItem );
		}
		else
			break;
	}
	return strPath;
}

// Add the directory to the parent.
BOOL ClsFileDirTree::AddDirectory( HTREEITEM hParent )
{
	// Put up the wait cursor.
	ClsWaitCursor wait;

	// Get the full path of the directory.
	ClsString strPath = GetPath( hParent );
	
	// Add the "*.*" filter.
	if ( strPath[ strPath.GetStringLength() - 1 ] != _T( '\\' )) strPath += _T( '\\' );
	strPath += _T( "*.*" );

	// Scan the directory.
	ClsFindFile	find;
	ClsStringArray	aDirs;
	if ( find.FindFile( strPath ))
	{
		do
		{
			// We only get the directories.
			if ( ! ( find.Attributes() & m_dwDontShow ) && ! find.IsDots() && find.IsDirectory())
			{
				// Get the file name.
				ClsString strName;
				if ( find.GetFilePath( strName ))
					// Add it to the directory array.
					aDirs.Add( strName );
			}
		} while ( find.FindNextFile());
		
		// Close the handle.
		find.Close();
	}

	// Device not ready?
	if ( ::GetLastError() == ERROR_NOT_READY )
		return FALSE;

	// File string array.
	ClsStringArray	aFiles;

	// Show files?
	if ( m_bShowFiles )
	{
		// If there is no filter we use the *.* filter.
		if ( m_strFilter.GetStringLength() == 0 ) 
			m_strFilter = _T( "*.*" );

		// Get files.
		ClsString str;
		int nIndex = m_strFilter.GetDelimitedPart( _T( ';' ), 0, str );
		if ( nIndex > 0 )
		{
			// Scan using all filters.
			do
			{
				// Get the full path of the directory.
				strPath = GetPath( hParent );
				
				// Add the filter.
				if ( strPath[ strPath.GetStringLength() - 1 ] != _T( '\\' )) strPath += _T( '\\' );
				strPath += str;

				// Scan the directory.
				if ( find.FindFile( strPath ))
				{
					do
					{
						// We only show the correct files.
						if ( ! ( find.Attributes() & m_dwDontShow ) && ! find.IsDots() && ! find.IsDirectory())
						{
							// Get the file name.
							ClsString strName;
							if ( find.GetFilePath( strName ))
								// Add it to the file array.
								aFiles.Add( strName );
						}
					} while ( find.FindNextFile());
					// Close the handle.
					find.Close();
				}
			// Next filter...
			} while (( nIndex = m_strFilter.GetDelimitedPart( _T( ';' ), nIndex, str )) > 0 );
		}
	}

	// Sort both arrays.
	aDirs.Sort();
	aFiles.Sort();

	// Preset result.
	BOOL bRC = TRUE;

	// No rendering.
	if ( ! m_bSilent ) SetRedraw( FALSE );

	// Add the directories.
	for ( int i = 0; i < aDirs.GetSize(); i++ )
	{
		// Add the directory entry.
		HTREEITEM hItem = AddTreeItem( hParent, aDirs[ i ] );
		if ( hItem == NULL )
		{
			bRC = FALSE;
			break;
		}

		// Is the directory empty?
		if ( HasEntries( hItem ))
		{
			// Add a single, empty, item so we get a LVN_ITEMEXPANDED
			// notification.
			if ( AddTreeItem( hItem, _T( "" )) == NULL )
			{
				bRC = FALSE;
				break;
			}
		}
	}

	// Show files?
	if ( m_bShowFiles )
	{
		// Add the files.
		for ( int i = 0; i < aFiles.GetSize(); i++ )
		{
			// Add the entry.
			if ( AddTreeItem( hParent, aFiles[ i ] ) == NULL )
			{
				bRC = FALSE;
				break;
			}
		}
	}

	// Re-render.
	if ( ! m_bSilent ) 
	{
		SetRedraw();
		UpdateWindow();
	}
	return bRC;
}

// Return TRUE if the directory has files or sub-directories
// and FALSE otherwise.
BOOL ClsFileDirTree::HasEntries( HTREEITEM hItem )
{
	// Get the full path of the directory.
	ClsString strPath = GetPath( hItem );
	
	// Add the *.* filter for a directory search.
	if ( strPath[ strPath.GetStringLength() - 1 ] != _T( '\\' )) strPath += _T( '\\' );
	strPath += _T( "*.*" );

	// We do not want to see error dialogs...
	DWORD dwOldMode = SetErrorMode( SEM_FAILCRITICALERRORS );

	// See if there are any sub directories.
	ClsFindFile find;
	if ( find.FindFile( strPath ))
	{
		do
		{
			// Get the name.
			ClsString strName;
			find.GetFileName( strName );

			// Is it a directory?.
			if ( ! ( find.Attributes() & m_dwDontShow ) && ! find.IsDots() && strName.GetStringLength() && find.IsDirectory())
				return TRUE;
		} while ( find.FindNextFile());
	}

	// Reset error mode.
	SetErrorMode( dwOldMode );

	// Device not ready?
	if ( ::GetLastError() == ERROR_NOT_READY )
		return FALSE;
	
	// Close the handle.
	find.Close();

	// Do we need to scan for files?
	if ( m_bShowFiles )
	{
		// If there is no filter we use the *.* filter.
		if ( m_strFilter.GetStringLength() == 0 ) 
			m_strFilter = _T( "*.*" );

		// Get files.
		ClsString str;
		int nIndex = m_strFilter.GetDelimitedPart( _T( ';' ), 0, str );
		if ( nIndex > 0 )
		{
			// Use all filters.
			do
			{
				// Get the full path of the directory.
				strPath = GetPath( hItem );
				
				// Add the filter.
				if ( strPath[ strPath.GetStringLength() - 1 ] != _T( '\\' )) strPath += _T( '\\' );
				strPath += str;

				// See if there are any sub entries.
				ClsFindFile find;
				if ( find.FindFile( strPath ))
				{
					do
					{
						// Get the name.
						ClsString strName;
						find.GetFileName( strName );

						// We only show the correct files.
						if ( ! ( find.Attributes() & m_dwDontShow ) && ! find.IsDots() && strName.GetStringLength())
							return TRUE;
					} while ( find.FindNextFile());
					// Close the handle.
					find.Close();
				}
			// Next filter...
			} while (( nIndex = m_strFilter.GetDelimitedPart( _T( ';' ), nIndex, str )) > 0 );
		}
	}
	return FALSE;
}

// By default nothing is done here.
void ClsFileDirTree::OnDriveEmpty( ClsString strPath )
{
}

// The default way of rendering the item displayed
// during the loading of the tree. When this overridable
// is called the device context has already been setup 
// to use the treeview font.
void ClsFileDirTree::OnRenderLoadingItem( ClsDC *pDC, ClsRect crBounds )
{
	// Setup the color to use for rendering the
	// loading item text. If the color specified equals
	// -1 we get the control it's text color. If this 
	// equals -1 aswell we use the system window text color.
	COLORREF color = ( m_crLoadingColor == CLR_DEFAULT ? GetTextColor() : m_crLoadingColor );
	if ( color == CLR_DEFAULT ) color = ::GetSysColor( COLOR_WINDOWTEXT );
	COLORREF crold = pDC->SetTextColor( color );

	// Obtain font height.
	ClsSize size = pDC->GetTextExtent( _T( " " ), 1 );

	// Render the text...
	pDC->TextOut( crBounds.Left() + 2, crBounds.Top() + (( crBounds.Height() / 2 ) - ( size.CY() / 2 )), m_strLoading );

	// Restore text color.
	pDC->SetTextColor( crold );
}

// Handle the TVN_ITEMEXPANDING notification.
LRESULT ClsFileDirTree::OnItemExpanding( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
{
	// Obtain the item data.
	TVITEM tvi;
	tvi.mask  = TVIF_PARAM;
	tvi.hItem = pTreeView->itemNew.hItem;
	if ( GetItem( tvi ))
	{
		// Get the entry it's drive type.
		ClsString strPath = GetPath( pTreeView->itemNew.hItem );

		// Are we expanding?
		if ( ! ( pTreeView->itemNew.state & TVIS_EXPANDED ))
		{
			// Is it a drive?
			if ( tvi.lParam )
			{
				// Any entries?
				if ( HasEntries( pTreeView->itemNew.hItem ) == FALSE )
				{
					// Call the overidable.
					OnDriveEmpty( strPath );
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

// Handle the TVN_ITEMEXPANDED notification.
LRESULT ClsFileDirTree::OnItemExpanded( LPNMTREEVIEW pTreeView, BOOL& bNotifyParent )
{
	// Obtain the item data.
	TVITEM tvi;
	tvi.mask  = TVIF_PARAM;
	tvi.hItem = pTreeView->itemNew.hItem;
	if ( GetItem( tvi ))
	{
		// Did it exand?
		if ( pTreeView->itemNew.state & TVIS_EXPANDED )
		{
			// Force a redraw of the
			if ( ! m_bSilent ) 
			{
				SetRedraw();
				UpdateWindow();
			}

			// Compute the rectangle of the text label
			// including the icon.
			ClsRect rc, rctot;
			HTREEITEM hChild = GetChild( pTreeView->itemNew.hItem );
			GetItemRect( hChild, rctot, FALSE );
			GetItemRect( hChild, rc, TRUE );
			rc.Right() = rctot.Right();
			rc.Left() -= ::GetSystemMetrics( SM_CXSMICON ) + 3;

			// Prepare a device context for the rendering.
			{
				// Get a DC.
				ClsGetDC	dc( this );

				// Device context valid?
				if ( dc.IsValid())
				{
					// Setup the treeview font.
					ClsFont		font;
					GetFont( font );
					ClsSelector	sel( &dc, font );

					// Erase the item background.
					ClsBrush brush;
					COLORREF color = GetBkColor();
					if ( color == ( COLORREF )-1 ) color = ::GetSysColor( COLOR_WINDOW );
					brush.CreateSolidBrush( color );
					dc.FillRect( rc, &brush );

					// Call the overridable.
					OnRenderLoadingItem( &dc, rc );
				}
			}

			// Delete sub-items.
			hChild = GetChild( pTreeView->itemNew.hItem );
			while ( hChild )
			{
				DeleteItem( hChild );
				hChild = GetChild( pTreeView->itemNew.hItem );
			}
			
			// Add directory.
			AddDirectory( pTreeView->itemNew.hItem );
		}
		else
		{
			// Delete sub-items.
			HTREEITEM hChild = GetChild( pTreeView->itemNew.hItem );
			while ( hChild )
			{
				DeleteItem( hChild );
				hChild = GetChild( pTreeView->itemNew.hItem );
			}

			// Add a single empty item so the parent displays
			// the expansion box.
			AddTreeItem( pTreeView->itemNew.hItem, _T( "" ));
		}
	}
	return 0;
}

// Window procedure override.
LRESULT ClsFileDirTree::WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	// The one we need?
	if ( uMsg == WM_PRINTCLIENT )
	{
		// Render what?
		if ( lParam & PRF_ERASEBKGND ) SendMessage( WM_ERASEBKGND, wParam );
		if ( lParam & PRF_CLIENT     ) SendMessage( WM_PAINT, wParam );
		return 0;
	}
	// Base class.
	return ClsTreeView::WindowProc( uMsg, wParam, lParam );
}

// We handle the background painting in the
// WM_PAINT message handler if the double buffering
// flag is truned on.
//
// If it is not we return -1 which tells the message
// handler to pass the message to the default message
// handler.
LRESULT ClsFileDirTree::OnEraseBkgnd( ClsDC *pDC )
{
	return m_bDoubleBuffer ? 0 : -1;
}

// WM_PAINT message handler. Will paint the tree contents
// into a buffer DC if appropiate.
LRESULT ClsFileDirTree::OnPaint( ClsDC *pDC )
{
	// Use double buffered rendering?
	if ( m_bDoubleBuffer )
	{
		// Use SEH to catch memory problems
		try
		{
			// Get the client rectangle.
			ClsRect rc = GetClientRect();

			// If the input device context is valid we
			// use it to create a buffer off. Otherwise we
			// use the DC created by the ClsPaintDC class
			// to create a buffer off.
			BOOL bCreated = FALSE;
			ClsDC *pdc;
			if ( pDC )
				// Use the supplied DC.
				pdc = pDC;
			else	  
			{ 
				// Use the DC create by the ClsPaintDC object.
				// The DC is the one returned by the BeginPaint()
				// API.
				pdc = new ClsPaintDC( this ); 
				bCreated = TRUE; 
			}


			// Preset the return code.
			LRESULT lResult = -1;

			// Brace the code so that the buffer DC runs
			// out of scope before the ClsPaintDC object
			// is deleted.
			{
				// Create DC buffer.
				ClsBufferDC dc( *pdc, rc );

				// Render the background.
				SendMessage( WM_ERASEBKGND, ( WPARAM )( HDC )dc );

				// Call the default procedure to render the tree contents.
				lResult = WindowProc( WM_PAINT, ( WPARAM )( HDC )dc, 0 );
			}

			// Delete the DC if necessary.
			if ( bCreated ) delete pdc;

			// Return the result.
			return lResult;
		}

		// If a memory problem occures in the above code
		// -1 is returned which calls the default message
		// handler.
		catch ( ClsMemoryException& me )
		{
			me;
		}
	}
	return -1;
}