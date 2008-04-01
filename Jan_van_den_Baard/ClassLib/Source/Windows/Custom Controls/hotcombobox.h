#ifndef _HOTCOMBOBOX_H_
#define _HOTCOMBOBOX_H_
//
// hotcombobox.h
//
// (C) Copyright 2003 Jan van den Baard.
//     All Rights Reserved.
//

#include "../controls/combobox.h"
#include "../controls/edit.h"
#include "../../coords/rect.h"
#include "../../strings/string.h"
#include "combocompletion.h"

// A ClsComboBox derived class which supports
// hottracking in default and Office XP
// visual styles.
class ClsHotComboBox : public ClsComboBox
{
	_NO_COPY( ClsHotComboBox );
public:
	// Constructor.
	ClsHotComboBox();

	// Destructor.
	virtual ~ClsHotComboBox();

	// Implementation.
	BOOL Attach( HWND hWnd, BOOL bDestroy = FALSE );
	void SetXPStyle( BOOL bXPStyle = TRUE, BOOL bRedraw = TRUE );
	inline void DoCompletion( BOOL bCompletion = TRUE ) { m_bDoComplete = bCompletion; }

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	virtual LRESULT OnDropDown( BOOL& bNotifyParent );
	virtual LRESULT OnCloseUp( BOOL& bNotifyParent );
	virtual LRESULT OnPaint( ClsDC *pDC );
	virtual BOOL PreTranslateMessage( LPMSG pMsg );
	virtual LRESULT OnEditUpdate( BOOL& bNotifyParent );
	virtual LRESULT OnSetFocus( BOOL& bNotifyParent );
	virtual LRESULT OnKillFocus( BOOL& bNotifyParent );
	
	// Helpers.
	void SizeEditControl();
	void RenderTheFrame( ClsDC *pDC );
	void DrawDDArrow( ClsDC *pDC );
	BOOL IsTracking();
	BOOL HasFocus();

	// Data.
	BOOL		m_bTracking;	// Mouse is tracking.
	BOOL		m_bDoComplete;	// Auto-completion?
	BOOL		m_bXPStyle;	// XP stule rendering.
	ClsCompletion	m_Completion;	// Auto-completion helper.
};

#endif // _HOTCOMBOBOX_H_