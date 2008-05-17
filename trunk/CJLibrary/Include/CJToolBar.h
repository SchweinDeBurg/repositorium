// CJToolBar.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 7/06/02 22:21 $
 * $Revision: 2 $
 * $Archive: /Projects/AddIns/ResOrg/CJLibrary/Include/CJToolBar.h $
 *
 * $History: CJToolBar.h $
 * 
 * *****************  Version 2  *****************
 * User: Andy         Date: 7/06/02    Time: 22:21
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Eliminate warnings when compiled with VC.NET
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 7/31/99    Time: 4:27p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 10:00p
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:20p
 * Updated in $/CodeJockey/Include
 * Re-wrote toolbar class for vc5 compatibility. Added customization based 
 * on the "Customizable toolbar" article by Doug Keith - 
 * http://www.codeguru.com/toolbar/customizable_tb.shtml.
 * 
 * Cleaned up inline functions, and import/export macro so that class will
 * be imported when linked to, and exported at compile time.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 4/03/99    Time: 11:44p
 * Updated in $/CodeJockey/Include
 * Added comments and cleaned up code.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 3/13/99    Time: 11:43p
 * Updated in $/CodeJockey/Include
 * Changed class to handle both Office and DevStudio style to toolbars. To
 * use Office toolbars, use the CreateEx() method, otherwise your toolbars
 * will look like DevStudio.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 12/14/98   Time: 11:42p
 * Updated in $/CodeJockey/Include
 * Changed class to derive from Paul DiLascia's CFlatToolBar, this class
 * adds gripper and control insertion.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 11/02/98   Time: 11:41p
 * Updated in $/CodeJockey/Include
 * Fixed bug with DrawNoGripper() method - (Christian Skovdal Andersen).
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 10/17/98   Time: 11:41p
 * Created in $/CodeJockey/Include
 * Initial re-write and release.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJTOOLBAR_H__
#define __CJTOOLBAR_H__

#include "CJToolBarBase.h"
#include "CJToolBarCtrl.h"

/////////////////////////////////////////////////////////////////////////////

// for determining version of COMCTL32.DLL
#define VERSION_WIN4    MAKELONG(0, 4)
#define VERSION_IE3     MAKELONG(70, 4)
#define VERSION_IE4     MAKELONG(71, 4)
#define VERSION_IE401   MAKELONG(72, 4)

extern int _ComCtlVersion;
DWORD   AFXAPI _GetComCtlVersion();
HBITMAP AFXAPI _LoadSysColorBitmap(HINSTANCE hInst, HRSRC hRsrc, BOOL bMono = FALSE);

// One of these for each drop-down button
struct DROPDOWNBUTTON {
	DROPDOWNBUTTON* next;
	UINT			idButton;		// command ID of button
	UINT			idMenu;			// popup menu to display
} ;

struct TOOLBARINFO {
	TBBUTTON		 tbButton;		// information regarding the button
	LPCTSTR			 btnText;		// text for the button
};

class CCJToolBarCtrl;


// CCJToolBar is a CCJToolBarBase derived class which Allows the 
// creation of toolbars simular to VisualStudio, Microsoft Office or Internet Explorer
class _CJX_EXT_CLASS CCJToolBar : public CCJToolBarBase
{
	DECLARE_DYNAMIC(CCJToolBar)
		
public:

	// Default constructor
	//
	CCJToolBar();

	// Virtual destructor
	//
	virtual ~CCJToolBar();

protected:

	TOOLBARINFO*		m_pBarInfo;				// points to a TOOLBARINFO array used for toolbar customization.
	CObList*			m_pControls;			// CObject pointer list which contains controls added to the toolbar
	bool				m_bShowDropArrow;		// true to display a drop arrow when docked vertical
	HRSRC				m_hRsrcImageWell;		// handle to loaded resource for image well
	HINSTANCE			m_hInstImageWell;		// instance handle to load image well from
	HBITMAP				m_hbmImageWell;			// contains color mapped button images
	BOOL				m_bDelayedButtonLayout; // used to manage when button layout should be done
	CRect				m_rcOldPos;				// used when toolbar is moved
	int					m_nButtons;				// used for customization
	HKEY				m_hKeyRoot;				// used for customization, holds registry key.
	CString				m_strSubKey;			// used for customization, holds registry subkey name.
	CString				m_strValueName;			// used for customization, holds registry value.
	static int			m_nBarNumber;			// used for customization, holds bar number for restoration.
	CSize				m_sizeImage;			// current image size
	CSize				m_sizeButton;			// current button size
	DROPDOWNBUTTON*		m_pDropButtons;			// list of dropdown button/menu pairs
	CMapStringToPtr*	m_pStringMap;			// used as CMapStringToUInt

public:

	virtual BOOL Create(CWnd* pParentWnd,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_TOOLBAR);

	virtual BOOL CreateEx(CWnd* pParentWnd, DWORD dwCtrlStyle = TBSTYLE_FLAT,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_ALIGN_TOP,
		CRect rcBorders = CRect(0, 0, 0, 0),
		UINT nID = AFX_IDW_TOOLBAR);
	
	// this member function is called to insert a control into the toolbar
	// and returns a pointer to the newly inserted control
	//
	CWnd* InsertControl(
		// CRuntimeClass of the control to be inserted.
		CRuntimeClass* pClass,
		// window title ( if any ) of the control
		LPCTSTR lpszWindowName,
		// size of the control to be inserted
		CRect& rect,
		// resource id of the button where the control is to be placed
		UINT nID,
		// style flags for the control
		DWORD dwStyle );

	CWnd* InsertControl(
		// points to already created control to be inserted.
		CWnd* pCtrl,
		// size of the control to be inserted
		CRect& rect,
		// resource id of the button where the control is to be placed
		UINT nID);
	
	// call to add drop-down buttons
	BOOL AddDropDownButton(
		// resource id for toolbar button
		UINT nIDButton,
		// resource id of dropdown menu to be associated with button.
		UINT nIDMenu,
		// if the button is to have a drop arrow
		BOOL bArrow=TRUE);
	
	// Find buttons structure for given ID
	DROPDOWNBUTTON* FindDropDownButton(
		// id of button to find.
		UINT nID);
	
	// silly function to fake out compiler with const-ness
	LRESULT SendMessageC(UINT m, WPARAM wp=0, LPARAM lp=0) const;

	void SetSizes(SIZE sizeButton, SIZE sizeImage);
    
	// button size should be bigger than image
	void SetHeight(int cyHeight);
    
	// call after SetSizes, height overrides bitmap size
	BOOL LoadToolBar(LPCTSTR lpszResourceName, TOOLBARINFO* pBarInfo=NULL);
	BOOL LoadToolBar(UINT nIDResource, TOOLBARINFO* pBarInfo=NULL);

	BOOL LoadBitmap(LPCTSTR lpszResourceName);
	BOOL LoadBitmap(UINT nIDResource);
	BOOL SetBitmap(HBITMAP hbmImageWell);

	// lpIDArray can be NULL to allocate empty buttons
	BOOL SetButtons(const UINT* lpIDArray, int nIDCount);
	
	// standard control bar things
	int CommandToIndex(UINT nIDFind) const;
	UINT GetItemID(int nIndex) const;
	UINT GetButtonStyle(int nIndex) const;
	void SetButtonStyle(int nIndex, UINT nStyle);
	
	void SaveState();
	void RestoreState();
	
	// for changing button info
	void GetButtonInfo(int nIndex, UINT& nID, UINT& nStyle, int& iImage) const;
	void SetButtonInfo(int nIndex, UINT nID, UINT nStyle, int iImage);
	BOOL SetButtonText(int nIndex, LPCTSTR lpszText);
	CString GetButtonText(int nIndex) const;
	void GetButtonText(int nIndex, CString& rString) const;
	
	// for direct access to the underlying common control
	CCJToolBarCtrl& GetToolBarCtrl() const;
	
	void SetOwner(CWnd* pOwnerWnd);
	BOOL AddReplaceBitmap(HBITMAP hbmImageWell);

	void GetButton(int nIndex, TBBUTTON* pButton) const;
	void SetButton(int nIndex, TBBUTTON* pButton);

	// Wrappers that are not in MFC but should be;
	// I copied these from CToolBarCtrl
	BOOL EnableButton(int nID, BOOL bEnable);
	BOOL CheckButton(int nID, BOOL bCheck);
	BOOL PressButton(int nID, BOOL bPress);
	BOOL HideButton(int nID, BOOL bHide);
	BOOL Indeterminate(int nID, BOOL bIndeterminate);
	BOOL IsButtonEnabled(int nID) const;
	BOOL IsButtonChecked(int nID) const;
	BOOL IsButtonPressed(int nID) const;
	BOOL IsButtonHidden(int nID) const;
	BOOL IsButtonIndeterminate(int nID) const;
	BOOL SetState(int nID, UINT nState);
	int GetState(int nID) const;
	BOOL AddButtons(int nNumButtons, LPTBBUTTON lpButtons);
	BOOL InsertButton(int nIndex, LPTBBUTTON lpButton);
	BOOL DeleteButton(int nIndex);
	int GetButtonCount() const;
	void Customize();
	int AddStrings(LPCTSTR lpszStrings);
	void SetButtonStructSize(int nSize);
	BOOL SetButtonSize(CSize sz);
	BOOL SetBitmapSize(CSize sz);
	void AutoSize();
	CToolTipCtrl* GetToolTips() const;
	void SetToolTips(CToolTipCtrl* pTip);
	void SetRows(int nRows, BOOL bLarger, LPRECT lpRect);
	int GetRows() const;
	BOOL SetCmdID(int nIndex, UINT nID);
	UINT GetBitmapFlags() const;
	
	// Wrappers for some of the newer messages--not complete
	BOOL SetIndent(int indent);
	HIMAGELIST GetImageList() const;
	HIMAGELIST SetImageList(HIMAGELIST hImgList);
	int GetBitmap(UINT nIdButton) const;
	DWORD SetExtendedStyle(DWORD dwStyle);
	BOOL GetRect(UINT nIdButton, RECT& rc) const;
	DWORD GetToolbarStyle() const;
	void SetToolbarStyle(DWORD dwStyle);
	int HitTest(CPoint p) const;
	int  GetHotItem() const;
	void SetHotItem(int iHot);
	BOOL MapAccelerator(TCHAR ch, UINT& nID) const;
	CSize GetPadding() const;
	CSize SetPadding(CSize sz);
	
	// Implementation
public:

#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif
	
protected:
	
	// implementation helpers
	CSize CalcLayout(DWORD nMode, int nLength = -1);
	CSize CalcSize(TBBUTTON* pData, int nCount);
	int WrapToolBar(TBBUTTON* pData, int nCount, int nWidth);
	void SizeToolBar(TBBUTTON* pData, int nCount, int nLength, BOOL bVert = FALSE);
	void Layout(); // called for for delayed button layout
	
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CCJToolBar)
	public:
	virtual CSize CalcFixedLayout(BOOL bStretch, BOOL bHorz);
	virtual CSize CalcDynamicLayout(int nLength, DWORD nMode);
	virtual void OnUpdateCmdUI(CFrameWnd* pTarget, BOOL bDisableIfNoHndler);
	virtual void OnBarStyleChange(DWORD dwOldStyle, DWORD dwNewStyle);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual int OnToolHitTest(CPoint point, TOOLINFO* pTI) const;
	virtual void GetItemRect(int nIndex, LPRECT lpRect) const;
	//}}AFX_VIRTUAL

	// helpers
	virtual void InvalidateOldPos(const CRect& rcInvalid);
	virtual CSize GetButtonSize(TBBUTTON* pData, int iButton);

	// override to do your own weird drop-down buttons
	virtual void OnDropDownButton(const NMTOOLBAR& nmtb, UINT nID, CRect rc);

// Generated message map functions
protected:

	LRESULT OnSetSizeHelper(CSize& size, LPARAM lParam);

	//{{AFX_MSG(CCJToolBar)
	afx_msg UINT OnNcHitTest(CPoint);
	afx_msg void OnNcPaint();
	afx_msg void OnPaint();
	afx_msg void OnNcCalcSize(BOOL, NCCALCSIZE_PARAMS*);
	afx_msg void OnWindowPosChanging(LPWINDOWPOS);
	afx_msg void OnWindowPosChanged(WINDOWPOS* lpwndpos);
	afx_msg void OnSysColorChange();
	afx_msg LRESULT OnSetButtonSize(WPARAM, LPARAM);
	afx_msg LRESULT OnSetBitmapSize(WPARAM, LPARAM);
	afx_msg BOOL OnNcCreate(LPCREATESTRUCT);
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	afx_msg void OnDestroy();
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	//}}AFX_MSG
	
	afx_msg void OnToolBarBtnDropDown(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg void OnToolBarQueryDelete(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarQueryInsert(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarChange(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarBeginDrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarEndDrag(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarBeginAdjust(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarCustomHelp(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarEndAdjust(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarGetButtonInfo(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnToolBarReset(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg LRESULT OnPreserveZeroBorderHelper(WPARAM, LPARAM);
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////
// inline functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE CCJToolBarCtrl& CCJToolBar::GetToolBarCtrl() const
	{ ASSERT(::IsWindow(m_hWnd)); return *(CCJToolBarCtrl*)this; }

_CJXLIB_INLINE BOOL CCJToolBar::LoadBitmap(UINT nIDResource)
	{ return LoadBitmap(MAKEINTRESOURCE(nIDResource)); }

_CJXLIB_INLINE BOOL CCJToolBar::LoadToolBar(UINT nIDResource, TOOLBARINFO* pBarInfo)
	{ return LoadToolBar(MAKEINTRESOURCE(nIDResource), pBarInfo); }

_CJXLIB_INLINE LRESULT CCJToolBar::SendMessageC(UINT m, WPARAM wp, LPARAM lp) const
	{ ASSERT(::IsWindow(m_hWnd)); return const_cast<CCJToolBar*>(this)->SendMessage(m, wp, lp); }

_CJXLIB_INLINE BOOL CCJToolBar::EnableButton(int nID, BOOL bEnable)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_ENABLEBUTTON, nID, MAKELPARAM(bEnable, 0)); }

_CJXLIB_INLINE BOOL CCJToolBar::CheckButton(int nID, BOOL bCheck)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_CHECKBUTTON, nID, MAKELPARAM(bCheck, 0)); }

_CJXLIB_INLINE BOOL CCJToolBar::PressButton(int nID, BOOL bPress)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_PRESSBUTTON, nID, MAKELPARAM(bPress, 0)); }

_CJXLIB_INLINE BOOL CCJToolBar::HideButton(int nID, BOOL bHide)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_HIDEBUTTON, nID, MAKELPARAM(bHide, 0)); }

_CJXLIB_INLINE BOOL CCJToolBar::Indeterminate(int nID, BOOL bIndeterminate)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_INDETERMINATE, nID, MAKELPARAM(bIndeterminate, 0)); }

_CJXLIB_INLINE BOOL CCJToolBar::IsButtonEnabled(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_ISBUTTONENABLED, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::IsButtonChecked(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_ISBUTTONCHECKED, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::IsButtonPressed(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_ISBUTTONPRESSED, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::IsButtonHidden(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_ISBUTTONHIDDEN, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::IsButtonIndeterminate(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_ISBUTTONINDETERMINATE, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::SetState(int nID, UINT nState)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_SETSTATE, nID, MAKELPARAM(nState, 0)); }

_CJXLIB_INLINE int CCJToolBar::GetState(int nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_GETSTATE, nID); }

_CJXLIB_INLINE BOOL CCJToolBar::AddButtons(int nNumButtons, LPTBBUTTON lpButtons)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_ADDBUTTONS, nNumButtons, (LPARAM)lpButtons); }

_CJXLIB_INLINE BOOL CCJToolBar::InsertButton(int nIndex, LPTBBUTTON lpButton)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_INSERTBUTTON, nIndex, (LPARAM)lpButton); }

_CJXLIB_INLINE BOOL CCJToolBar::DeleteButton(int nIndex)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_DELETEBUTTON, nIndex); }

_CJXLIB_INLINE int CCJToolBar::GetButtonCount() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_BUTTONCOUNT); }

_CJXLIB_INLINE void CCJToolBar::Customize()
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_CUSTOMIZE, 0, 0L); }

_CJXLIB_INLINE int CCJToolBar::AddStrings(LPCTSTR lpszStrings)
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessage(TB_ADDSTRING, 0, (LPARAM)lpszStrings); }

_CJXLIB_INLINE void CCJToolBar::SetButtonStructSize(int nSize)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_BUTTONSTRUCTSIZE, nSize); }

_CJXLIB_INLINE BOOL CCJToolBar::SetButtonSize(CSize sz)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_SETBUTTONSIZE, 0, MAKELPARAM(sz.cx, sz.cy)); }

_CJXLIB_INLINE BOOL CCJToolBar::SetBitmapSize(CSize sz)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_SETBITMAPSIZE, 0, MAKELPARAM(sz.cx, sz.cy)); }

_CJXLIB_INLINE void CCJToolBar::AutoSize()
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_AUTOSIZE); }

_CJXLIB_INLINE CToolTipCtrl* CCJToolBar::GetToolTips() const
	{ ASSERT(::IsWindow(m_hWnd)); return (CToolTipCtrl*)CWnd::FromHandle((HWND)SendMessageC(TB_GETTOOLTIPS)); }

_CJXLIB_INLINE void CCJToolBar::SetToolTips(CToolTipCtrl* pTip)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_SETTOOLTIPS, (WPARAM)pTip->m_hWnd); }

_CJXLIB_INLINE void CCJToolBar::SetRows(int nRows, BOOL bLarger, LPRECT lpRect)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_SETROWS, MAKELPARAM(nRows, bLarger), (LPARAM)lpRect); }

_CJXLIB_INLINE int CCJToolBar::GetRows() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_GETROWS); }

_CJXLIB_INLINE BOOL CCJToolBar::SetCmdID(int nIndex, UINT nID)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_SETCMDID, nIndex, nID); }

_CJXLIB_INLINE UINT CCJToolBar::GetBitmapFlags() const
	{ ASSERT(::IsWindow(m_hWnd)); return (UINT) SendMessageC(TB_GETBITMAPFLAGS); }

// Wrappers for some of the newer messages--not complete
_CJXLIB_INLINE BOOL CCJToolBar::SetIndent(int indent)
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessage(TB_SETINDENT, indent); }

_CJXLIB_INLINE HIMAGELIST CCJToolBar::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return (HIMAGELIST)SendMessageC(TB_GETIMAGELIST); }

_CJXLIB_INLINE HIMAGELIST CCJToolBar::SetImageList(HIMAGELIST hImgList)
	{ ASSERT(::IsWindow(m_hWnd)); return (HIMAGELIST)SendMessage(TB_SETIMAGELIST, 0, (LPARAM)hImgList); }

_CJXLIB_INLINE int CCJToolBar::GetBitmap(UINT nIdButton) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_GETBITMAP, nIdButton); }

_CJXLIB_INLINE DWORD CCJToolBar::SetExtendedStyle(DWORD dwStyle)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)SendMessage(TB_SETEXTENDEDSTYLE, 0, dwStyle); }

_CJXLIB_INLINE BOOL CCJToolBar::GetRect(UINT nIdButton, RECT& rc) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_GETRECT, nIdButton, (LPARAM)&rc); }

_CJXLIB_INLINE DWORD CCJToolBar::GetToolbarStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)SendMessageC(TB_GETSTYLE); }

_CJXLIB_INLINE void CCJToolBar::SetToolbarStyle(DWORD dwStyle)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_SETSTYLE, 0, dwStyle); }

_CJXLIB_INLINE int CCJToolBar::HitTest(CPoint p) const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_HITTEST, 0, (LPARAM)&p); }

_CJXLIB_INLINE int CCJToolBar:: GetHotItem() const
	{ ASSERT(::IsWindow(m_hWnd)); return (int)SendMessageC(TB_GETHOTITEM); }

_CJXLIB_INLINE void CCJToolBar::SetHotItem(int iHot)
	{ ASSERT(::IsWindow(m_hWnd)); SendMessage(TB_SETHOTITEM, iHot); }

_CJXLIB_INLINE BOOL CCJToolBar::MapAccelerator(TCHAR ch, UINT& nID) const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL)SendMessageC(TB_MAPACCELERATOR, (WPARAM)ch, (LPARAM)&nID); }

_CJXLIB_INLINE CSize CCJToolBar::GetPadding() const
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)SendMessageC(TB_GETPADDING); }

_CJXLIB_INLINE CSize CCJToolBar::SetPadding(CSize sz) 
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD)SendMessage(TB_SETPADDING, 0, MAKELPARAM(sz.cx,sz.cy)); }

// Styles for toolbar buttons
#define TBBS_BUTTON     MAKELONG(TBSTYLE_BUTTON, 0) // this entry is button
#define TBBS_SEPARATOR  MAKELONG(TBSTYLE_SEP, 0)    // this entry is a separator
#define TBBS_CHECKBOX   MAKELONG(TBSTYLE_CHECK, 0)  // this is an auto check button
#define TBBS_GROUP      MAKELONG(TBSTYLE_GROUP, 0)  // marks the start of a group
#define TBBS_CHECKGROUP (TBBS_GROUP|TBBS_CHECKBOX)  // normal use of TBBS_GROUP
#define TBBS_DROPDOWN   MAKELONG(TBSTYLE_DROPDOWN, 0) // drop down style
#define TBBS_AUTOSIZE   MAKELONG(TBSTYLE_AUTOSIZE, 0) // autocalc button width
#define TBBS_NOPREFIX   MAKELONG(TBSTYLE_NOPREFIX, 0) // no accel prefix for this button

// styles for display states
#define TBBS_CHECKED    MAKELONG(0, TBSTATE_CHECKED)    // button is checked/down
#define TBBS_PRESSED    MAKELONG(0, TBSTATE_PRESSED)    // button is being depressed
#define TBBS_DISABLED   MAKELONG(0, TBSTATE_ENABLED)    // button is disabled
#define TBBS_INDETERMINATE  MAKELONG(0, TBSTATE_INDETERMINATE)  // third state
#define TBBS_HIDDEN     MAKELONG(0, TBSTATE_HIDDEN) // button is hidden
#define TBBS_WRAPPED    MAKELONG(0, TBSTATE_WRAP)   // button is wrapped at this point
#define TBBS_ELLIPSES   MAKELONG(0, TBSTATE_ELIPSES)
#define TBBS_MARKED     MAKELONG(0, TBSTATE_MARKED)

#endif //__TBCTL_H__

/////////////////////////////////////////////////////////////////////////////
