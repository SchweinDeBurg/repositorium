//
// mru.cpp
//
// (C) Copyright 2000-2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "mru.h"
#include "../menus/menuiteminfo.h"
#include "regkey.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor.
ClsMRU::ClsMRU()
{
	// Default data.
	m_hParent	   = NULL;
	m_nMaxEntries	   = 10;
	m_nBaseID	   = 0xDEAD;
	m_strEmptyItemName = _T( "<Empty>" );
	m_Menu.OldStyle()  = TRUE;
	m_bUseIcons	   = FALSE;
}

// Destructor.
ClsMRU::~ClsMRU()
{
	m_Menu.Destroy();
}

// Setup the object.
BOOL ClsMRU::SetupMRU( HKEY hKey, LPCTSTR pszSubKey, UINT nBaseID /* = 0xDEAD */, int nMax /* = 10 */ )
{
	_ASSERT( nMax >= 4 && nMax <= 10 ); // Make sure we stay in range.
	_ASSERT_VALID( hKey ); // Required.
	_ASSERT_VALID( pszSubKey ); // Required.

	// Store values.
	m_hParent     = hKey;
	m_strSubKey   = pszSubKey;
	m_nMaxEntries = nMax;
	m_nBaseID     = nBaseID;

	// Clear counter.
	m_nCounter = 0;

	// Setup menu image list. Since the system imagelist is a per-process
	// shared resource I'm not sure if what I do below is legal. I change the
	// background color of the imagelist to CLR_NONE...
	m_Menu.SetImageList( ClsGetApp()->GetSystemImageList());

	// Create the menu...
	if ( m_Menu.Create( TRUE ))
	{
		// We are a popup...
		m_Menu.IsPopupMenu() = TRUE;

		// Set item info.
		ClsMenuItemInfo mi;
		mi.fMask  = MIIM_TYPE | MIIM_ID;
		mi.fType  = MFT_STRING;

		// Read the entries.
		for ( int i = 0; i < m_nMaxEntries; i++ )
		{
			// Get the entry...
			ClsString str;
			if ( GetMRUEntry( i, str ))
			{
				// Build item.
				mi.wID	      = i + m_nBaseID;
				mi.dwTypeData = str;
				mi.cch	      = str.GetStringLength();

				// Add the item.
				if ( m_Menu.InsertItem(( UINT )-1, mi, TRUE ) == FALSE )
					return FALSE;

				// Increase counter.
				m_nCounter++;
			}
		}

		// Any recent entries?
		if ( m_nCounter == 0 )
		{
			// No. Add the empty item.
			if ( AddEmptyItem() == FALSE )
				return FALSE;
		} 

		// Convert the menus.
		if ( m_Menu.ConvertMenus() == FALSE )
			return FALSE;
		if ( SetupIcons() == FALSE )
			return FALSE;

		return TRUE;
	}
	return FALSE;
}

// Add an entry to the MRU list.
BOOL ClsMRU::AddMRUEntry( LPCTSTR pszPath )
{
	// Valid?
	if ( pszPath == NULL || *pszPath == _T( '\0' ))
		return FALSE;

	// If the file name is already
	// located in the recent list
	// we do not add it again.
	if ( NewMRUEntry( pszPath ) == FALSE )
		return TRUE;

	// Build name.
	TCHAR szPath[ MAX_PATH + 4 ];
#if (_MSC_VER < 1500)
	_stprintf( szPath, _T( "&0 %s" ), pszPath );
#else
	_stprintf_s( szPath, _countof( szPath ), _T( "&0 %s" ), pszPath );
#endif   // _MSC_VER

	// Setup structure.
	ClsMenuItemInfo mi;
	mi.fMask      = MIIM_TYPE;
	mi.fType      = MFT_STRING;
	mi.dwTypeData = szPath;
	mi.cch        = ( UINT )_tcslen( szPath );

	// Insert the menu item.
	if ( m_Menu.InsertItem( 0, mi, TRUE ))
	{
		// Already any entries?
		if ( m_nCounter == 0 )
		{
			// No. Delete the empty item.
			if ( m_Menu.DeleteMenu( 1, TRUE ) == FALSE )
				return FALSE;
		}

		// Did we reach the maximum number
		// of entries?
		if ( m_nCounter == m_nMaxEntries )
		{
			// Delete recent entry.
			if ( m_Menu.DeleteMenu( m_nMaxEntries, TRUE ) == FALSE )
				return FALSE;
		}
		else
			// Increase recent counter.
			m_nCounter++;

		// Renumber the menus.
		if ( RenumberMRUList() == FALSE )
			return FALSE;
	}
	else
		return FALSE;

	// Convert menus.
	if ( m_Menu.ConvertMenus() == FALSE )
		return FALSE;
	if ( SetupIcons() == FALSE )
		return FALSE;

	return TRUE;
}

// Remove an entry from the MRU list.
BOOL ClsMRU::RemoveMRUEntry( LPCTSTR pszPath )
{
	// Valid?
	if ( pszPath == NULL || *pszPath == _T( '\0' ))
		return FALSE;

	// Set item info.
	ClsString str;
	
	// Iterate the entries.
	for ( int i = 0; i < m_nMaxEntries; i++ )
	{
		// Get item text.
		if ( m_Menu.GetItemText( i, str, TRUE ))
		{
			// Is this the one?
			str = str.Right( str.GetStringLength() - 3 );
			if ( str.CompareNoCase( pszPath ) == 0 )
			{
				// Remove the entry from the
				// recent file list.
				if ( m_Menu.DeleteMenu( i, TRUE ) == FALSE )
					return FALSE;

				// Decrease counter.
				m_nCounter--;

				// Any recent entries?
				if ( m_nCounter == 0 ) 
				{
					// Add empty item.
					if ( AddEmptyItem() == FALSE )
						return FALSE;
				}
				else	
					// Renumber list.
					RenumberMRUList();

				// Convert menus.
				if ( m_Menu.ConvertMenus() == FALSE )
					return FALSE;
				if ( SetupIcons() == FALSE )
					return FALSE;

				// Done.
				return TRUE;
			}
		}
	}
	return FALSE;
}

// Select a MRU entry. Does nothing more than moving
// the entry to the top of the list.
BOOL ClsMRU::SelectMRUEntry( LPCTSTR pszPath )
{
	// Remove the entry from the list.
	if ( RemoveMRUEntry( pszPath ))
		// And add it again.
		return AddMRUEntry( pszPath );
	return FALSE;
}

// Get the path of the MRU entry.
BOOL ClsMRU::GetMRUEntryPath( UINT nID, ClsString& str )
{
	// Obtain item name.
	ClsString strFullName;
	if ( m_Menu.GetItemText( nID, strFullName ))
	{
		// Extract path name.
		str = strFullName.Right( strFullName.GetStringLength() - 3 );
		return TRUE;
	}
	return FALSE;
}

// Save the MRU list to the registry.
BOOL ClsMRU::SaveMRUList()
{
	// First we clear the old entries.
	ClearMRUList();

	// Open or create the key.
	ClsRegKey key;
	if ( key.CreateKey( m_hParent, m_strSubKey ) == ERROR_SUCCESS )
	{
		ClsString str;
		TCHAR 	  szEntry[ 10 ];

		// Iterate the recent menus.
		for ( int i = 0; i < m_nCounter; i++ )
		{
			// Obtain item name.
			if ( m_Menu.GetItemText( i, str, TRUE ))
			{
				// Build key name.
#if (_MSC_VER < 1500)
				_stprintf( szEntry, _T( "MRU%ld" ), i );
#else
				_stprintf_s( szEntry, _countof( szEntry ), _T( "MRU%ld" ), i );
#endif   // _MSC_VER

				// Set the key.
				str = str.Right( str.GetStringLength() - 3 );
				if ( key.SetValue( str, szEntry ) != ERROR_SUCCESS )
					return FALSE;
			}
			else
				return FALSE;
		}
	}
	else
		return FALSE;
	return TRUE;
}

// Add the empty item.
BOOL ClsMRU::AddEmptyItem()
{
	// Setup item information.
	ClsMenuItemInfo mi;
	mi.fMask      = MIIM_TYPE | MIIM_STATE | MIIM_ID;
	mi.fType      = MFT_STRING;
	mi.fState     = MFS_GRAYED;
	mi.wID	      = m_nBaseID + m_nMaxEntries;
	mi.dwTypeData = ( LPTSTR )m_strEmptyItemName;
	mi.cch	      = m_strEmptyItemName.GetStringLength();

	// Add the item.
	return m_Menu.InsertItem(( UINT )-1, mi, TRUE );
}

// Renumber the MRU list.
BOOL ClsMRU::RenumberMRUList()
{
	// Iterate the menu.
	ClsMenuItemInfo mi;
	ClsString str;

	for ( int i = 0; i < m_Menu.GetItemCount(); i++ )
	{
		// Setup fields to retrieve.
		mi.fMask      = MIIM_ID;

		// Get the data.
		if ( m_Menu.GetItemInfo( i, mi, TRUE ) && m_Menu.GetItemText( i, str, TRUE ))
		{
			/*
			 *	Not the empty item!
			 */
			if ( mi.wID != m_nBaseID + m_nMaxEntries )
			{
				// Change the number.
				str[ 1 ] = ( TCHAR )( i + 0x30 );

				// Setup the menu text.
				if ( m_Menu.SetItemText( i, str, TRUE ) == FALSE )
					return FALSE;

				// Renumber ID.
				mi.wID = i + m_nBaseID;
				if ( m_Menu.SetItemInfo( i, mi, TRUE ) == FALSE )
					return FALSE;
			}
		}
		else
			return FALSE;
	}
	return TRUE;
}

// Determine if the path is already
// in the MRU list.
BOOL ClsMRU::NewMRUEntry( LPCTSTR pszPath )
{
	// Iterate menu.
	ClsString str;	
	for ( int i = 0; i < m_Menu.GetItemCount(); i++ )
	{
		// Get the entry name.
		if ( m_Menu.GetItemText( i, str, TRUE ))
		{
			// Present?
			str = str.Right( str.GetStringLength() - 3 );
			if ( _tcsicmp( pszPath, str ) == 0 )
				// Yes. Return FALSE so that
				// the AddRecentEntry() function will
				// not add this name to the
				// recent file menu.
				return FALSE;
		}
		else
			return FALSE;
	}
	return TRUE;
}

// Clear the current registry contents.
void ClsMRU::ClearMRUList()
{
	// Open the key.
	ClsRegKey	key;
	if ( key.OpenKey( m_hParent, m_strSubKey ) == ERROR_SUCCESS )
	{
		// Iterate the entries.
		TCHAR		szEntry[ 10 ];
		for ( int i = 0; i < m_nMaxEntries; i++ )
		{
			// Build subkey name.
#if (_MSC_VER < 1500)
			_stprintf( szEntry, _T( "MRU%ld" ), i );
#else
			_stprintf_s( szEntry, _countof( szEntry ), _T( "MRU%ld" ), i );
#endif   // _MSC_VER

			// Delete it.
			key.DeleteValue( szEntry );
		}
	}
}

// Read an entry from the registry.
BOOL ClsMRU::GetMRUEntry( int nNum, ClsString& str )
{
	// Build key name.
	TCHAR szEntry[ 10 ];
#if (_MSC_VER < 1500)
	_stprintf( szEntry, _T( "MRU%ld" ), nNum );
#else
	_stprintf_s( szEntry, _countof( szEntry ), _T( "MRU%ld" ), nNum );
#endif   // _MSC_VER

	// Pre-contruct menu name.
	TCHAR szPath[ MAX_PATH + 4 ];
#if (_MSC_VER < 1500)
	_stprintf( szPath, _T( "&%ld " ), nNum );
#else
	_stprintf_s( szPath, _countof( szPath ), _T( "&%ld " ), nNum );
#endif   // _MSC_VER

	// Open the key.
	ClsRegKey key;
	if ( key.OpenKey( m_hParent, m_strSubKey ) == ERROR_SUCCESS ) 
	{
		// Read the value.
		DWORD dwSize = MAX_PATH;
		if ( key.QueryValue( szEntry, ( LPTSTR )&szPath[ 3 ], &dwSize ) == ERROR_SUCCESS )
		{
			// Valid data?
			if ( _tcslen( szPath ) == 3 )
				return FALSE;

			// Store name.
			str = szPath;
			return TRUE;
		}
	}
	return FALSE;
}

// Get the index of the file-icon.
int ClsMRU::GetIconIndex( LPCTSTR pszPath )
{ 
	// Get the index of the icon.
	SHFILEINFO fi;
	if ( ::SHGetFileInfo( pszPath, 0, &fi, sizeof( fi ), SHGFI_ICON | SHGFI_SMALLICON | SHGFI_USEFILEATTRIBUTES ) == 0 )
		return -1;
	
	// Destroy the icon and return the index.
	::DestroyIcon( fi.hIcon );
	return fi.iIcon;
}

// Setup the icons vor the menu in it's present
// status.
BOOL ClsMRU::SetupIcons()
{ 
	// Iterate the items.
	if (( HMENU )m_Menu )
	{
		for ( int i = 0; i < m_Menu.GetItemCount(); i++ )
		{
			// Obtain the path name of the item.
			ClsString str;
			if ( GetMRUEntryPath( m_nBaseID + i, str ))
			{
				// Set it's icon index.
				if ( m_Menu.SetItemBitmap( i, m_bUseIcons ? GetIconIndex( str ) : -1, TRUE ) == FALSE )
					return FALSE;
			}
		}
	}
	return TRUE;
}