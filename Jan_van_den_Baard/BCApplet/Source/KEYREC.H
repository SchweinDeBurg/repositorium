//
//	keyrec.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	keyboard recorder dialog.
//

// Key recorder edit class.
class KeyRecEdit : public ClsEdit
{
	_NO_COPY( KeyRecEdit );
public:
	// Construction/destruction.
	KeyRecEdit() : m_cQual( 0 ), m_cCode( 0 ) {;}
	virtual ~KeyRecEdit() {;}

	// Implementation.
	ClsString& KeyString();

protected:
	// Overidables.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Data.
	ClsString	m_KeyString;
	TCHAR		m_cQual, m_cCode;
};
	
// Key recorder dialog class.
class KeyRec : public ClsDialog
{
	_NO_COPY( KeyRec );
public:
	// Construction/destruction.
	KeyRec();
	virtual ~KeyRec();
	
	// Implementation.
	BOOL KeyRecord( ClsWindow& Parent, ClsString& str );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data.
	KeyRecEdit	m_Key;
	ClsString      *m_pKeyString;
};
