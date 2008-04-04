//
//	keyboard.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Keyboard page.
//

// Keyboard page class.
class KeyboardPage : public Page
{
	_NO_COPY( KeyboardPage );
public:
	// Construction/destruction.
	KeyboardPage();
	virtual ~KeyboardPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT OnMeasureItem( UINT nID, LPMEASUREITEMSTRUCT pMis );
	virtual LRESULT OnDrawItem( UINT nID, LPDRAWITEMSTRUCT pDis );
	
	// Helpers.
	void SetupControls();
	LPHASH GetSelection();
	BOOL CreatePopupMenu();
	void AddKeyMapping( ClsString strKey );
	void CreateNode( int nType, LPVOID lpvData );

	// Data.
	EditToolbar	m_Tools, m_ComTools;
	ClsListBox	m_Keys, m_Commands;
	HIMAGELIST	m_hImages;
	ClsBitmapMenu	m_RealMenu;
	ClsBitmapMenu	m_Menu;
};
