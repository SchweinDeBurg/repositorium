//
//	page.h
//
//	(C) Copyright 2000-2002 by Jan van den Baard.
//	    All Rights Reserved.
//

// Page base.
class Page : public ClsDialog
{
	_NO_COPY( Page );
public:
	// Construction/destruction.
	Page();
	virtual ~Page();

	// Implementation.
	inline void SetIcon( UINT nIcon ) { m_nIcon = nIcon; }
	inline UINT GetIcon() { return m_nIcon; }
	void HandleEditControl( UINT nNotifyCode, UINT nCtrlID, LPTSTR *pStrPtr, BOOL bChange = TRUE );
	
	// Overidables.
	virtual void RefreshData( LPPARSER pParser ) {;}
	virtual BOOL SavePage( ClsStdioFile *pFile, LPPARSER pParser ) { return TRUE; }

protected:
	// Window procedure.
	virtual LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam )
	{
		if ( uMsg == WM_NCACTIVATE )
		{
			// Pass it on to the parent.
			ClsWindow *pParent = GetParent();
			if ( pParent ) 
				return pParent->SendMessage( uMsg, wParam, lParam );
		}
		// To the baseclass.
		return ClsDialog::WindowProc( uMsg, wParam, lParam );
	}

	// Helpers.
	LPTSTR CopyEditContents( UINT nCtrlID );
	
	// Data.
	UINT		m_nIcon;
	LPPARSER	m_pParser;
};
