#ifndef _BUFFERDC_H_
#define _BUFFERDC_H_
//
// bufferdc.h
//
// (C) Copyright 2000-2004 Jan van den Baard.
//     All Rights Reserved.
//

#include "dc.h"
#include "bitmap.h"
#include "../coords/size.h"

// A class which creates a compatible device context
// and bitmap to allow off-screen rendering. In it's
// destructor the off-screen contents is rendered in
// the parent DC.
//
// If the off-screen rendering environment fails to
// create all rendering is done on the screen.
class ClsBufferDC : public ClsDC
{
	_NO_COPY( ClsBufferDC );
public:
	// Constructors. Creates a compatible DC from the
	// input DC.
	ClsBufferDC()
	{
		m_bOffsetInBuffer = FALSE;
	}

	ClsBufferDC( ClsDC& parent, ClsRect rc )
	{
		m_bOffsetInBuffer = FALSE;
		Setup( &parent, rc );
	}
	ClsBufferDC( ClsDC *parent, LPRECT pRect = NULL, BOOL bOffsetInBuffer = FALSE )
	{
		m_bOffsetInBuffer = bOffsetInBuffer;
		Setup( parent, pRect );
	}

	// Set rectangle.
	inline void SetRect( LPRECT pRect, BOOL bOffsetInBuffer = FALSE )
	{
		m_bOffsetInBuffer = bOffsetInBuffer;
		if ( pRect ) m_Rect = pRect;
		else m_Input->GetClipBox( m_Rect );
	}

	// Destructor. BltBit() the contents of the offscreen
	// dc into the destination dc.
	~ClsBufferDC()
	{
		// Was the buffer constructed?
		if ( m_CompatibleBM.IsValid())
		{
			// Blast the buffer into the input DC.
			m_Input->BitBlt( m_Rect.Left(), m_Rect.Top(), m_Rect.Width(), m_Rect.Height(), &m_CompatibleDC, m_bOffsetInBuffer ? m_Rect.Left() : 0, m_bOffsetInBuffer ? m_Rect.Top() : 0, SRCCOPY );

			// Put back the old bitmap.
			m_CompatibleDC.SelectObject( m_OldBM );

			// We are responsible for destroying the
			// device context because the ClsDC destructor
			// will not do it for us.
			m_CompatibleDC.DeleteDC();
		}

		// Detach the wrapped handle.
		Detach();
	}

protected:
	// Helper.
	void Setup( ClsDC *parent, LPRECT pRect = NULL )
	{
		// Preset data.
		m_Input	= parent;

		// Copy rectangle.
		if ( pRect ) m_Rect = pRect;
		else	     parent->GetClipBox( m_Rect );

		// Determine the bitmap size.
		int nWidth, nHeight;
		if ( m_bOffsetInBuffer )
		{
			// Make sure the offsets are included in the
			// bitmap.
			nWidth  = m_Rect.Left() + m_Rect.Width();
			nHeight = m_Rect.Top() + m_Rect.Height();
		}
		else
		{
			// Just the rectangle.
			nWidth = m_Rect.Width();
			nHeight = m_Rect.Height();
		}

		// Create a compatible DC.
		if ( m_CompatibleDC.CreateCompatibleDC( parent ))
		{
			// And a compatible bitmap.
			if ( m_CompatibleBM.CreateCompatibleBitmap( *parent, nWidth, nHeight ))
			{
				// Select the bitmap into the DC.
				m_OldBM = m_CompatibleDC.SelectObject( &m_CompatibleBM );

				// Attach the DC.
				Attach( m_CompatibleDC );

				// Copy the content of the parent DC if it is 
				// transparent. If it is not we fill the rectangle
				// with the parent it's background color.
				if ( parent->GetBkMode() == TRANSPARENT )
					BitBlt( m_Rect.Left(), m_bOffsetInBuffer ? m_Rect.Top() : 0, m_bOffsetInBuffer ? m_Rect.Width() : 0, m_Rect.Height(), parent, m_Rect.Left(), m_Rect.Top(), SRCCOPY );
				else
					FillSolidRect( m_Rect, parent->GetBkColor());
				return;
			}
			// Destroy the DC.
			m_CompatibleDC.DeleteDC();
		}
		// Use the input DC.
		Attach( *parent );
	}

	// Data.
	BOOL	      m_bIsOK;
	BOOL	      m_bOffsetInBuffer;
	ClsRect	      m_Rect;
	ClsDC	     *m_Input;
	ClsDC	      m_CompatibleDC;
	ClsBitmap     m_CompatibleBM;
	ClsGdiObject *m_OldBM;
};

#endif _BUFFERDC_H_