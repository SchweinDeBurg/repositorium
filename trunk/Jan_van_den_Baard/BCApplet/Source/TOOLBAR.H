//
//	toolbar.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	New, delete, up and down toolbar.
//

// New/Delete/Up/Down toolbar class.
class EditToolbar : public ClsToolbar
{
	_NO_COPY( EditToolbar );
public:
	// Construction/destruction.
	EditToolbar();
	virtual ~EditToolbar();

	// Toolbar button IDs as send to the parent window.
	enum {
		ETID_NEW = 0xBEEF,
		ETID_DELETE,
		ETID_UP,
		ETID_DOWN,
		ETID_EDIT,
		ETID_INSERT,
		ETID_COLOR,
		ETID_BGCOLOR
	};

	// Implementation.
	void SetupToolbar( BOOL bNewDel, BOOL bUpDown, BOOL bKey, BOOL bColor, BOOL bBgColor = FALSE );
	inline void EnableButton( UINT nID, BOOL bEnable = TRUE ) { SendMessage( TB_ENABLEBUTTON, nID, MAKELONG( bEnable,  0 )); }
	LPCTSTR GetTip( UINT nID );

protected:
	// Overidables.
	virtual LRESULT OnGetInfoTip( LPNMTBGETINFOTIP pTip, BOOL& bNotifyParent );

	// Data.
	TCHAR		m_chTip[ 256 ];
};
