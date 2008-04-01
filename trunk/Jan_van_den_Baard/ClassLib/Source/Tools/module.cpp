//
// module.cpp
//
// (C) Copyright 2000-2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../stdafx.h"

#include "module.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// Constructor. Loads the specified
// library.
ClsModule::ClsModule( LPCTSTR pszLibName )
{
	// Load the library.
	m_hModule = ::LoadLibrary( pszLibName );
}

// Constructor. Loads the specified
// library using the LoadLibraryEx()
// API.
ClsModule::ClsModule( LPCTSTR pszLibName, DWORD dwFlags )
{
	// Load the library.
	m_hModule = ::LoadLibraryEx( pszLibName, NULL, dwFlags );
}

// Destructor. Frees the wrapped module.
ClsModule::~ClsModule()
{
	// Is the module valid?
	if ( m_hModule )
		// Free it.
		::FreeLibrary( m_hModule );
}

// Determine wether the library
// has been loaded correctly.
BOOL ClsModule::IsValid() const
{
	// return success or failure.
	return ( BOOL )( m_hModule ? TRUE : FALSE );
}

// Get the filename of the loaded
// module.
LPCTSTR ClsModule::GetModuleName() const
{
	_ASSERT_VALID( m_hModule ); // Must be valid.

	// Win32 strings are 4Kchars max.
	static TCHAR		szTemp[ 4096 ];

	// Get the name.
	::GetModuleFileName( m_hModule, szTemp, 4096 );

	// Return the name.
	return szTemp;
}		

// Get the address of a procedure.
FARPROC ClsModule::GetProcAddress( LPCSTR pszProcName ) const
{
	_ASSERT_VALID( m_hModule ); // Must be valid.

	// Return the procedure address.
	return ::GetProcAddress( m_hModule, pszProcName );
}

// Operator which returns the handle of
// the loaded library.
ClsModule::operator HMODULE() const
{
	// Return the handle.
	return m_hModule;
}

// Check version numbers.
BOOL ClsModule::CheckModuleVersion( DWORD dwMajor, DWORD dwMinor )
{
	_ASSERT_VALID( m_hModule ); // Must be valid.

	// Check the versioning.
	return CheckModuleVersion( m_hModule, dwMajor, dwMinor );
}

// Check version numbers.
BOOL ClsModule::CheckModuleVersion( HMODULE hModule, DWORD dwMajor, DWORD dwMinor )
{
	_ASSERT_VALID( hModule ); // Must be valid.

	// Preset the result.
	BOOL bRes = TRUE;

	// Only on a valid module.
	if ( hModule ) 
	{
		// You must get this function explicitly because earlier versions of the DLL 
		// don't implement this function. That makes the lack of implementation of the 
		// function a version marker in itself.
		DLLGETVERSIONPROC pDllGetVersion = ( DLLGETVERSIONPROC )::GetProcAddress( hModule, "DllGetVersion" );
      
		// Function present?
		if ( pDllGetVersion ) 
		{ 
			// Initialize structure.
			DLLVERSIONINFO dvi = { 0 };      
			dvi.cbSize = sizeof ( dvi );   
			
			// Call the function.
			if ( SUCCEEDED(( *pDllGetVersion )( &dvi )))
			{ 
				// Major version number OK?
				if ( dvi.dwMajorVersion < dwMajor ) 
					// Major version number is lower
					// than required.
					bRes = FALSE;
				else if ( dvi.dwMajorVersion == dwMajor && dvi.dwMinorVersion < dwMinor )
					// Major version number matches but the
					// minor version number is lower than
					// required.
					bRes = FALSE;
			} 
			else 
				// Function failed so we return FALSE.
				bRes = FALSE;
		} 
		else
			// DllGetVersion function not available so
			// we return FALSE.
			bRes = FALSE;
	} 
	else
		// Not a valid handle...
		bRes = FALSE;

	// Return the result.
        return bRes;
}
