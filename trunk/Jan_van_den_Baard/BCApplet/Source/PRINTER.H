//
//	printer.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Printer page.
//

// Printer page.
class PrinterPage : public Page
{
	_NO_COPY( PrinterPage );
public:
	// Construction/destruction.
	PrinterPage();
	virtual ~PrinterPage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );
	
protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data.
	ClsCheckBox	m_Header, m_Footer, m_Colors, m_LineNums;
	ClsEdit		m_HLeft, m_HMid, m_HRight, m_FLeft, m_FMid, m_FRight;
	ClsRadioButton	m_Screen, m_Select;
	ClsInfoControl	m_STHead, m_STFoot, m_STOpt;
	ClsButton	m_Font;
};
