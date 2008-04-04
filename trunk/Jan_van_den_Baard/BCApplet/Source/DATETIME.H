//
//	datetime.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	Date & Time page.
//

// Timer event.
#define IDT_UPDATE	100

// DateTime page class.
class DateTimePage : public Page
{
	_NO_COPY( DateTimePage );
public:
	// Construction/destruction.
	DateTimePage();
	virtual ~DateTimePage();

	// Overidables.
	virtual void RefreshData( LPPARSER pParser );
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser );

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Data.
	ClsInfoControl	m_STFormat, m_STSample;
	ClsEdit		m_DFormat, m_TFormat;
	ClsStatic	m_DSample, m_TSample;
};
