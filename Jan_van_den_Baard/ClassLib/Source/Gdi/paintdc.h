#ifndef _PAINTDC_H_
#define _PAINTDC_H_
//
// paintdc.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "dc.h"
#include "..\windows\window.h"

// A HDC wrapper which should be used
// during the processing of a WM_PAINT message.
class ClsPaintDC : public ClsDC
{
public:
	// Constructor. Calls the BeginPaint()
	// API to setup rendering.
	ClsPaintDC( ClsWindow *pWindow )
	{
		_ASSERT_VALID( pWindow ); // Must be valid.

		// Save the corresponding window.
		m_pWindow = pWindow;

		// Attach the device context returned by
		// the BeginPaint() API.
		Attach( ::BeginPaint( pWindow->GetSafeHWND(), &m_PaintStruct ));
	}

	// Destructor. Detaches the wrapped device context and
	// calls the EndPaint() API.
	virtual ~ClsPaintDC()
	{
		// First we detach the device context.
		Detach();

		// Call EndPaint().
		EndPaint( m_pWindow->GetSafeHWND(), &m_PaintStruct );
	}

	// Operators.
	operator LPPAINTSTRUCT() const
	{
		return ( LPPAINTSTRUCT )&m_PaintStruct;
	}

	operator PAINTSTRUCT&() const
	{
		return ( PAINTSTRUCT& )m_PaintStruct;
	}

protected:
	// Data.
	PAINTSTRUCT	m_PaintStruct;
	ClsWindow      *m_pWindow;
};

#endif // _PAINTDC_H_
