//
//	insertedit.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Text insert editor dialog.
//

// TextInsert dialog class.
class TextInsert : public ClsSizeDialog
{
	_NO_COPY( TextInsert );

public:
	// Construction/destruction.
	TextInsert();
	virtual ~TextInsert();
	
	// Implementation.
	BOOL Edit( ClsWindow& Parent, LPCNODE pCNode, HICON hIcon );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );
	virtual LRESULT OnSize( UINT nSizeType, int nWidth, int nHeight );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Helpers.
	LPTSTR Str2CNode( LPCTSTR pszString );
	LPTSTR CNode2Str( LPCTSTR pszString );
	
	// Data
	ClsLayoutEngine m_Master, m_Buttons;
	ClsSize		m_MinSize;
	ClsLStatic	m_Static;
	ClsLPushButton  m_Ok, m_Cancel;
	ClsBrainchild	m_Edit;
	LPCNODE		m_pCNode;
	HICON		m_hIcon;
};
