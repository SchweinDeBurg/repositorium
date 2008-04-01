#ifndef _FILEDIRBROWSER_H_
#define _FILEDIRBROWSER_H_
//
// filedirbrowser.h
//
// (C) Copyright 2002 Jan van den Baard.
//     All Rights Reserved.
//

#include "browser.h"
#include "../../strings/string.h"

// A ClsEdit derived class which enables the user
// to enter text and push a selection button to drop
// a "ClsFileDirTree" control for file or directory
// selection.
class ClsFileDirBrowser : public ClsBrowser
{
	_NO_COPY( ClsFileDirBrowser );
public:
	// Constructor.
	ClsFileDirBrowser();

	// Destructor.
	virtual ~ClsFileDirBrowser() {;}

	// Implementation.
	inline ClsString& BasePath()
	{ return ( ClsString& )m_sBasePath; }
	inline ClsString& LoadingText()
	{ return ( ClsString& )m_sLoadingText; }
	inline ClsString& Filters()
	{ return ( ClsString& )m_sFilters; }
	inline COLORREF& LoadingTextColor()
	{ return ( COLORREF& )m_crLoadingTextColor; }
	inline BOOL& ShowFiles()
	{ return ( BOOL& )m_bShowFiles; }

protected:
	// Overidables.
	virtual void GetButtonSize( ClsDC *pDC, ClsSize& sButtonSize );
	virtual void RenderButton( ClsDC *pDC, ClsRect rcClip );
	virtual void OnBrowserClicked();
	virtual void OnFileDropped( HDROP hDrop );

	// Data.
	ClsString	m_sBasePath;
	ClsString	m_sLoadingText;
	ClsString	m_sFilters;
	COLORREF	m_crLoadingTextColor;
	BOOL		m_bShowFiles;
};

#endif // _FILEDIRBROWSER_H_