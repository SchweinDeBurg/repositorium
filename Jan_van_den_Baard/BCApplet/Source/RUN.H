//
//	run.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Run command dialog.
//

// Run command dialog class.
class Run : public ClsDialog
{
	_NO_COPY( Run );

public:
	// Construction/destruction.
	Run();
	virtual ~Run();
	
	// Implementation.
	BOOL Edit( ClsWindow& Parent, LPCNODE pCNode );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Helpers.
	void SplitRunCommand();
	
	// Data
	ClsFileDirBrowser	m_Run;
	ClsEdit			m_Args;
	LPCNODE			m_pCNode;
};
