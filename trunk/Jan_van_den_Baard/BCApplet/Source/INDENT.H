//
//	indent.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Indention page.
//

// Indention page class.
class IndentPage : public Page
{
	_NO_COPY( IndentPage );
public:
	// Construction/destruction.
	IndentPage();
	virtual ~IndentPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Helpers.
	void SetupControls();
	
	// Data.
	EditToolbar	m_Tools;
	ClsCheckBox	m_Auto, m_Smart;
	ClsRadioButton	m_Left, m_Right;
	ColorList	m_List;
	ClsEdit		m_Edit;
	ClsEdit		m_Tab;
	ClsUpDown	m_TabUpDown;
	LPPARSER	m_pParser;
};
