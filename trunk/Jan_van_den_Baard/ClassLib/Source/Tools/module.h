#ifndef _MODULE_H_
#define _MODULE_H_
//
// module.h
//
// (C) Copyright 2000-2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "../strings/string.h"

// A class which handles the loading and unloading
// of dynamic link libraries.
class ClsModule
{
	// No copy constructor.
	_NO_COPY( ClsModule );
public:
	// Constructors.
	ClsModule( LPCTSTR pszLibName );
	ClsModule( LPCTSTR pszLibName, DWORD dwFlags );

	// Destructor.
	virtual ~ClsModule();

	// Interface.
	BOOL IsValid() const ;
	LPCTSTR GetModuleName() const;
	FARPROC GetProcAddress( LPCSTR pszProcName ) const;
	BOOL CheckModuleVersion( DWORD dwMajor, DWORD dwMinor );
	static BOOL CheckModuleVersion( HMODULE hModule, DWORD dwMajor, DWORD dwMinor );

	// Operator overloads;
	operator HMODULE() const;

protected:
	// The wrapped handle.
	HMODULE		m_hModule;
};

#endif // _MODULE_H_