//
//	hardcoded.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Hardcoded command selection dialog.
//

// Hardcoded dialog class.
class Hardcoded : public ClsDialog
{
	_NO_COPY( Hardcoded );

public:
	// Construction/destruction.
	Hardcoded();
	virtual ~Hardcoded();
	
	// Implementation.
	BOOL Select( ClsWindow& Parent, LPCNODE pCNode );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data
	ClsListBox	m_List;
	LPCNODE		m_pCNode;
};
