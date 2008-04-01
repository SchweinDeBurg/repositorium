#ifndef _GETDC_H_
#define _GETDC_H_
//
// getdc.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "dc.h"
#include "..\windows\window.h"

// A HDC wrapper which should be used
// to obtain a window device context.
class ClsGetDC : public ClsDC
{
public:
	// Constructor. Calls the GetDC()
	// API.
	ClsGetDC( ClsWindow *pWindow )
	{
		// Save the corresponding window.
		m_pWindow = pWindow;

		// Attach the device context returned by
		// the GetDC() API.
		Attach( ::GetDC( pWindow ? pWindow->GetSafeHWND() : NULL ));
	}

	// Constructor. Calls the GetDCEx()
	// API.
	ClsGetDC( ClsWindow *pWindow, HRGN hRgn, DWORD dwFlags )
	{
		// Save the corresponding window.
		m_pWindow = pWindow;

		// Attach the device context returned by
		// the GetDC() API.
		Attach( ::GetDCEx( pWindow ? pWindow->GetSafeHWND() : NULL, hRgn, dwFlags ));
	}

	// Destructor. Detaches the wrapped DC and calls the 
	// ReleaseDC() API.
	virtual ~ClsGetDC()
	{
		// Call ReleaseDC() and detach it.
		::ReleaseDC( m_pWindow ? m_pWindow->GetSafeHWND() : NULL, Detach());
	}

protected:
	// Data.
	ClsWindow      *m_pWindow;
};

#endif // _GETDC_H_