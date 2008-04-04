//
//	files.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Files page.
//

// Number of backup modes.
#define BACKUP_MODES	4

// Files page class.
class FilesPage : public Page
{
	_NO_COPY( FilesPage );
public:
	// Construction/destruction.
	FilesPage();
	virtual ~FilesPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data.
	ClsInfoControl		m_STDef, m_STBack, m_STAs;
	ClsFileDirBrowser	m_Folder;
	ClsComboBox		m_Mode;
	ClsCheckBox		m_Backup;
	ClsRadioButton		m_BakMode[ BACKUP_MODES ];
};
