//
//	about.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//
//	About page.
//

// About page.
class AboutPage : public Page
{
	_NO_COPY( AboutPage );
public:
	// Construction/destruction.
	AboutPage();
	virtual ~AboutPage();

protected:
	// Overidables.
	virtual LRESULT OnInitDialog( LPPROPSHEETPAGE pPsp );
	virtual LRESULT OnCommand( UINT nNotifyCode, UINT nCtrlID, HWND hWndCtrl );

	// Data.
	ClsInfoControl	m_Copyright, m_Disclaimer;
	ClsHyperLink	m_Http, m_Email;
};
