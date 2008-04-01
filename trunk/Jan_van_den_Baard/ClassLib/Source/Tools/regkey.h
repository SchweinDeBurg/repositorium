#ifndef _REGKEY_H_
#define _REGKEY_H_
//
// regkey.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"

// A HKEY wrapper.
class ClsRegKey
{
public:
	// Constructor.
	ClsRegKey();

	// Destructor.
	virtual ~ClsRegKey();

	// Implementation.
	LONG OpenKey( HKEY hKey, LPCTSTR pszSubKey, REGSAM samDesired = KEY_ALL_ACCESS );
	LONG CreateKey( HKEY hKey, LPCTSTR pszSubKey, LPTSTR lpClass = REG_NONE, DWORD dwOptions = REG_OPTION_NON_VOLATILE, REGSAM samDesired = KEY_ALL_ACCESS, LPSECURITY_ATTRIBUTES lpSecurityAttributes = NULL, LPDWORD lpdwDisposition = NULL );
	BOOL Attach( HKEY hKey );
	HKEY Detach();
	LONG CloseKey();
	LONG DeleteKey( LPCTSTR pszSubKey );
	LONG DeleteValue( LPCTSTR pszValueName );
	LONG QueryValue( LPCTSTR pszValueName, DWORD& dwValue ) const;
	LONG QueryValue( LPCTSTR pszValueName, LPTSTR pszValue, LPDWORD lpdwCount ) const;
	LONG QueryValue( LPCTSTR pszValueName, LPBYTE pValue, LPDWORD lpdwSize ) const;
	LONG SetValue( DWORD dwValue, LPCTSTR pszValueName = NULL );
	LONG SetValue( LPCTSTR pszValue, LPCTSTR pszValueName = NULL );
	LONG SetValue( LPBYTE pValue, DWORD dwSize, LPCTSTR pszValueName );

protected:
	// Data.
	HKEY	m_hKey;
};

#endif // _REGKEY_H_