#ifndef _COLORDIALOG_H_
#define _COLORDIALOG_H_
//
// colordialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commondialog.h"

// A wrapper for the Color common dialog.
class ClsColorDialog : public ClsCommonDialog
{
	_NO_COPY( ClsColorDialog );
public:
	// Constructor. Does nothing.
	ClsColorDialog()
	{;}

	// Destructor. Does nothing.
	virtual ~ClsColorDialog()
	{;}

	// Get the selected color.
	inline COLORREF GetColor() const
	{ return m_crColor; }

	// Get the custom color table.
	static void GetCustomColors( COLORREF *pColorTable )
	{
		_ASSERT_VALID( pColorTable ); // Must be valid.

		// Copy the custom colors.
		memcpy(( void * )pColorTable, ( void * )GetCustomColorTable(), 16 * sizeof( COLORREF ));
	}

	// Set the custom color table.
	static void SetCustomColors( COLORREF *pColorTable )
	{
		_ASSERT_VALID( pColorTable ); // Must be valid.

		// Copy the custom colors.
		memcpy(( void * )GetCustomColorTable(), ( void * )pColorTable, 16 * sizeof( COLORREF ));
	}

	// Popup the color dialog.
	BOOL DoModal( ClsWindow *parent, COLORREF crColor, DWORD dwFlags = CC_FULLOPEN, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );

	// Operator overload.
	operator COLORREF() const
	{ return m_crColor; }

protected:
	// Hook procedure. This will attach the dialog
	// to the object.
	static UINT CALLBACK HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Get a pointer to the internal custom color array. The pointer
	// returned points to an array of 16 COLORREF values which are
	// shared by all instances of this class.
	static COLORREF *GetCustomColorTable()
	{ 
		// default color table.
		static COLORREF customColors[ 16 ] =
		{
			RGB( 255, 255, 255), RGB( 239, 239, 239),
			RGB( 223, 223, 223), RGB( 207, 207, 207),
			RGB( 191, 191, 191), RGB( 175, 175, 175),
			RGB( 159, 159, 159), RGB( 143, 143, 143),
			RGB( 127, 127, 127), RGB( 111, 111, 111),
			RGB(  95,  95,  95), RGB(  79,  79,  79),
			RGB(  63,  63,  63), RGB(  47,  47,  47),
			RGB(  31,  31,  31), RGB(  15,  15,  15)
		};

		// Return the table.
		return &customColors[ 0 ]; 
	}

	// Overidable. Called just before the dialog is opened.
	virtual BOOL PreShowDialog( LPCHOOSECOLOR pChooseColor )
	{ return TRUE; }

	// Data.
	COLORREF		m_crColor;
};

#endif // _COLORDIALOG_H_