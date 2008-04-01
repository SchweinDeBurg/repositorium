#ifndef _BOYERMOORE_H_
#define _BOYERMOORE_H_
//
// boyermoore.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#ifndef _UNICODE
#include "../standard.h"
#include "../strings/string.h"

// A class that implements the boyer-moore search,
// algorithm for forward and backward searches.
class ClsBoyerMoore
{
public:
	// Construction, destruction.
	ClsBoyerMoore();
	ClsBoyerMoore( const ClsBoyerMoore& bm );
	ClsBoyerMoore( LPCSTR pszSearchString, BOOL bCaseOn = FALSE );
	virtual ~ClsBoyerMoore();

	// Implementation.
	int FindForward( char *pData, int nLength );
	int FindBackward( char *pData, int nLength );
	inline void SetSearchString( LPCSTR pszSearchString ) { _ASSERT_VALID( pszSearchString ); m_strSearchString = pszSearchString; SetSearchString(); }
	inline void SetCaseMode( BOOL bCase = TRUE ) { m_bCaseOn = bCase; SetSearchString(); }
	
	// Operator overloads.
	const ClsBoyerMoore& operator=( const ClsBoyerMoore& bm );
	const ClsBoyerMoore& operator=( LPCSTR pszSearchString );
protected:
	// Helpers.
	void SetSearchString();

	// Data.
	ClsString	m_strSearchString;
	BOOL		m_bCaseOn;
	int		m_aDeltas[ 256 ];
	int		m_aBackDeltas[ 256 ];
};

#endif // _BOYERMOORE_H_
#endif // _UNICODE