#ifndef _WAITCURSOR_H_
#define _WAITCURSOR_H_
//
// waitcursor.h
//
// (C) Copright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "../standard.h"

// A simple class which shows the wait cursor.
class ClsWaitCursor
{
	_NO_COPY( ClsWaitCursor );
public:
	// Constructor. Simply set the
	// wait cursor.
	ClsWaitCursor()
	{
		// Set the cursor remembering the
		// old cursor.
		m_hCursor = ::SetCursor( ::LoadCursor( NULL, IDC_WAIT ));
	}

	// Destructor. Restores the previous cursor.
	virtual ~ClsWaitCursor()
	{
		// Restore cursor when it was valid. If it was
		// not set the arrow cursor.
		::SetCursor( m_hCursor ? m_hCursor : ::LoadCursor( NULL, IDC_ARROW ));
	}

	// Restore wait cursor.
	inline void Restore()
	{
		// Restore the wait cursor.
		::SetCursor( ::LoadCursor( NULL, IDC_WAIT ));
	}
protected:
	// Data.
	HCURSOR m_hCursor;
};

#endif // _WAITCURSOR_H_
