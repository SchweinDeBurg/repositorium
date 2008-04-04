//
//	main.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Main dialog.
//

// Main dialog class.
class SettingsDialog : public ClsDialog
{
	_NO_COPY( SettingsDialog );
public:
	// Construction/destruction.
	SettingsDialog();
	virtual ~SettingsDialog();

	// Implementation.
	void Changed( LPPARSER pParser );
	inline SelectionTree *GetTree() { return &m_Tree; }
	LPTSTR StrToPool( ClsString& str );

protected:
	// Helpers.
	BOOL SetChanges();
	BOOL GetSettingsLocation();
	BOOL LoadSettings();
	BOOL AddLanguage( LPCTSTR pszName, BOOL bFirst = FALSE );
	BOOL ReallyExit();

	// Overidables.
	virtual LRESULT OnInitDialog( PROPSHEETPAGE *pPropSheetPage );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnNotify( LPNMHDR pHdr );
	virtual BOOL OnCancel() { return ReallyExit(); }

	// Data.
	ClsString		m_strSettings;			// Settings location for current user.
	ClsInfoBar		m_Bar;				// Page title bar.
	ClsFont			m_BarFont;			// Page title bar font.
	SelectionTree		m_Tree;				// Treeview.
	EditToolbar		m_Tools;			// Toolbar.
	HTREEITEM		m_hLast;			// Last page item.
	HTREEITEM		m_hAbout;			// About page item.
	DWORD			m_dwAddCounter;			// Counter for added languages.

	// Pages.
	AboutPage		m_About;
	PrinterPage		m_Printer;
	GeneralPage		m_General;
	DateTimePage		m_DateTime;
	FilesPage		m_Files;
	SyntaxPage		m_Syntax;
	IndentPage		m_Indent;
	KeyboardPage		m_Keyboard;
};
