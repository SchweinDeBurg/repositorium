//
//	syntax.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Syntax Coloring page.
//

// Syntax coloring page class.
class SyntaxPage : public Page
{
	_NO_COPY( SyntaxPage );
public:
	// Construction/destruction.
	SyntaxPage();
	virtual ~SyntaxPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Helpers.
	void SetupToolbar();
	void SaveColor( ClsStdioFile *pFile, LPPARSER pParser, LPCTSTR pszDesc, DWORD dwFlag, int nIndex );

	// Data.
	EditToolbar	m_Tools, m_Color;
	ColorList	m_Blocks, m_Common;
	ClsEdit		m_Escape;
	ClsCheckBox	m_Syntax;
};
