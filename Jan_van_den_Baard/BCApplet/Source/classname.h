//
//	classname.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Text classname dialog.
//

// ClassName dialog class.
class ClassName : public ClsDialog
{
	_NO_COPY( ClassName );

public:
	// Construction/destruction.
	ClassName();
	virtual ~ClassName();
	
	// Implementation.
	BOOL Name( ClsWindow& Parent, ClsString& name );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data
	ClsEdit		m_Edit;
	ClsString      *m_pName;
};
