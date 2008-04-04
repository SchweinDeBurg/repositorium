//
//	general.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	General page.
//

// General page class.
class GeneralPage : public Page
{
	_NO_COPY( GeneralPage );
public:
	// Construction/destruction.
	GeneralPage();
	virtual ~GeneralPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data.
	ClsStatic	m_Sep, m_Label1, m_Label2;
	ClsInfoControl	m_STGeneral, m_STOptions, m_STGlobal;
	ClsEdit		m_Class, m_Ext, m_Delim, m_Hops, m_Hist, m_MarkerOfs;
	ClsComboBox	m_Caret;
	ClsCheckBox	m_Syntax, m_Marker;
	ClsUpDown	m_UDHops, m_UDHist, m_UDMarkerOfs;
	OptionTree	m_Options;
	UINT		m_nListHeight;
};
