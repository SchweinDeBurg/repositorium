// CJHyperLink.h : header file
//
//
// CJHyperLink static control. Will open the default browser with the given URL
// when the user clicks on the link.
//
// Copyright Chris Maunder, 1997
// Feel free to use and distribute. May not be sold for profit. 
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/26/99 10:42p $
 * $Revision: 6 $
 * $Archive: /CodeJock/Include/CJHyperLink.h $
 *
 * $History: CJHyperLink.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:42p
 * Updated in $/CodeJock/Include
 * Made class methods virtual for inheritance purposes.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 12:26p
 * Updated in $/CodeJock/Include
 * Added source control history to file header.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJHYPERLINK_H__
#define __CJHYPERLINK_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

// A CStatic derived classc will open the default browser with the 
// given URL when the user clicks on the link. 
class _CJX_EXT_CLASS CCJHyperLink : public CStatic
{
// Construction/destruction
public:
    CCJHyperLink();
    virtual ~CCJHyperLink();

// Attributes
public:

// Operations
public:

    virtual void SetURL(CString strURL);
    virtual CString GetURL() const;

    virtual void SetColors(COLORREF crLinkColor, COLORREF crVisitedColor, 
                    COLORREF crHoverColor = -1);
    virtual COLORREF GetLinkColor() const;
    virtual COLORREF GetVisitedColor() const;
    virtual COLORREF GetHoverColor() const;

    virtual void SetVisited(BOOL bVisited = TRUE);
    virtual BOOL GetVisited() const;

    virtual void SetLinkCursor(HCURSOR hCursor);
    virtual HCURSOR GetLinkCursor() const;

    virtual void SetUnderline(BOOL bUnderline = TRUE);
    virtual BOOL GetUnderline() const;

    virtual void SetAutoSize(BOOL bAutoSize = TRUE);
    virtual BOOL GetAutoSize() const;

    virtual HINSTANCE GotoURL(LPCTSTR url, int showcmd);

// Overrides
    // ClassWizard generated virtual function overrides
    //{{AFX_VIRTUAL(CCJHyperLink)
    public:
    virtual BOOL PreTranslateMessage(MSG* pMsg);
    protected:
    virtual void PreSubclassWindow();
    //}}AFX_VIRTUAL

// Implementation
protected:
    virtual void ReportError(int nError);
    virtual LONG GetRegKey(HKEY key, LPCTSTR subkey, LPTSTR retdata);
    virtual void PositionWindow();
    virtual void SetDefaultCursor();

// Protected attributes
protected:
    COLORREF m_crLinkColor, m_crVisitedColor;     // Hyperlink Colors
    COLORREF m_crHoverColor;                       // Hover Color
    BOOL     m_bOverControl;                        // cursor over control?
    BOOL     m_bVisited;                            // Has it been visited?
    BOOL     m_bUnderline;                          // underline hyperlink?
    BOOL     m_bAdjustToFit;                        // Adjust window size to fit text?
    CString  m_strURL;                              // hyperlink URL
    CFont    m_Font;                                // Underline font if necessary
    HCURSOR  m_hLinkCursor;                         // Cursor for hyperlink
    CToolTipCtrl m_ToolTip;                         // The tooltip

    // Generated message map functions
protected:
    //{{AFX_MSG(CCJHyperLink)
    afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
    afx_msg BOOL OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message);
    afx_msg void OnMouseMove(UINT nFlags, CPoint point);
    //}}AFX_MSG
    afx_msg void OnClicked();
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // __CJHYPERLINK_H__
