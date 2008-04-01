#ifndef _WINDOWDC_H_
#define _WINDOWDC_H_
//
// windowdc.h
//
// (C) Copyright 2001 Jan van den Baard.
//     All Rights Reserved.
//

#include "dc.h"
#include "..\windows\window.h"

// A HDC wrapper which should is used
// to obtain a window device context.
class ClsWindowDC : public ClsDC
{
public:
	// Constructor. Calls the GetWindowDC()
	// API.
	ClsWindowDC( ClsWindow *pWindow )
	{
		// Save the corresponding window.
		m_pWindow = pWindow;

		// Attach the device context returned by
		// the GetWindowDC() API.
		Attach( ::GetWindowDC( pWindow ? pWindow->GetSafeHWND() : NULL ));
	}

	// Destructor. Detaches the wrapped DC and calls the 
	// ReleaseDC() API.
	virtual ~ClsWindowDC()
	{
		// Call ReleaseDC().
		::ReleaseDC( m_pWindow ? m_pWindow->GetSafeHWND() : NULL, Detach());
	}

protected:
	// Data.
	ClsWindow      *m_pWindow;
};

#endif // _WINDOWDC_H_