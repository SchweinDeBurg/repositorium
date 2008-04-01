#ifndef _FILEDIALOG_H_
#define _FILEDIALOG_H_
//
// filedialog.h
//
// (C) Copyright 2000 Jan van den Baard.
//     All Rights Reserved.
//

#include "commondialog.h"
#include "../../strings/string.h"

// A wrapper for the File common dialog.
class ClsFileDialog : public ClsCommonDialog
{
	_NO_COPY( ClsFileDialog );
public:
	// Constructor.
	ClsFileDialog()
	{
		// Default the data.
		m_pszFilters   = NULL;
		m_nFilterIndex = 0;
	}

	// Destructor.
	virtual ~ClsFileDialog()
	{;}

	// Interface.
	inline LPCTSTR& Filters()
	{ return ( LPCTSTR& )m_pszFilters; }
	inline int& FilterIndex()
	{ return ( int& )m_nFilterIndex; }
	inline ClsString& Caption()
	{ return ( ClsString& )m_strCaption; }
	inline ClsString& DefExt()
	{ return ( ClsString& )m_strDefExt; }
	inline ClsString& Dir()
	{ return ( ClsString& )m_strDir; }
	BOOL DoModal( ClsWindow *parent, LPCTSTR pszDir, LPCTSTR pszFile, BOOL bOpenDialog = TRUE, DWORD dwFlags = 0, int x = CW_USEDEFAULT, int y = CW_USEDEFAULT );
	int GetName( int nPrev, ClsString& str ) const;

protected:
	// Hook procedure. This will attach the dialog
	// to the object.
	static UINT CALLBACK HookProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );

	// Routes the CDN_xxx messages to their virtual handlers.
	virtual LRESULT OnNotify( LPNMHDR pNMHDR );

	// Overidable. Called just before the dialog is opened.
	virtual BOOL PreShowDialog( LPOPENFILENAME pOpenFileName )
	{ return TRUE; }

	// Overidables.
	virtual LRESULT OnFileOK( LPOFNOTIFY pOFNotify )
	{ return 0; }
	virtual LRESULT OnFolderChange( LPOFNOTIFY pOFNotify )
	{ return 0; }
	virtual LRESULT OnHelp( LPOFNOTIFY pOFNotify )
	{ return 0; }
	virtual LRESULT OnInitDone( LPOFNOTIFY pOFNotify )
	{ return 0; }
	virtual LRESULT OnSelChange( LPOFNOTIFY pOFNotify )
	{ return 0; }
	virtual LRESULT OnShareViolation( LPOFNOTIFY pOFNotify )
	{ return OFN_SHAREWARN; }
	virtual LRESULT OnTypeChange( LPOFNOTIFY pOFNotify )
	{ return 0; }

	// Data.
	LPCTSTR		m_pszFilters;
	int		m_nFilterIndex;
	ClsString	m_strCaption;
	ClsString	m_strDefExt;
	ClsString	m_strDir;
	ClsString	m_strFile;
};

#endif // _FILEDIALOG_H_
