#ifndef _SAVEDC_H_
#define _SAVEDC_H_
//
// savedc.h
//
// (C) Copyright 2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "dc.h"

// A simple class which can be used
// to manage the SaveDC() and 
// RestoreDC() APIs.
class ClsSaveDC
{
public:
	// Constructor. Calls the SaveDC() API.
	ClsSaveDC( HDC hDC )
	{
		// Save the DC handle.
		m_hDC = hDC;
		m_nSavedDC = ::SaveDC( hDC );
		_ASSERT( m_nSavedDC != 0 );
	}

	// Destructor. Calls the RestoreDC() API.
	virtual ~ClsSaveDC()
	{
		// Call RestoreDC().
		if ( m_nSavedDC ) ::RestoreDC( m_hDC, m_nSavedDC );
	}

protected:
	// Data.
	HDC		m_hDC;
	int		m_nSavedDC;
};

#endif // _SAVEDC_H_