#ifndef _HOTEDIT_H_
#define _HOTEDIT_H_
//
// hotedit.h
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../controls/edit.h"
#include "../../coords/rect.h"
#include "../../strings/string.h"

// A ClsEdit derived class which supports
// hottracking in default and Office XP
// visual styles.
class ClsHotEdit : public ClsEdit
{
	_NO_COPY( ClsHotEdit );
public:
	// Constructor.
	ClsHotEdit();

	// Destructor.
	virtual ~ClsHotEdit();

	// Implementation.
	BOOL Attach( HWND hWnd, BOOL bDestroy = FALSE );
	void SetXPStyle( BOOL bXPStyle = TRUE, BOOL bRedraw = TRUE );

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Helpers.
	void RenderTheFrame( ClsDC *pDC );

	// Data.
	BOOL	m_bXPStyle;
	BOOL	m_bTracking;
};

#endif // _HOTEDIT_H_