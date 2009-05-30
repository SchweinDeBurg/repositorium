/*
Module : ListptionsCtrl.h
Purpose: Defines the interface for an MFC class to implement a list options control 
         similiar to properties control which appears in Visual Basic 6
Created: PJN / 04-12-2001

Copyright (c) 2001 - 2008 by PJ Naughter.  (Web: www.naughter.com, Email: pjna@naughter.com)

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

#ifndef __LISTOPTIONSCTRL_H__
#define __LISTOPTIONSCTRL_H__

#ifndef CLISTOPTIONSCTRL_EXT_CLASS
#define CLISTOPTIONSCTRL_EXT_CLASS
#endif
#ifndef CLISTOPTIONSCTRL_EXT_API
#define CLISTOPTIONSCTRL_EXT_API
#endif


/////////////////////////////// Includes //////////////////////////////////////

#ifndef __AFXDTCTL_H__
#pragma message("To avoid this message, please put afxdtctl.h in your pre compiled header (normally stdafx.h)")
#include <afxdtctl.h>
#endif


/////////////////////////////// Classes ///////////////////////////////////////

//forward declaration
class CListOptionsCtrl;
class CListOptionsBrowseButton;


//Class which represents an edit box inside a combo box as used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsComboEdit : public CEdit
{
public:
//Constructors / Destructors
	CListOptionsComboEdit();

//Methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };

protected:
	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsComboEdit)
  
//Member variables  
  CListOptionsCtrl* m_pListCtrl;
};

//Class which represents a combo box used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsCombo : public CComboBox
{
public:
//Constructors / Destructors
	CListOptionsCombo();
	virtual ~CListOptionsCombo() {};

protected:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
  
    m_pListCtrl = pListCtrl; 
    m_Edit.SetListBuddy(pListCtrl);
  };
  void SetButtonBuddy(CListOptionsBrowseButton* pButtonCtrl) 
  { 
    m_pButtonCtrl = pButtonCtrl; 
  };
  void SetListIndex(int nIndex) 
  { 
    m_nListCtrlIndex = nIndex; 
  };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual int   GetDropDownHeight();
  virtual BOOL  IsRelatedWnd(CWnd* pChild);
  virtual void  OnShowNewValue() {};

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg LRESULT OnCtlColor(WPARAM wParam, LPARAM lParam);
	afx_msg void OnDestroy();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsCombo)

//Member variables
  CListOptionsCtrl*         m_pListCtrl;
  CListOptionsBrowseButton* m_pButtonCtrl;
  int                       m_nListCtrlIndex;
  BOOL                      m_bDoNotDestroyUponLoseFocus;
  CListOptionsComboEdit     m_Edit;
  
  friend class CListOptionsCtrl;
};

//Class which represents a combo box which allows a Font Name to be specified
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsFontNameCombo : public CListOptionsCombo
{
public:
//Constructors / Destructors
	virtual ~CListOptionsFontNameCombo() {};

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsFontNameCombo)

//Misc Methods
  virtual DWORD GetWindowStyle();
  int EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType);
  static int CALLBACK _EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData);
};

//Class which represents a combo box which allows a True / False value to be specified
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsBooleanCombo : public CListOptionsCombo
{
public:
//Constructors / Destructors
	virtual ~CListOptionsBooleanCombo() {};

protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsBooleanCombo)
};

//forward declaration
class CListOptionsBrowseButton;

//Class which represents an edit box used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsEdit : public CEdit
{
public:
//Constructors / Destructors
	CListOptionsEdit();
	virtual ~CListOptionsEdit() {};

protected:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };
  void SetButtonBuddy(CListOptionsBrowseButton* pButtonCtrl) 
  { 
    m_pButtonCtrl = pButtonCtrl; 
  };
  void SetListIndex(int nIndex) 
  { 
    m_nListCtrlIndex = nIndex; 
  };

//Virtual methods
  virtual DWORD   GetWindowStyle();
  virtual int     GetHeight(int nItemHeight);
  virtual void    BrowseForFolder(const CString& sInitialFolder);
  virtual void    BrowseForFile(const CString& sInitialFile);
  virtual CString GetBrowseForFolderCaption();
  virtual CString GetBrowseForFileCaption();
  virtual CString GetFileExtensionFilter();
  virtual void    OnShowNewValue() {};

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

  static int CALLBACK SHBrowseSetSelProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsEdit)

//Member variables
  CListOptionsCtrl*         m_pListCtrl;
  CListOptionsBrowseButton* m_pButtonCtrl;
  int                       m_nListCtrlIndex;
  BOOL                      m_bDoNotDestroyUponLoseFocus;
  
  friend class CListOptionsCtrl;
  friend class CListOptionsBrowseButton;
};

//Class which represents the spin control which can be used in association with an edit box by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsSpinCtrl : public CSpinButtonCtrl
{
public:
//Constructors / Destructors
	CListOptionsSpinCtrl();
	virtual ~CListOptionsSpinCtrl() {};

protected:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };
  void SetEditBuddy(CListOptionsEdit* pEdit) 
  { 
    m_pEdit = pEdit; 
  };
  void SetListIndex(int nIndex) 
  { 
    m_nListCtrlIndex = nIndex; 
  };
  virtual DWORD GetWindowStyle();
  virtual void GetDefaultRange(int &lower, int& upper);

	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsSpinCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int               m_nListCtrlIndex;
  CListOptionsEdit* m_pEdit;
  
  friend class CListOptionsCtrl;
};

//Class which represents the browse button which can be used in association with an edit box by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsBrowseButton : public CButton
{
public:
//Constructors / Destructors
	CListOptionsBrowseButton();
	virtual ~CListOptionsBrowseButton() {};

protected:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };
  void SetEditBuddy(CListOptionsEdit* pEdit) 
  { 
    m_pEdit = pEdit; 
  };
  void SetComboBuddy(CListOptionsCombo* pCombo)
  {
    m_pCombo = pCombo;
  }

  void            SetListIndex(int nIndex) { m_nListCtrlIndex = nIndex; };
  virtual DWORD   GetWindowStyle();
  virtual int     GetWidth();
  virtual CString GetCaption();
  COLORREF        GetColor() const { return m_Color; };
  void            SetColor(COLORREF color);
  void            GetFontItem(LOGFONT* pLogFont);
  void            SetFontItem(const LOGFONT* pLogFont);

//Virtual methods
  virtual void    BrowseForColor();
  virtual void    BrowseForFont();
  virtual void    BrowseForOpaque();
  virtual void    OnShowNewValue() {};

protected:
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnClicked();

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsBrowseButton)

//Member variables
  COLORREF           m_Color;
  LOGFONT            m_Font;
  CListOptionsCtrl*  m_pListCtrl;
  CListOptionsEdit*  m_pEdit;
  CListOptionsCombo* m_pCombo;
  int                m_nListCtrlIndex;
  
  friend class CListOptionsCtrl;
};

//Class which is used for browsing for filenames
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsFileDialog : public CFileDialog
{
public:
//Constructors / Destructors
	CListOptionsFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt = NULL, LPCTSTR lpszFileName = NULL,
                       	 DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,	LPCTSTR lpszFilter = NULL, CWnd* pParentWnd = NULL);

protected:
  virtual void OnInitDone();

	DECLARE_MESSAGE_MAP()
	DECLARE_DYNAMIC(CListOptionsFileDialog)
};

//Class which represents a date / time control used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsDateCtrl : public CDateTimeCtrl
{
public:
//Constructors / Destructors
	CListOptionsDateCtrl();
	virtual ~CListOptionsDateCtrl() {};

//Methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

protected:
//Misc methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };
  void SetListIndex(int nIndex) 
  { 
    m_nListCtrlIndex = nIndex; 
  };
  void GetDateTime(SYSTEMTIME& st) const 
  { 
    memcpy(&st, &m_SystemTime, sizeof(SYSTEMTIME)); 
  };
  void SetDateTime(const SYSTEMTIME& st) 
  { 
    memcpy(&m_SystemTime, &st, sizeof(SYSTEMTIME)); 
  };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual BOOL IsRelatedWnd(CWnd* pChild);
  virtual void OnShowNewValue() {};

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsDateCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int               m_nListCtrlIndex;
  BOOL              m_bDoNotDestroyUponLoseFocus;
  SYSTEMTIME        m_SystemTime;

  friend class CListOptionsCtrl;
};

//Class which represents a time control used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsTimeCtrl : public CListOptionsDateCtrl
{
public:
//Constructors / Destructors
	virtual ~CListOptionsTimeCtrl() {};

//methods
  virtual CString GetDisplayText(const SYSTEMTIME& st);

protected:
  virtual DWORD GetWindowStyle();

	DECLARE_MESSAGE_MAP()

  DECLARE_DYNCREATE(CListOptionsTimeCtrl)
};

//Class which represents IP Address control used by the list options class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsIPAddressCtrl : public CIPAddressCtrl
{
public:
//Constructors / Destructors
	CListOptionsIPAddressCtrl();
	virtual ~CListOptionsIPAddressCtrl() {};

//methods
  virtual CString GetDisplayText(DWORD dwAddress);

protected:
//Misc methods
  void  SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };
  void  SetListIndex(int nIndex) 
  { 
    m_nListCtrlIndex = nIndex; 
  };
  DWORD GetIPAddress() const { return m_dwAddress; };
  void  SetIPAddress(DWORD dwAddress) { m_dwAddress = dwAddress; };

//Virtual methods
  virtual DWORD GetWindowStyle();
  virtual BOOL IsRelatedWnd(CWnd* pChild);
  virtual void OnShowNewValue() {};

	afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode ();
	afx_msg void OnKillFocus(CWnd* pNewWnd);

	DECLARE_MESSAGE_MAP()
  DECLARE_DYNCREATE(CListOptionsIPAddressCtrl)

//Member variables
  CListOptionsCtrl* m_pListCtrl;
  int               m_nListCtrlIndex;
  BOOL              m_bDoNotDestroyUponLoseFocus;
  DWORD             m_dwAddress;
  
  friend class CListOptionsCtrl;
};

//Class which will be the header control (subclassed) for the List Options control
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsHeaderCtrl : public CHeaderCtrl
{
public:
//Constructors / Destructors
	CListOptionsHeaderCtrl();
	virtual ~CListOptionsHeaderCtrl() {};

//Methods
  void SetListBuddy(CListOptionsCtrl* pListCtrl) 
  { 
    //Validate our parameters
    ASSERT(pListCtrl);
    
    m_pListCtrl = pListCtrl; 
  };

protected:
  afx_msg BOOL OnBeginTrack(NMHDR* pNMHDR, LRESULT* pResult);

	DECLARE_MESSAGE_MAP()

//Member variables
  CListOptionsCtrl* m_pListCtrl;
};

//Class which is stored in the list options item data 
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsItemData
{
public:
//Enums
  enum ControlType
  {
    Unknown,
    OpaqueBrowser,
    Spin,
    FileBrowser,
    FolderBrowser,
    ColorBrowser,
    FontBrowser,
    ComboBox,
    EditBox,
    DateTimeCtrl,
    IPAddressCtrl,
  };

//Data
  CRuntimeClass* m_pRuntimeClass1;
  CRuntimeClass* m_pRuntimeClass2;
  ControlType    m_Type;
  DWORD_PTR      m_dwItemData;
  COLORREF       m_Color;
  LOGFONT        m_Font;
  SYSTEMTIME     m_DateTime;
  bool           m_bDrawColorForIcon;
  DWORD          m_dwIPAddress;
  bool           m_bPromptBoldText;
  bool           m_bPromptItalicText;
  bool           m_bPromptUnderlineText;
  bool           m_bValueBoldText;
  bool           m_bValueItalicText;
  bool           m_bValueUnderlineText;
  COLORREF       m_PromptTextColor;
  COLORREF       m_PromptBackgroundColor;
  COLORREF       m_ValueTextColor;
  COLORREF       m_ValueBackgroundColor;

//Methods
  CListOptionsItemData() : m_Type(Unknown),
                           m_pRuntimeClass1(NULL),
                           m_pRuntimeClass2(NULL),
                           m_dwItemData(0),
                           m_Color(RGB(255, 0, 0)),
                           m_bDrawColorForIcon(true),
                           m_dwIPAddress(0),
                           m_bPromptBoldText(false),
                           m_bPromptItalicText(false),
                           m_bPromptUnderlineText(false),
                           m_bValueBoldText(false),
                           m_bValueItalicText(false),
                           m_bValueUnderlineText(false),
                           m_PromptTextColor(GetSysColor(COLOR_WINDOWTEXT)),
                           m_PromptBackgroundColor(GetSysColor(COLOR_WINDOW)),
                           m_ValueTextColor(m_PromptTextColor),
                           m_ValueBackgroundColor(m_PromptBackgroundColor)
  {
    memset(&m_Font, 0, sizeof(m_Font));
    memset(&m_DateTime, 0, sizeof(m_DateTime));
  }
};

//The actual list options control class
class CLISTOPTIONSCTRL_EXT_CLASS CListOptionsCtrl : public CListCtrl
{
public:
//Constructors / Destructors
  CListOptionsCtrl();

//Misc
  void      SetAutoSelect(BOOL bAutoSelect) { m_bAutoSelect = bAutoSelect; };
  BOOL      GetAutoSelect() const { return m_bAutoSelect; };
  DWORD_PTR GetUserItemData(int nIndex);
  void      RequestChildReposition();

//Validation methods
  BOOL      IsBoolean(int nIndex);
  BOOL      IsEditBox(int nIndex);
  BOOL      IsFileItem(int nIndex);
  BOOL      IsFolderItem(int nIndex);
  BOOL      IsColorItem(int nIndex);
  BOOL      IsFontItem(int nIndex);
  BOOL      IsOpaqueItem(int nIndex);

//Adding a boolean combo box to an item
  BOOL      AddBoolean(int nIndex, DWORD_PTR dwItemData = 0);
  BOOL      SetBoolean(int nIndex, BOOL bValue);
  BOOL      GetBoolean(int nIndex, BOOL& bValue);

//Adding a combo box to an item
  BOOL      AddComboBox(int nIndex, CRuntimeClass* pRuntimeClass, DWORD_PTR dwItemData = 0);
  CString   GetComboText(int nIndex);
  void      SetComboText(int nIndex, const CString& sComboText);

//Adding an edit box (and a spin control) to an item
  BOOL      AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, DWORD_PTR dwItemData = 0);
  BOOL      AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassSpinCtrl, DWORD_PTR dwItemData = 0);
  CString   GetEditText(int nIndex);
  void      SetEditText(int nIndex, const CString& sEditText);

//Adding a file / Folder edit box (and a browse button) to an item
  BOOL      AddFileEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFileEditText(int nIndex);
  void      SetFileEditText(int nIndex, const CString& sEditText);

  BOOL      AddFolderEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);
  CString   GetFolderEditText(int nIndex);
  void      SetFolderEditText(int nIndex, const CString& sEditText);

//Adding a Color selector to an item
  BOOL      AddColorSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0, BOOL bDrawColorForIcon = TRUE);  
  COLORREF  GetColor(int nIndex);
  void      SetColor(int nIndex, COLORREF color);

//Adding a Opaque selector to an item
  BOOL      AddOpaque(int nIndex, CRuntimeClass* pRuntimeClass1, CRuntimeClass* pRuntimeClass2, DWORD_PTR dwItemData = 0);  
  DWORD_PTR GetOpaque(int nIndex);
  void      SetOpaque(int nIndex, DWORD_PTR dwItemData);

//Adding a font name selector to an item
  BOOL      AddFontSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData = 0);  
  void      GetFontItem(int nIndex, LOGFONT* pLogFont);
  void      SetFontItem(int nIndex, const LOGFONT* pLogFont);

//Adding a Date Time  selector to an item
  BOOL      AddDateTime(int nIndex, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData = 0);  
  void      GetDateTime(int nIndex, SYSTEMTIME& st);
  void      SetDateTime(int nIndex, const SYSTEMTIME& st);

//Adding an IP Address selector to an item
  BOOL      AddIPAddress(int nIndex, CRuntimeClass* pRuntimeClassIPAddress, DWORD_PTR dwItemData = 0);  
  DWORD     GetIPAddress(int nIndex);
  void      SetIPAddress(int nIndex, DWORD dwAddress);

protected:
//Variables
  CListOptionsCombo*         m_pCombo;
  CListOptionsEdit*          m_pEdit;
  CListOptionsSpinCtrl*      m_pSpin;
  CListOptionsBrowseButton*  m_pButton;
  CListOptionsDateCtrl*      m_pDateTime;
  CListOptionsIPAddressCtrl* m_pIPAddress;
  int                        m_nControlItem;
  BOOL                       m_bAutoSelect;
  CListOptionsHeaderCtrl     m_ctrlHeader;
  CFont                      m_Font;
  CFont                      m_BoldFont;
  CFont                      m_ItalicFont;
  CFont                      m_UnderlineFont;
  CFont                      m_ItalicBoldFont;
  CFont                      m_ItalicUnderlineFont;
  CFont                      m_ItalicUnderlineBoldFont;

//Methods
  virtual void DestroyOldChildControl();
  virtual void CreateNewChildControl(int nindex);
  virtual void CreateSpinCtrl(CRuntimeClass* pRuntimeClassSpinCtrl, CRect rItem, CRect rPrimaryControl);
  virtual void CreateBrowseButton(CRuntimeClass* pRuntimeClassBrowseButton, CRect rItem, int nColumnWidth);
  virtual void UpdateListControlValueFromChildControl(int nIndex);
  virtual void HandleChildControlLosingFocus();
  int          GetSelectedItem();
  void         SetSelected(int nIndex, BOOL bSelect);
	virtual void PreSubclassWindow();
	virtual void HandleLoseFocusLogic(CWnd* pNewWnd);

	afx_msg void OnDestroy();
	afx_msg void OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);
	afx_msg void OnKeyDown( UINT nChar, UINT nRepCnt, UINT nFlags );
	afx_msg void OnKillFocus(CWnd* pNewWnd);
	afx_msg void OnCBNKillFocus();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg BOOL OnClick(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
  afx_msg LRESULT OnSetFocusToChild(WPARAM wParam, LPARAM lParam);
  afx_msg LRESULT OnRepositionChild(WPARAM wParam, LPARAM lParam);
  afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);

  DECLARE_DYNAMIC(CListOptionsCtrl)

	DECLARE_MESSAGE_MAP()

  friend class CListOptionsEdit;
  friend class CListOptionsStatic;
  friend class CListOptionsCombo;
  friend class CListOptionsSpinCtrl;
  friend class CListOptionsBrowseButton;
  friend class CListOptionsHeaderCtrl;
  friend class CListOptionsDateCtrl;
  friend class CListOptionsIPAddressCtrl;
  friend class CListOptionsComboEdit;
};

//Dialog Data exchange support
void CLISTOPTIONSCTRL_EXT_API DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
void CLISTOPTIONSCTRL_EXT_API DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, int& nValue);
void CLISTOPTIONSCTRL_EXT_API DDX_ListCombo(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
void CLISTOPTIONSCTRL_EXT_API DDX_ListFileEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
void CLISTOPTIONSCTRL_EXT_API DDX_ListFolderEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText);
void CLISTOPTIONSCTRL_EXT_API DDX_ListColor(CDataExchange* pDX, int nIDC, int nIndex, COLORREF& color);
void CLISTOPTIONSCTRL_EXT_API DDX_ListFont(CDataExchange* pDX, int nIDC, int nIndex, LOGFONT* pLogFont);
void CLISTOPTIONSCTRL_EXT_API DDX_ListBoolean(CDataExchange* pDX, int nIDC, int nIndex, BOOL& bValue);
void CLISTOPTIONSCTRL_EXT_API DDX_ListDateTime(CDataExchange* pDX, int nIDC, int nIndex, SYSTEMTIME& st);
void CLISTOPTIONSCTRL_EXT_API DDX_ListIPAddress(CDataExchange* pDX, int nIDC, int nIndex, DWORD& dwAddress);

#endif //__LISTOPTIONSCTRL_H__
