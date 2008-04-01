//
// regkey.cpp
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "regkey.h"
#include "../application.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Set's the handle to NULL.
ClsRegKey::ClsRegKey()
{
	// Clear handle.
	m_hKey = NULL;
}

// Destructor. Closes the wrapped key.
ClsRegKey::~ClsRegKey()
{
	// If we have a valid handle
	// close it.
	CloseKey();
}

// Open a key.
LONG ClsRegKey::OpenKey( HKEY hKey, LPCTSTR pszSubKey, REGSAM samDesired /* = KEY_ALL_ACCESS */ )
{
	_ASSERT( m_hKey == NULL ); // May not be valid.

	// Open the key.
	return ::RegOpenKeyEx( hKey, pszSubKey, 0, samDesired, &m_hKey );
}

// Create a key.
LONG ClsRegKey::CreateKey( HKEY hKey, LPCTSTR pszSubKey, LPTSTR lpClass /* = REG_NONE */, DWORD dwOptions /* = REG_OPTION_NON_VOLATILE */, REGSAM samDesired /* = KEY_ALL_ACCESS */, LPSECURITY_ATTRIBUTES lpSecurityAttributes /* = NULL */, LPDWORD lpdwDisposition /* = NULL */ )
{
	_ASSERT( m_hKey == NULL ); // May not be valid.

	// Create the key.
	return ::RegCreateKeyEx( hKey, pszSubKey, 0, lpClass, dwOptions, samDesired, lpSecurityAttributes, &m_hKey, lpdwDisposition );
}

// Attach a key to the object.
BOOL ClsRegKey::Attach( HKEY hKey )
{
	_ASSERT( m_hKey == NULL ); // May not be valid.
	_ASSERT_VALID( hKey ); // Must be valid.

	// Set the handle.
	if ( m_hKey == NULL )
	{
		m_hKey = hKey;
		return TRUE;
	}
	return FALSE;
}

// Detach the key. Note: detaching the key will
// not close it.
HKEY ClsRegKey::Detach()
{
	_ASSERT_VALID( m_hKey ); // Must be valid.

	// Get the wrapped key.
	HKEY hResult = m_hKey;

	// Clear the member.
	m_hKey = NULL;

	// Return the handle.
	return hResult;
}

// Close the wrapped key.
LONG ClsRegKey::CloseKey()
{
	// Success by default.
	LONG rc = ERROR_SUCCESS;

	// Key valid?
	if ( m_hKey )
	{
		// Close the Key.
		rc = ::RegCloseKey( m_hKey );
		if ( rc == ERROR_SUCCESS )
			// Clear the handle member.
			m_hKey = NULL;
	}
	return rc;
}

// Delete a key and all of it's sub-keys.
LONG ClsRegKey::DeleteKey( LPCTSTR pszSubKey )
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszSubKey ); // Must be valid.

	// Return code.
	LONG lResult;

	// Are we running NT?
	if ( ClsGetApp()->GetPlatformID() == VER_PLATFORM_WIN32_WINDOWS )
		// We are not running NT so a simple call 
		// to the RegDeleteKey() API is enough.
		lResult = ::RegDeleteKey( m_hKey, pszSubKey );
	else
	{
		// Open the key.
		ClsRegKey key;
		if (( lResult = key.OpenKey( m_hKey, pszSubKey )) != ERROR_SUCCESS )
			return lResult;

		// Needed for RegEnumKeyEx().
		FILETIME	ftime;
		TCHAR		szKey[ MAX_PATH ];
		DWORD		dwSize = MAX_PATH;

		// Enummerate the subkeys.
		while ( ::RegEnumKeyEx( key.m_hKey, 0, szKey, &dwSize, NULL, NULL, NULL, &ftime ) == ERROR_SUCCESS )
		{
			// Delete the subkey it's subkeys.
			if (( lResult = key.DeleteKey( szKey )) != ERROR_SUCCESS )
				return lResult;

			// Set back the size since it was changed.
			dwSize = MAX_PATH;
		}

		// Close the key.
		key.CloseKey();

		// Delete the parent.
		lResult = ::RegDeleteKey( m_hKey, pszSubKey );
	}
	return lResult;
}

// Delete a value.
LONG ClsRegKey::DeleteValue( LPCTSTR pszValueName )
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszValueName ); // Must be valid.

	// Delete the value.
	return ::RegDeleteValue( m_hKey, pszValueName );
}

// Query a 32 bit value.
LONG ClsRegKey::QueryValue( LPCTSTR pszValueName, DWORD& dwValue ) const
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszValueName ); // Must be value.

	// Presets.
	DWORD dwType = REG_DWORD;
	DWORD dwSize = sizeof( DWORD );

	// Query the value.
	return ::RegQueryValueEx( m_hKey, pszValueName, NULL, &dwType, ( LPBYTE )&dwValue, &dwSize );
}

// Query a 0-terminated string.
LONG ClsRegKey::QueryValue( LPCTSTR pszValueName, LPTSTR pszValue, LPDWORD lpdwCount ) const
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszValueName ); // Must be value.

	// Presets.
	DWORD dwType = REG_SZ;

	// Query the value.
	return ::RegQueryValueEx( m_hKey, pszValueName, NULL, &dwType, ( LPBYTE )pszValue, lpdwCount );
}

// Query a byte array.
LONG ClsRegKey::QueryValue( LPCTSTR pszValueName, LPBYTE pValue, LPDWORD lpdwSize ) const
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszValueName ); // Must be value.

	// Presets.
	DWORD dwType = REG_NONE;

	// Query the value.
	return ::RegQueryValueEx( m_hKey, pszValueName, NULL, &dwType, pValue, lpdwSize );
}

// Set a 32 bit value.
LONG ClsRegKey::SetValue( DWORD dwValue, LPCTSTR pszValueName /* = NULL */ )
{
	_ASSERT_VALID( m_hKey ); // Must be valid.

	// Set the value.
	return ::RegSetValueEx( m_hKey, pszValueName, 0, REG_DWORD, ( LPBYTE )&dwValue, sizeof( DWORD ));
}

// Set a 0-terminated string.
LONG ClsRegKey::SetValue( LPCTSTR pszValue, LPCTSTR pszValueName /* = NULL */ )
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pszValue ); // Must be valid.

	// Set the value.
	return ::RegSetValueEx( m_hKey, pszValueName, 0, REG_SZ, ( LPBYTE )pszValue, ( DWORD )_tcslen( pszValue ) + 1 );
}

// Set a byte array.
LONG ClsRegKey::SetValue( LPBYTE pValue, DWORD dwSize, LPCTSTR pszValueName /* = NULL */ )
{
	_ASSERT_VALID( m_hKey ); // Must be valid.
	_ASSERT_VALID( pValue ); // Must be valid.

	// Set the value.
	return ::RegSetValueEx( m_hKey, pszValueName, 0, REG_NONE, pValue, dwSize );
}