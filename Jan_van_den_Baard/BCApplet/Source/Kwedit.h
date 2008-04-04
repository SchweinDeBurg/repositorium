//
//	kwedit.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Keyword list edit dialog.
//

// Keyword edit dialog class.
class KWEdit : public ClsSizeDialog
{
	_NO_COPY( KWEdit );
public:
	// Construction/destruction.
	KWEdit();
	virtual ~KWEdit();
	
	// Implementation.
	void Edit( ClsWindow& Parent, LPKEYWORDS pKW, KeywordEdit *pKWE );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnClose() { EndDialog( FALSE ); return 0;}
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	
	// Helpers.
	BOOL SaveKeywords();
	
	// Data
	ClsLayoutEngine m_Master, m_Buttons;
	ClsSize		m_MinSize;
	ClsLStatic	m_Static;
	ClsLPushButton  m_Ok, m_Cancel;
	ClsBrainchild	m_Edit;
	TCHAR		m_szOutput[ MAX_PATH ];
	LPKEYWORDS	m_pKW;
	COLORREF	m_crBkgnd;
	HICON		m_hIcon;
};
