#ifndef _SELECTOR_H_
#define _SELECTOR_H_
//
// selector.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"
#include "dc.h"

// Simple class which keeps track of object
// selections in a device context.
class ClsSelector
{
	// No copy constructor.
	_NO_COPY( ClsSelector );
public:
	// Constructor. Select the object 
	// into a device context.
	ClsSelector( ClsDC *pDC, HGDIOBJ hObject )
	{
		// Save the DC.
		m_pDC = pDC;

		// Select the object saving the current
		// object.
		if ( pDC ) 
			m_hObject = pDC->SelectObject( hObject );
	}

	// Destructor. Restores the old object.
	~ClsSelector()
	{
		// Only if this one is valid.
		if ( m_pDC )
			m_pDC->SelectObject( m_hObject );
	}

protected:
	// Data.
	ClsDC  *m_pDC;
	HGDIOBJ	m_hObject;
};

#endif // _SELECTOR_H_