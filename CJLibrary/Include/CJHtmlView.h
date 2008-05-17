// CJHtmlView.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Iuri Apollonio's article 'Doc / View - 
// Using HtmlView apart from Tecno Preview' http://www.codeguru.com/doc_view/htmlview.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 11/01/99 8:53p $
 * $Revision: 9 $
 * $Archive: /CodeJock/Include/CJHtmlView.h $
 *
 * $History: CJHtmlView.h $
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 11/01/99   Time: 8:53p
 * Updated in $/CodeJock/Include
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 1:29a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:17p
 * Updated in $/CodeJockey/Include
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:29p
 * Created in $/CodeJockey/Include
 * Copied from MFC v6 and techno preview for v5. Added to extend
 * functionality to Visual C++ 5.0 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJHTMLVIEW_H__
#define __CJHTMLVIEW_H__

#include <docobj.h>
#include <shlobj.h>
#include <exdisp.h>
#include <afxconv.h>

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// The CCJHtmlView class provides the functionality of the WebBrowser control 
// within the context of MFC's document/view architecture. The WebBrowser control 
// is a window in which the user can browse sites on the World Wide Web, as well 
// as folders in the local file system and on a network. The WebBrowser control 
// supports hyperlinking, Uniform Resource Locator (URL) navigation, and maintains 
// a history list.
class _CJX_EXT_CLASS CCJHtmlView : public CFormView
{
	DECLARE_DYNCREATE(CCJHtmlView)
	DECLARE_EVENTSINK_MAP()

protected: // create from serialization only

	CCJHtmlView();

	virtual ~CCJHtmlView();

// Attributes
public:

	CString GetType() const;
	long GetLeft() const;
	void SetLeft(long nNewValue);
	long GetTop() const;
	void SetTop(long nNewValue);
	long GetHeight() const;
	void SetHeight(long nNewValue);
	void SetVisible(BOOL bNewValue);
	BOOL GetVisible() const;
	CString GetLocationName() const;
	READYSTATE GetReadyState() const;
	BOOL GetOffline() const;
	void SetOffline(BOOL bNewValue);
	BOOL GetSilent() const;
	void SetSilent(BOOL bNewValue);
	BOOL GetTopLevelContainer() const;
	CString GetLocationURL() const;
	BOOL GetBusy() const;
	LPDISPATCH GetApplication() const;
	LPDISPATCH GetParentBrowser() const;
	LPDISPATCH GetContainer() const;
	LPDISPATCH GetHtmlDocument() const;
	CString GetFullName() const;
	int GetToolBar() const;
	void SetToolBar(int nNewValue);
	BOOL GetMenuBar() const;
	void SetMenuBar(BOOL bNewValue);
	BOOL GetFullScreen() const;
	void SetFullScreen(BOOL bNewValue);
	OLECMDF QueryStatusWB(OLECMDID cmdID) const;
	BOOL GetRegisterAsBrowser() const;
	void SetRegisterAsBrowser(BOOL bNewValue);
	BOOL GetRegisterAsDropTarget() const;
	void SetRegisterAsDropTarget(BOOL bNewValue);
	BOOL GetTheaterMode() const;
	void SetTheaterMode(BOOL bNewValue);
	BOOL GetAddressBar() const;
	void SetAddressBar(BOOL bNewValue);
	BOOL GetStatusBar() const;
	void SetStatusBar(BOOL bNewValue);

// Operations
public:

	void GoBack();
	void GoForward();
	void GoHome();
	void GoSearch();
	void Navigate(LPCTSTR URL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL,
		LPCTSTR lpszHeaders = NULL, LPVOID lpvPostData = NULL,
		DWORD dwPostDataLen = 0);
	void Navigate2(LPITEMIDLIST pIDL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags = 0,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeaders = NULL,
		LPVOID lpvPostData = NULL, DWORD dwPostDataLen = 0);
	void Navigate2(LPCTSTR lpszURL, DWORD dwFlags,
		CByteArray& baPostedData,
		LPCTSTR lpszTargetFrameName = NULL, LPCTSTR lpszHeader = NULL);
	void Refresh();
	void Refresh2(int nLevel);
	void Stop();
	void PutProperty(LPCTSTR lpszProperty, const VARIANT& vtValue);
	void PutProperty(LPCTSTR lpszPropertyName, double dValue);
	void PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue);
	void PutProperty(LPCTSTR lpszPropertyName, long lValue);
	void PutProperty(LPCTSTR lpszPropertyName, short nValue);
	BOOL GetProperty(LPCTSTR lpszProperty, CString& strValue);
	COleVariant GetProperty(LPCTSTR lpszProperty);
	void ExecWB(OLECMDID cmdID, OLECMDEXECOPT cmdexecopt, VARIANT* pvaIn,
		VARIANT* pvaOut);
	BOOL LoadFromResource(LPCTSTR lpszResource);
	BOOL LoadFromResource(UINT nRes);

// Overrides
public:

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJHtmlView)
	public:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	virtual void OnNavigateComplete2(LPCTSTR strURL);
	virtual void OnBeforeNavigate2(LPCTSTR lpszURL, DWORD nFlags, LPCTSTR lpszTargetFrameName, CByteArray& baPostedData, LPCTSTR lpszHeaders, BOOL* pbCancel);
	virtual void OnStatusTextChange(LPCTSTR lpszText);
	virtual void OnProgressChange(long nProgress, long nProgressMax);
	virtual void OnCommandStateChange(long nCommand, BOOL bEnable);
	virtual void OnDownloadBegin();
	virtual void OnDownloadComplete();
	virtual void OnTitleChange(LPCTSTR lpszText);
	virtual void OnPropertyChange(LPCTSTR lpszProperty);
	virtual void OnNewWindow2(LPDISPATCH* ppDisp, BOOL* Cancel);
	virtual void OnDocumentComplete(LPCTSTR lpszURL);
	virtual void OnQuit();
	virtual void OnVisible(BOOL bVisible);
	virtual void OnToolBar(BOOL bToolBar);
	virtual void OnMenuBar(BOOL bMenuBar);
	virtual void OnStatusBar(BOOL bStatusBar);
	virtual void OnFullScreen(BOOL bFullScreen);
	virtual void OnTheaterMode(BOOL bTheaterMode);
	protected:
	virtual void NavigateComplete2(LPDISPATCH pDisp, VARIANT* URL);
	virtual void BeforeNavigate2(LPDISPATCH pDisp, VARIANT* URL, VARIANT* Flags, VARIANT* TargetFrameName, VARIANT* PostData, VARIANT* Headers,   BOOL* Cancel);
	virtual void DocumentComplete(LPDISPATCH pDisp, VARIANT* URL);
	//}}AFX_VIRTUAL
	
// Implementation
public:
	
	CWnd m_wndBrowser;

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	IWebBrowser2* m_pBrowserApp;

// Generated message map functions
protected:
	//{{AFX_MSG(CCJHtmlView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnDestroy();
	afx_msg void OnFilePrint();
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// Inline functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE void CCJHtmlView::SetRegisterAsBrowser(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_RegisterAsBrowser((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetRegisterAsDropTarget(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_RegisterAsDropTarget((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetTheaterMode(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_TheaterMode((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetVisible(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Visible((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetMenuBar(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_MenuBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetToolBar(int nNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_ToolBar(nNewValue); }

_CJXLIB_INLINE void CCJHtmlView::SetOffline(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Offline((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetSilent(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Silent((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::GoBack()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoBack(); }

_CJXLIB_INLINE void CCJHtmlView::GoForward()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoForward(); }

_CJXLIB_INLINE void CCJHtmlView::GoHome()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoHome(); }

_CJXLIB_INLINE void CCJHtmlView::GoSearch()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->GoSearch(); }

_CJXLIB_INLINE void CCJHtmlView::Refresh()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Refresh(); }

_CJXLIB_INLINE void CCJHtmlView::Refresh2(int nLevel)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Refresh2(COleVariant((long) nLevel, VT_I4)); }

_CJXLIB_INLINE void CCJHtmlView::Stop()
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->Stop(); }

_CJXLIB_INLINE void CCJHtmlView::SetFullScreen(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_FullScreen((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetAddressBar(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_AddressBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

_CJXLIB_INLINE void CCJHtmlView::SetHeight(long nNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Height(nNewValue); }

_CJXLIB_INLINE void CCJHtmlView::PutProperty(LPCTSTR lpszPropertyName, long lValue)
	{ ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(lValue, VT_UI4)); }

_CJXLIB_INLINE void CCJHtmlView::PutProperty(LPCTSTR lpszPropertyName, short nValue)
	{ ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(nValue, VT_UI2)); }

_CJXLIB_INLINE void CCJHtmlView::PutProperty(LPCTSTR lpszPropertyName, LPCTSTR lpszValue)
	{ ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(lpszValue, VT_BSTR)); }

_CJXLIB_INLINE void CCJHtmlView::PutProperty(LPCTSTR lpszPropertyName, double dValue)
	{ ASSERT(m_pBrowserApp != NULL); ASSERT(m_pBrowserApp != NULL); PutProperty(lpszPropertyName, COleVariant(dValue)); }

_CJXLIB_INLINE void CCJHtmlView::SetTop(long nNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Top(nNewValue); }

_CJXLIB_INLINE void CCJHtmlView::SetLeft(long nNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_Left(nNewValue); }

_CJXLIB_INLINE void CCJHtmlView::SetStatusBar(BOOL bNewValue)
	{ ASSERT(m_pBrowserApp != NULL); m_pBrowserApp->put_StatusBar((short) (bNewValue ? AFX_OLE_TRUE : AFX_OLE_FALSE)); }

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJHTMLVIEW_H__

