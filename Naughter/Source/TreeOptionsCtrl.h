/*
Module : TreeOptionsCtrl.h
Purpose: Defines the interface for an MFC class to implement a tree options control 
         similiar to the advanced tab as seen on the "Internet options" dialog in 
         Internet Explorer 4 and later
Created: PJN / 31-03-1999

Copyright (c) 1999 - 2012 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////// Defines ///////////////////////////////////////

#pragma once

#ifndef __TREEOPTIONSCTRL_H__
#define __TREEOPTIONSCTRL_H__

#ifndef CTREEOPTIONSCTRL_EXT_CLASS
#define CTREEOPTIONSCTRL_EXT_CLASS
#endif


/////////////////////////////// Includes //////////////////////////////////////

#ifndef __AFXDTCTL_H__
#pragma message("To avoid this message please put afxdtctl.h in your pre compiled header (normally stdafx.h)")
#include <afxdtctl.h>
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//forward declaration
class CTreeOptionsCtrl;
class CTreeOptionsBrowseButton;

//Class which represents an edit box inside a combo box as used by the tree options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsComboEdit : public CEdit
{
public:
//Constructors / Destructors
	CTreeOptionsComboEdit();

//Methods
  void SetListBuddy(CTreeOptionsCtrl* pTreeCtrl) 
  { 
    //validate our parameters
    ASSERT(pTreeCtrl);
    
    m_pTreeCtrl = pTreeCtrl; 
  }

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsComboEdit)
  
//Member variables  
  CTreeOptionsCtrl* m_pTreeCtrl;
};

//Class which represents a combo box used by the tree options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsCombo : public CComboBox
{
public:
//Constructors / Destructors
	CTreeOptionsCombo();
	virtual ~CTreeOptionsCombo() {};

protected:
//Misc methods
  void SetButtonBuddy(CTreeOptionsBrowseButton* pButton) 
  { 
    m_pButtonCtrl = pButton; 
  }
  void SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl)   
  { 
    //validate our parameters
    ASSERT(pTreeCtrl);
    
    m_pTreeCtrl = pTreeCtrl; 
  }
  void SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  virtual DWORD GetWindowStyle();
  virtual int GetDropDownHeight();
  BOOL IsRelatedWnd(CWnd* pChild);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsCombo)

//Member variables
  CTreeOptionsCtrl*         m_pTreeCtrl;
  CTreeOptionsBrowseButton* m_pButtonCtrl;
  HTREEITEM                 m_hTreeCtrlItem;
  BOOL                      m_bDoNotDestroyUponLoseFocus;
  CTreeOptionsComboEdit     m_Edit;
  
  friend class CTreeOptionsCtrl;
};

//Class which represents a combo box which allows a Font Name to be specified
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsFontNameCombo : public CTreeOptionsCombo
{
public:
//Constructors / Destructors
	CTreeOptionsFontNameCombo();
	virtual ~CTreeOptionsFontNameCombo() {};

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsFontNameCombo)

//Misc Methods
  virtual DWORD GetWindowStyle();
  int EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType);
  static int CALLBACK _EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData);
};

//Class which represents a combo box which allows a True / False value to be specified
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsBooleanCombo : public CTreeOptionsCombo
{
public:
//Constructors / Destructors
	CTreeOptionsBooleanCombo();
	virtual ~CTreeOptionsBooleanCombo() {};

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsBooleanCombo)
};

//forward declaration
class CTreeOptionsBrowseButton;
class CMultiLineEditFrameWnd;

//Class which represents an edit box used by the tree options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsEdit : public CEdit
{
public:
//Constructors / Destructors
	CTreeOptionsEdit();
	virtual ~CTreeOptionsEdit() {};

protected:
//Misc methods
  void SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl) 
  { 
    //validate our parameters
    ASSERT(pTreeCtrl);
    
    m_pTreeCtrl = pTreeCtrl; 
  }
  void SetButtonBuddy(CTreeOptionsBrowseButton* pButtonCtrl) 
  { 
    m_pButtonCtrl = pButtonCtrl; 
  }
  void SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  void SetMultiLineFrameWndBuddy(CMultiLineEditFrameWnd* pFrameWnd) 
  { 
    m_pFrameWnd = pFrameWnd; 
  }
  virtual DWORD GetWindowStyle();
  virtual int GetHeight(int nItemHeight);
  virtual void BrowseForFolder(const CString& sInitialFolder);
  virtual void BrowseForFile(const CString& sInitialFile);
  virtual CString GetBrowseForFolderCaption();
  virtual CString GetBrowseForFileCaption();
  virtual CString GetFileExtensionFilter();
  virtual CString FormControlText(const CString& sText);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

  static int CALLBACK SHBrowseSetSelProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsEdit)

//Member variables
  CTreeOptionsCtrl*         m_pTreeCtrl;
  CTreeOptionsBrowseButton* m_pButtonCtrl;
  HTREEITEM                 m_hTreeCtrlItem;
  CMultiLineEditFrameWnd*   m_pFrameWnd;
  BOOL                      m_bDoNotDestroyUponLoseFocus;
  
  friend class CTreeOptionsCtrl;
  friend class CTreeOptionsBrowseButton;
};

//Class which represents the spin control which can be used in association with an edit box by the tree options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsSpinCtrl : public CSpinButtonCtrl
{
public:
//Constructors / Destructors
	CTreeOptionsSpinCtrl();

protected:
//Misc methods
  void SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl);
  void SetEditBuddy(CTreeOptionsEdit* pEdit);
  void SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  virtual DWORD GetWindowStyle();
  virtual void GetDefaultRange(int &nLower, int& nUpper);

	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsSpinCtrl)

//Member variables
  CTreeOptionsCtrl* m_pTreeCtrl;
  HTREEITEM m_hTreeCtrlItem;
  CTreeOptionsEdit* m_pEdit;
  friend class CTreeOptionsCtrl;
};

//Class which represents the browse button which can be used in association with an edit box by the tree options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsBrowseButton : public CButton
{
public:
//Constructors / Destructors
	CTreeOptionsBrowseButton();

protected:
//Misc methods
  void            SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl);
  void            SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  void            SetEditBuddy(CTreeOptionsEdit* pEdit);
  void            SetFrameBuddy(CMultiLineEditFrameWnd* pFrameWnd);
  void            SetComboBuddy(CTreeOptionsCombo* pCombo);
  virtual DWORD   GetWindowStyle();
  virtual int     GetWidth();
  virtual CString GetCaption();
  COLORREF        GetColor() const 
  { 
    return m_Color; 
  }
  void            SetColor(COLORREF color);
  void            GetFontItem(LOGFONT* pLogFont);
  void            SetFontItem(const LOGFONT* pLogFont);
  virtual void    BrowseForColor();
  virtual void    BrowseForFont();
  virtual void    BrowseForOpaque();

	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClicked();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsBrowseButton)

//Member variables
  COLORREF                m_Color;
  LOGFONT                 m_Font;
  CTreeOptionsCtrl*       m_pTreeCtrl;
  CTreeOptionsEdit*       m_pEdit;
  CTreeOptionsCombo*      m_pCombo;
  CMultiLineEditFrameWnd* m_pFrame;
  HTREEITEM               m_hTreeCtrlItem;
  
  friend class CTreeOptionsCtrl;
};

//Class which is used for browsing for filenames
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsFileDialog : public CFileDialog
{
public:
//Constructors / Destructors
	CTreeOptionsFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL,
                       	 DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);

protected:
	DECLARE_DYNAMIC(CTreeOptionsFileDialog)

  virtual void OnInitDone();

	DECLARE_MESSAGE_MAP()
};

//Class which represents a date / time control used by the list options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsDateCtrl : public CDateTimeCtrl
{
public:
//Constructors / Destructors
	CTreeOptionsDateCtrl();

//Methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

protected:
//Misc methods
  void SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl) 
  { 
    //validate our parameters
    ASSERT(pTreeCtrl);
    
    m_pTreeCtrl = pTreeCtrl; 
  }
  void SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  virtual DWORD GetWindowStyle();
  virtual BOOL IsRelatedWnd(CWnd* pChild);
  void GetDateTime(SYSTEMTIME& st) const 
  { 
    memcpy(&st, &m_SystemTime, sizeof(SYSTEMTIME)); 
  }
  void SetDateTime(const SYSTEMTIME& st) 
  { 
    memcpy(&m_SystemTime, &st, sizeof(SYSTEMTIME)); 
  }
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsDateCtrl)

//Member variables
  CTreeOptionsCtrl* m_pTreeCtrl;
  HTREEITEM         m_hTreeCtrlItem;
  BOOL              m_bDoNotDestroyUponLoseFocus;
  SYSTEMTIME        m_SystemTime;
  
  friend class CTreeOptionsCtrl;
};

//Class which represents a time control used by the list options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsTimeCtrl : public CTreeOptionsDateCtrl
{
public:
//Constructors / Destructors
	CTreeOptionsTimeCtrl();

//methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

protected:
  virtual DWORD GetWindowStyle();

	DECLARE_MESSAGE_MAP()

  DECLARE_DYNCREATE(CTreeOptionsTimeCtrl)
};

//Class which represents IP Address control used by the list options class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsIPAddressCtrl : public CIPAddressCtrl
{
public:
//Constructors / Destructors
	CTreeOptionsIPAddressCtrl();

//methods
  virtual CString GetDisplayText(DWORD dwAddress);

protected:
//Misc methods
  void SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl) 
  { 
    //validate our parameters
    ASSERT(pTreeCtrl);
    
    m_pTreeCtrl = pTreeCtrl; 
  }
  void SetTreeItem(HTREEITEM hItem) 
  { 
    m_hTreeCtrlItem = hItem; 
  }
  virtual DWORD GetWindowStyle();
  DWORD GetIPAddress() const 
  { 
    return m_dwAddress; 
  }
  void SetIPAddress(DWORD dwAddress) 
  { 
    m_dwAddress = dwAddress; 
  }
  virtual BOOL IsRelatedWnd(CWnd* pChild);

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CTreeOptionsIPAddressCtrl)

//Member variables
  CTreeOptionsCtrl* m_pTreeCtrl;
  HTREEITEM         m_hTreeCtrlItem;
  BOOL              m_bDoNotDestroyUponLoseFocus;
  DWORD             m_dwAddress;

  friend class CTreeOptionsCtrl;
};

//Class which is stored in the tree options item data
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsItemData
{
public:
//Enums
  enum ControlType
  {
    Unknown,
    Normal,
    Spin,
    FileBrowser,
    FolderBrowser,
    ColorBrowser,
    FontBrowser,
    CheckBox,
    RadioButton,
    ComboBox,
    EditBox,
    DateTimeCtrl,
    IPAddressCtrl,
    OpaqueBrowser,
  };

//Data
  CRuntimeClass* m_pRuntimeClass1;
  CRuntimeClass* m_pRuntimeClass2;
  ControlType    m_Type;
	DWORD_PTR      m_dwItemData;
  COLORREF       m_Color;
  LOGFONT        m_Font;
  SYSTEMTIME     m_DateTime;
  DWORD          m_dwIPAddress;
  bool           m_bDrawColorForIcon;
  bool           m_bEnabled;
  bool           m_bChecked;
  bool           m_bSemiChecked;
  CString        m_sMultiLineText;

//Methods
  CTreeOptionsItemData() : m_Type(Unknown),
                           m_pRuntimeClass1(NULL),
                           m_pRuntimeClass2(NULL),
                           m_dwItemData(0),
                           m_Color(RGB(255, 0, 0)),
                           m_dwIPAddress(0),
                           m_bDrawColorForIcon(true),
                           m_bEnabled(true),
                           m_bChecked(false),
                           m_bSemiChecked(false)
  {
    memset(&m_Font, 0, sizeof(m_Font));
    memset(&m_DateTime, 0, sizeof(m_DateTime));
  }
};

class CTREEOPTIONSCTRL_EXT_CLASS CMultiLineEditFrameWnd : public CFrameWnd
{
public:
//Constructors / Destructors
	CMultiLineEditFrameWnd();

protected:
	afx_msg void OnSetFocus(CWnd* pOldWnd);
	afx_msg void OnClose();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CMultiLineEditFrameWnd)

  CTreeOptionsEdit* m_pEdit;

  friend class CTreeOptionsCtrl;
};

//The actual tree options control class
class CTREEOPTIONSCTRL_EXT_CLASS CTreeOptionsCtrl : public CTreeCtrl
{
public:
//Constructors / Destructors
  CTreeOptionsCtrl();
  virtual ~CTreeOptionsCtrl();

//Misc
  void         SetAutoSelect(BOOL bAutoSelect) { m_bAutoSelect = bAutoSelect; };
  BOOL         GetAutoSelect() const { return m_bAutoSelect; };
  void         SetToggleOverIconOnly(BOOL bToggle) { m_bToggleOverIconOnly = bToggle; };
  BOOL         GetToggleOverIconOnly() const { return m_bToggleOverIconOnly; };
  DWORD_PTR    GetUserItemData(HTREEITEM hItem) const;
  BOOL         SetUserItemData(HTREEITEM hItem, DWORD_PTR dwData);
  void         SetTextSeparator(const CString& sSeparator) { m_sSeparator = sSeparator; };
  CString      GetTextSeparator() const { return m_sSeparator; };
  void         Clear();
  virtual BOOL DeleteAllItems();
  void         SetUseThemes(BOOL bTheme);
  BOOL         GetUseThemes() const { return m_bThemed; };
  void         SetFlatStyleRadioButtons(BOOL bFlat);
  BOOL         GetFlatStyleRadioButtons() const { return m_bFlatStyleRadioButtons; };
  void         SetFlatStyleCheckBoxes(BOOL bFlat);
  BOOL         GetFlatStyleCheckBoxes() const { return m_bFlatStyleCheckBoxes; };

//Inserting items into the control
  HTREEITEM InsertGroup(LPCTSTR lpszItem, int nImage, HTREEITEM hParent = TVI_ROOT, HTREEITEM hAfter = TVI_LAST, DWORD_PTR dwItemData = 0);
  HTREEITEM InsertCheckBox(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck = TRUE, HTREEITEM hAfter = TVI_LAST, DWORD_PTR dwItemData = 0);
  HTREEITEM InsertRadioButton(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck = TRUE, HTREEITEM hAfter = TVI_LAST, DWORD_PTR dwItemData = 0);

//Validation methods
  BOOL IsGroup(HTREEITEM hItem) const;
  BOOL IsCheckBox(HTREEITEM hItem) const;
  BOOL IsRadioButton(HTREEITEM hItem) const;
  BOOL IsEditBox(HTREEITEM hItem) const;
  BOOL IsFileItem(HTREEITEM hItem) const;
  BOOL IsFolderItem(HTREEITEM hItem) const;
  BOOL IsColorItem(HTREEITEM hItem) const;
  BOOL IsFontItem(HTREEITEM hItem) const;
  BOOL IsDateTimeItem(HTREEITEM hItem) const;
  BOOL IsIPAddressItem(HTREEITEM hItem) const;
  BOOL IsOpaqueItem(HTREEITEM hItem) const;

//Setting / Getting combo states
  virtual BOOL SetCheckBox(HTREEITEM hItem, BOOL bCheck);
  virtual BOOL GetCheckBox(HTREEITEM hItem, BOOL& bCheck) const;

//Setting / Getting radio states
  virtual BOOL SetRadioButton(HTREEITEM hParent, int nIndex);
  virtual BOOL SetRadioButton(HTREEITEM hItem);
  virtual BOOL GetRadioButton(HTREEITEM hParent, int& nIndex, HTREEITEM& hCheckItem) const;
  virtual BOOL GetRadioButton(HTREEITEM hItem, BOOL& bCheck) const;

//Enable / Disbale items
  virtual BOOL SetGroupEnable(HTREEITEM hItem, BOOL bEnable);
  virtual BOOL SetCheckBoxEnable(HTREEITEM hItem, BOOL bEnable);
  virtual BOOL SetRadioButtonEnable(HTREEITEM hItem, BOOL bEnable);  
  virtual BOOL GetRadioButtonEnable(HTREEITEM hItem, BOOL& bEnable) const;
  virtual BOOL GetCheckBoxEnable(HTREEITEM hItem, BOOL& bEnable) const;

//Adding a combo box to an item
  BOOL      AddComboBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClass, DWORD_PTR dwItemData = 0);
  CString   GetComboText(HTREEITEM hItem) const;
  void      SetComboText(HTREEITEM hItem, const CString& sComboText);

//Adding an edit box (and a spin control or button) to an item
  BOOL      AddEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, DWORD_PTR dwItemData = 0);
  BOOL      AddEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassSpinCtrl, DWORD_PTR dwItemData = 0);
  CString   GetEditText(HTREEITEM hItem) const;
  void      SetEditText(HTREEITEM hItem, const CString& sEditText);
  void      SetMultiLineEditText(HTREEITEM hItem, const CString& sEditText);
  CString   GetMultiLineEditText(HTREEITEM hItem) const;

//Adding a file / Folder edit box (and a browse button) to an item
  BOOL      AddFileEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFileEditText(HTREEITEM hItem) const;
  void      SetFileEditText(HTREEITEM hItem, const CString& sEditText);
  BOOL      AddFolderEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFolderEditText(HTREEITEM hItem) const;
  void      SetFolderEditText(HTREEITEM hItem, const CString& sEditText);

//Adding a Color selector to an item
  BOOL      AddColorSelector(HTREEITEM hItem, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0, BOOL bDrawColorForIcon = TRUE);  
  COLORREF  GetColor(HTREEITEM hItem) const;
  void      SetColor(HTREEITEM hItem, COLORREF color);

//Adding a font name selector to an item
  BOOL      AddFontSelector(HTREEITEM hItem, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);  
  void      GetFontItem(HTREEITEM hItem, LOGFONT* pLogFont) const;
  void      SetFontItem(HTREEITEM hItem, const LOGFONT* pLogFont);

//Adding a Date Time  selector to an item
  BOOL      AddDateTime(HTREEITEM hItem, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData = 0);  
  void      GetDateTime(HTREEITEM hItem, SYSTEMTIME& st) const;
  void      SetDateTime(HTREEITEM hItem, const SYSTEMTIME& st);

//Adding an IP Address selector to an item
  BOOL      AddIPAddress(HTREEITEM hItem, CRuntimeClass* pRuntimeClassIPAddress, DWORD_PTR dwItemData = 0);  
  DWORD     GetIPAddress(HTREEITEM hItem) const;
  void      SetIPAddress(HTREEITEM hItem, DWORD dwAddress);

//Adding a Opaque selector to an item
  BOOL      AddOpaque(HTREEITEM hItem, CRuntimeClass* pRuntimeClass1, CRuntimeClass* pRuntimeClass2, DWORD_PTR dwItemData = 0);  
  DWORD_PTR GetOpaque(HTREEITEM hItem) const;
  void      SetOpaque(HTREEITEM hItem, DWORD_PTR dwItemData);

//Virtual methods    
  virtual HTREEITEM CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter = TVI_LAST);
  virtual HTREEITEM CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter = TVI_LAST);
  virtual void      HandleChildControlLosingFocus();

protected:
//Variables
  CTreeOptionsCombo*         m_pCombo;
  CTreeOptionsEdit*          m_pEdit;
  CMultiLineEditFrameWnd*    m_pMultiLineEditFrameWnd;
  CTreeOptionsSpinCtrl*      m_pSpin;
  CTreeOptionsBrowseButton*  m_pButton;
  CTreeOptionsDateCtrl*      m_pDateTime;
  CTreeOptionsIPAddressCtrl* m_pIPAddress;
  HTREEITEM                  m_hControlItem;
  BOOL                       m_bToggleOverIconOnly;
  BOOL                       m_bAutoSelect;
  CFont                      m_Font;
  CString                    m_sSeparator;
  BOOL                       m_bDoNotDestroy;
  BOOL                       m_bFlatStyleRadioButtons;
  BOOL                       m_bFlatStyleCheckBoxes;

//Windows XP Themes suport
  HINSTANCE                  m_hUXTheme;
  BOOL                       m_bThemed;

  typedef HTHEME (WINAPI OPENTHEMEDATA)(HWND, LPCWSTR);
  typedef OPENTHEMEDATA* LPOPENTHEMEDATA;
  LPOPENTHEMEDATA            m_lpfnOpenThemeData;

  typedef HRESULT (WINAPI DRAWTHEMEBACKGROUND)(HTHEME, HDC, int, int, const RECT*, const RECT*);
  typedef DRAWTHEMEBACKGROUND* LPDRAWTHEMEBACKGROUND;
  LPDRAWTHEMEBACKGROUND      m_lpfnDrawThemeBackground;

  typedef HRESULT (WINAPI DRAWTHEMEEDGE)(HTHEME, HDC, int, int, const RECT*, UINT, UINT, RECT*);
  typedef DRAWTHEMEEDGE* LPDRAWTHEMEEDGE;
  LPDRAWTHEMEEDGE            m_lpfnDrawThemeEdge;

  typedef HRESULT (WINAPI CLOSETHEMEDATA)(HTHEME);
  typedef CLOSETHEMEDATA* LPCLOSETHEMEDATA;
  LPCLOSETHEMEDATA m_lpfnCloseThemeData;

//Methods
  virtual void  DestroyOldChildControl();
  virtual void  RemoveChildControlText(HTREEITEM hItem);
  virtual void  CreateNewChildControl(HTREEITEM hItem);
  virtual void  CreateSpinCtrl(CRuntimeClass* pRuntimeClassSpinCtrl, CRect rItem, CRect rText, CRect rPrimaryControl);
  virtual void  CreateBrowseButton(CRuntimeClass* pRuntimeClassBrowseButton, CRect rItem, CRect rText);
  virtual void  UpdateTreeControlValueFromChildControl(HTREEITEM hItem);
  virtual BOOL  SetCheckBoxIntern(HTREEITEM hItem, BOOL bCheck);
  virtual BOOL  HandleCheckBox(HTREEITEM hItem, BOOL bCheck);
  virtual BOOL  SetSemiCheckBox(HTREEITEM hItem, BOOL bSemi);
  virtual BOOL  GetSemiCheckBox(HTREEITEM hItem, BOOL& bSemi) const;
  virtual void  MemDeleteAllItems(HTREEITEM hParent);
  virtual BOOL  GetItemIconOffset(int left, int right, int y, int& nStartIcon, int& nEndIcon);
  virtual CRect GetItemIconPosition(const CRect& rItemIcon);
  virtual void  HandleLoseFocusLogic(CWnd* pNewWnd);
  static BOOL CALLBACK _EnumIPAddressWindows(HWND hwnd, LPARAM lParam);

	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCBNKillFocus();
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnSetFocusToChild(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnRepositionChild(WPARAM wParam, LPARAM lParam);
  afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_DYNAMIC(CTreeOptionsCtrl)

	DECLARE_MESSAGE_MAP()

  friend class CTreeOptionsEdit;
  friend class CTreeOptionsStatic;
  friend class CTreeOptionsCombo;
  friend class CTreeOptionsSpinCtrl;
  friend class CTreeOptionsBrowseButton;
  friend class CTreeOptionsDateCtrl;
  friend class CTreeOptionsIPAddressCtrl;
  friend class CTreeOptionsComboEdit;
};

//Dialog Data exchange support
void DDX_TreeCheck(CDataExchange* pDX, int nIDC, HTREEITEM hItem, BOOL& bCheck);
void DDX_TreeRadio(CDataExchange* pDX, int nIDC, HTREEITEM hParent, int& nIndex);
void DDX_TreeEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText);
void DDX_TreeEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, int& nValue);
void DDX_TreeCombo(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText);
void DDX_TreeFileEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText);
void DDX_TreeFolderEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText);
void DDX_TreeColor(CDataExchange* pDX, int nIDC, HTREEITEM hItem, COLORREF& color);
void DDX_TreeFont(CDataExchange* pDX, int nIDC, HTREEITEM hItem, LOGFONT* pLogFont);
void DDX_TreeBoolean(CDataExchange* pDX, int nIDC, HTREEITEM hItem, BOOL& bValue);
void DDX_TreeDateTime(CDataExchange* pDX, int nIDC, HTREEITEM hItem, SYSTEMTIME& st);
void DDX_TreeIPAddress(CDataExchange* pDX, int nIDC, HTREEITEM hItem, DWORD& dwAddress);

#endif //__TREEOPTIONSCTRL_H__
