/*
Module : ListOptionsCtrl.cpp
Purpose: Implementation for an MFC class to implement a list options control 
         similiar to the properties control which appears in Visual Basic 6
Created: PJN / 04-12-2001
History: PJN / 13-01-2002 1. Made _ListOptionsItemData structure globally available and renamed it
                          to CListOptionsItemData.
                          2. Each child control class now is initialized with the index of the list
                          view control at which it is created. This allows added flexibility in what
                          a child control can do.
                          3. Added a method to add an "Opaque Browser" to the list options control. An
                          Opaque Browser is where the list options control allows a end user specified
                          structure to be edited by the list options control without it explicitly
                          knowing what it is editing. A standard "..." browse button is used and virtual
                          functions are used to allow end user code to implement the required "browsing"
                          functionality at the right time.
                          4. Added a WM_SIZE handler to allow the class to work correctly where the 
                          containing view or dialog resizes it for layout reasons.
                          5. Fixed a problem in CListOptionsCtrl::OnKillFocus which was not checking the
                          value of GetSelectedItem before using it
                          6. Updated copyright messages in code and in documentation
         PJN / 23-01-2002 1. Improved the "Opaque Browser" support to allow the two runtime classes to be 
                          specified e.g. you can have a combo box in addition to a button
         PJN / 31-05-2002 1. Color is now optionally drawn in the icon for color browser items
         PJN / 01-06-2002 1. Moved application and code to VC 6 to allow item 2 below.
                          2. Now fully supports the CDateTimeCtrl for editing of dates and times
         PJN / 02-06-2002 1. Now fully supports the CIPAddressCtrl for editing of IP addresses
         PJN / 17-10-2202 1. Changed function "BrowserForOpaque" to "BrowseForOpaque"
         PJN / 05-05-2004 1. Fixed some compiler warnings when the code is compiled using VC 7.x. Please
                          note that to implement these fixes, the code will now require the Platform SDK
                          to be installed if you compile the code on VC 6.
         PJN / 26-11-2004 1. Fixed a bug where in place editing would fail on a multiple monitor setup 
                          when the listoptions control was on a monitor which had negative coordinates 
                          relative to the primary display
         PJN / 03-05-2006 1. Updated the copyright details.
                          2. Control now allows the per item text to be Bold, italic or underlined or any combination
                          of the three.
                          3. Optimizied the construction of member variables in all classes
                          4. Inlined the virtual destructors.
                          5. Provided a set of macros which allow the code to be easily added to an extension
                          DLL.
                          6. Control now uses WM_USER instead of WM_APP for private messages
                          7. Prompt and value text color and background color can also be customized.
                          8. Updated documentation to use the same style as the web site.
                          9. Did a spell check of the documentation.
         PJN / 08-05-2006 1. Removed unnecessary message map for CListOptionsSpinCtrl WM_CHAR
                          2. Removed unnecessary message map for CListOptionsBrowseButton WM_CHAR
                          3. Reworked CListOptionsDateCtrl::GetDisplayText to handle variable length data
                          4. Reworked CListOptionsTimeCtrl::GetDisplayText to handle variable length data
                          5. Removed unnecessary message map for CListOptionsTimeCtrl WM_CREATE
                          6. CListOptionsEdit::GetBrowseForFolderCaption(), CListOptionsEdit::GetBrowseForFileCaption()
                          and CListOptionsEdit::GetFileExtensionFilter() now use string resources instead of string 
                          literals.
         PJN / 07-07-2006 1. Code now uses newer C++ style casts instead of C style casts.
                          2. Removed various unused constructors and destructors in the sample app's code.
                          3. Updated code to compile cleanly using VC 2005.
         PJN / 23-12-2007 1. Updated copyright details.
                          2. Removed VC 6 style AppWizard comments from the code.
                          3. Optimized CListOptionsItemData constructor code
                          4. Boolean member variables of CListOptionsItemData have now been made "bool" instead of "BOOL"
                          5. Fixed an issue where if you create a combo box with a style of CBS_DROPDOWN instead of 
                          CBS_DROPDOWNLIST, then the combo box would not be properly deactivated when it loses focus to a 
                          control outside of the ListOptionsCtrl while the cursor is in the combo box edit field. Thanks
                          to Tobias Wolf for reporting this issue.
                          6. Fixed a crash where you select a combo box item, select something in it and then hit tab.
                          7. Focus is now correctly transferred to a list options button if you hit tab on a list options
                          combo box.
         PJN / 15-06-2008 1. Updated copyright details
                          2. Code now compiles cleanly using Code Analysis (/analyze)
                          3. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          4. The code now only supports VC 2005 or later. 
                          5. Replaced all calls to CopyMemory with memcpy

Copyright (c) 2001 - 2008 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


//////////////// Includes ////////////////////////////////////////////

#include "stdafx.h"
#include "ListOptionsCtrlRes.h"
#ifndef _SHLOBJ_H_
#pragma message("To avoid this message, please put shlobj.h in your pre compiled header (normally stdafx.h)")
#include <Shlobj.h>
#endif
#include "ListOptionsCtrl.h"


//////////////// Macros / Locals /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT LIST_OPTIONS_COMBOBOX_ID = 100;
const UINT LIST_OPTIONS_EDITBOX_ID  = 101;
const UINT LIST_OPTIONS_SPINCTRL_ID = 102;
const UINT LIST_OPTIONS_BROWSEBUTTONCTRL_ID = 103;
const UINT LIST_OPTIONS_STATIC_ID = 104;
const UINT LIST_OPTIONS_DATETIMECTRL_ID  = 105;
const UINT LIST_OPTIONS_IPADDRESSCTRL_ID  = 106;

#define WM_LOC_SETFOCUS_TO_CHILD WM_USER + 1
#define WM_LOC_REPOSITION_CHILD_CONTROL WM_USER + 2


//////////////// Implementation //////////////////////////////////////

IMPLEMENT_DYNAMIC(CListOptionsCtrl, CListCtrl)

BEGIN_MESSAGE_MAP(CListOptionsCtrl, CListCtrl)
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
	ON_WM_SIZE()
  ON_WM_MOUSEWHEEL()
  ON_MESSAGE(WM_LOC_SETFOCUS_TO_CHILD, OnSetFocusToChild)
  ON_MESSAGE(WM_LOC_REPOSITION_CHILD_CONTROL, OnRepositionChild)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT_EX(LVN_DELETEITEM, OnDeleteItem)
  ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, OnCustomDraw)
  ON_CBN_KILLFOCUS(LIST_OPTIONS_COMBOBOX_ID, OnCBNKillFocus)
END_MESSAGE_MAP()

CListOptionsCtrl::CListOptionsCtrl() : m_pCombo(NULL),
                                       m_pEdit(NULL),
                                       m_pSpin(NULL),
                                       m_pButton(NULL),
                                       m_pDateTime(NULL),
                                       m_pIPAddress(NULL),
                                       m_nControlItem(-10),
                                       m_bAutoSelect(FALSE)
{
}

LRESULT CListOptionsCtrl::OnSetFocusToChild(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  if (m_pCombo)
    m_pCombo->SetFocus();
  else if (m_pEdit)
    m_pEdit->SetFocus();
  else if (m_pDateTime)
    m_pDateTime->SetFocus();
  else if (m_pIPAddress)
    m_pIPAddress->SetFocus();
  
  return 0L;  
}

LRESULT CListOptionsCtrl::OnRepositionChild(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  int nIndex = GetSelectedItem();
  if (nIndex != -1)
  {
    UpdateListControlValueFromChildControl(nIndex);
    DestroyOldChildControl();
    CreateNewChildControl(nIndex);
  }

  return 0L;
}

DWORD_PTR CListOptionsCtrl::GetUserItemData(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  return pItemData->m_dwItemData;
}

void CListOptionsCtrl::OnSize(UINT nType, int cx, int cy) 
{
  //Clean up any controls currently open we used
  if (m_nControlItem != -1)
  {
    UpdateListControlValueFromChildControl(m_nControlItem);
    DestroyOldChildControl();
  }

  //let the parent class do its thing
	CListCtrl::OnSize(nType, cx, cy);
}

BOOL CListOptionsCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  //Clean up any controls currently open we used
  if (m_nControlItem != -1)
  {
    UpdateListControlValueFromChildControl(m_nControlItem);
    DestroyOldChildControl();
  }

  //Let the parent class do its thing
  return CListCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CListOptionsCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);
	
	*pResult = 0;

  //Free up the memory we had allocated in the item data (if needed)
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(pNMListView->iItem));
  if (pItemData)
    delete pItemData;

  return FALSE;
}

void CListOptionsCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (nChar == VK_RIGHT)
	{
		int nItem = GetSelectedItem();
		if (GetItemData(nItem) && m_nControlItem != -1)
		{ 
      // if we have a children and VK_RIGHT -> Focus on it !
			CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nItem));
      if (pItemData->m_pRuntimeClass1)
      {
			  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsCombo)))
			  {
				  if (m_pCombo->IsWindowVisible())
					  m_pCombo->SetFocus();
			  }
			  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsEdit)))
				{
					if (m_pEdit->IsWindowVisible())
						m_pEdit->SetFocus();
				}
				else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsDateCtrl)))
				{
					if (m_pDateTime->IsWindowVisible())
						m_pDateTime->SetFocus();
				}
				else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsIPAddressCtrl)))
				{
					if (m_pIPAddress->IsWindowVisible())
						m_pIPAddress->SetFocus();
				}
				else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)))
				{
					if (m_pButton->IsWindowVisible())
						m_pButton->SetFocus();
				}
				else
        {
					//Pass on to the parent since we didn't handle it
  				CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
        }
			}
		}
		else
    {
	    //Pass on to the parent since we didn't handle it
		  CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }
	}
  else if (nChar == VK_UP)
  {
    //Explicitely select the item above
    int nIndex = GetSelectedItem();
    if (nIndex > 0)
    {
      SetItemState(nIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
      SetItemState(nIndex-1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

      //Destroy the old combo or edit box if need be
      if (m_nControlItem != -1)
      {
        UpdateListControlValueFromChildControl(m_nControlItem);
        DestroyOldChildControl();
        m_nControlItem = -1;
      }

      CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex-1));
	    if (pItemData && pItemData->m_pRuntimeClass1 && m_nControlItem == -1)
        CreateNewChildControl(nIndex-1);

      //Auto select the control if configured to do so
      if (m_bAutoSelect)
        PostMessage(WM_LOC_SETFOCUS_TO_CHILD);
    }
  }
  else if (nChar == VK_DOWN)
  {
    //Explicitely select the item above
    int nIndex = GetSelectedItem();
    if (nIndex < (GetItemCount()-1))
    {
      SetItemState(nIndex, 0, LVIS_SELECTED | LVIS_FOCUSED);
      SetItemState(nIndex+1, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

      //Destroy the old combo or edit box if need be
      if (m_nControlItem != -1)
      {
        UpdateListControlValueFromChildControl(m_nControlItem);
        DestroyOldChildControl();
        m_nControlItem = -1;
      }

      CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex+1));
	    if (pItemData && pItemData->m_pRuntimeClass1 && m_nControlItem == -1)
        CreateNewChildControl(nIndex+1);

      //Auto select the control if configured to do so
      if (m_bAutoSelect)
        PostMessage(WM_LOC_SETFOCUS_TO_CHILD);
    }
  }
	else
  {
	  //Pass on to the parent since we didn't handle it
	  CListCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

void CListOptionsCtrl::RequestChildReposition()
{  
  //Ask the list control to reposition the button if need be  
  PostMessage(WM_LOC_REPOSITION_CHILD_CONTROL);
}

void CListOptionsCtrl::SetSelected(int nIndex, BOOL bSelect)
{
  if (bSelect)
    SetItemState(nIndex, LVIS_SELECTED,	LVIS_SELECTED);
  else
    SetItemState(nIndex, 0,	LVIS_SELECTED);
}

BOOL CListOptionsCtrl::AddBoolean(int nIndex, DWORD_PTR dwItemData)
{
  return AddComboBox(nIndex, RUNTIME_CLASS(CListOptionsBooleanCombo), dwItemData);
}

BOOL CListOptionsCtrl::IsBoolean(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::ComboBox;  //We use a simple combo with 2 strings in it
                                                                 //So the validation we do is incomplete here
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsEditBox(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::EditBox;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsColorItem(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::ColorBrowser;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsFontItem(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::FontBrowser;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsFileItem(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::FileBrowser;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsFolderItem(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::FolderBrowser;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::IsOpaqueItem(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  if (pItemData)
    return pItemData->m_Type == CListOptionsItemData::OpaqueBrowser;
  else
    return FALSE;
}

BOOL CListOptionsCtrl::SetBoolean(int nIndex, BOOL bValue)
{
  //Validate our parameters
  ASSERT(IsBoolean(nIndex)); //Must be a check box to check it

  //Defer to the combo box method
  CString sText;
  if (bValue)  
    VERIFY(sText.LoadString(IDS_LISTOPTIONS_TRUE));
  else
    VERIFY(sText.LoadString(IDS_LISTOPTIONS_FALSE));
  SetComboText(nIndex, sText);

  return TRUE;
}

BOOL CListOptionsCtrl::GetBoolean(int nIndex, BOOL& bValue)
{
  //Validate our parameters
  ASSERT(IsBoolean(nIndex)); //Must be a combo item to check it

  CString sCompare;
  VERIFY(sCompare.LoadString(IDS_LISTOPTIONS_TRUE));
  if (GetComboText(nIndex) == sCompare)
    bValue = TRUE;
  else
    bValue = FALSE;
  
  return TRUE;
}

void CListOptionsCtrl::PreSubclassWindow() 
{
  //Let the parent class do its thing	
	CListCtrl::PreSubclassWindow();

  //Change the window style to be details
	LONG lWndStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	lWndStyle &= ~LVS_TYPEMASK;
	lWndStyle |= LVS_REPORT;
	SetWindowLong(m_hWnd, GWL_STYLE, lWndStyle);

  //Turn on full row select (makes the control look more like the VB equivalent)
  SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_FULLROWSELECT, LVS_EX_FULLROWSELECT);

  //Turn on grid lines
  SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, LVS_EX_GRIDLINES, LVS_EX_GRIDLINES);

  //Subclass the header control and hook it up to this object
  m_ctrlHeader.SubclassWindow(GetDlgItem(0)->m_hWnd);
  m_ctrlHeader.SetListBuddy(this);

  //Delete all the columns before we add our ones
  while (DeleteColumn(0));

  //Work out the width for each column
  CRect r;
  GetClientRect(&r);
  int nColumnWidth = r.Width()/2;

  //Also add two columns one for the attribute text
  //and one for the attribute value
  CString sColumn;
  VERIFY(sColumn.LoadString(IDS_LISTOPTIONS_ATTRIBUTE_TEXT));
  InsertColumn(0, sColumn, LVCFMT_LEFT, nColumnWidth);
  VERIFY(sColumn.LoadString(IDS_LISTOPTIONS_ATTRIBUTE_VALUE));
  InsertColumn(1, sColumn, LVCFMT_LEFT, nColumnWidth);

  //Also create 6 font's we use for customizing the text drawing for each item
  CFont* pFont = GetFont();
  LOGFONT lf;
  pFont->GetLogFont(&lf);

  LOGFONT lfCopy;
  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfWeight = FW_BOLD;
  VERIFY(m_BoldFont.CreateFontIndirect(&lfCopy));

  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfItalic = TRUE;
  VERIFY(m_ItalicFont.CreateFontIndirect(&lfCopy));

  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfUnderline = TRUE;
  VERIFY(m_UnderlineFont.CreateFontIndirect(&lfCopy));

  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfWeight = FW_BOLD;
  lfCopy.lfItalic = TRUE;
  VERIFY(m_ItalicBoldFont.CreateFontIndirect(&lfCopy));

  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfUnderline = TRUE;
  lfCopy.lfItalic = TRUE;
  VERIFY(m_ItalicUnderlineFont.CreateFontIndirect(&lfCopy));

  memcpy(&lfCopy, &lf, sizeof(lfCopy));
  lfCopy.lfWeight = FW_BOLD;
  lfCopy.lfUnderline = TRUE;
  lfCopy.lfItalic = TRUE;
  VERIFY(m_ItalicUnderlineBoldFont.CreateFontIndirect(&lfCopy));
}

BOOL CListOptionsCtrl::AddComboBox(int nIndex, CRuntimeClass* pRuntimeClass, DWORD_PTR dwItemData)
{
  //Validate our parameters
  ASSERT(pRuntimeClass);

  //Pointer to the runtime class is stored in the Item data  
  CListOptionsItemData* pItemData = new CListOptionsItemData;
  pItemData->m_dwItemData = dwItemData;
  pItemData->m_pRuntimeClass1 = pRuntimeClass;
  pItemData->m_Type = CListOptionsItemData::ComboBox;
  return SetItemData(nIndex, reinterpret_cast<DWORD_PTR>(pItemData));
}

CString CListOptionsCtrl::GetComboText(int nIndex)
{
  return GetItemText(nIndex, 1);
}

void CListOptionsCtrl::SetComboText(int nIndex, const CString& sComboText)
{
  SetItemText(nIndex, 1, sComboText);
}

void CListOptionsCtrl::DestroyOldChildControl()
{
  if (m_pCombo)
  {
    m_pCombo->DestroyWindow();
    delete m_pCombo;
    m_pCombo = NULL;
  }
  if (m_pEdit)
  {
    m_pEdit->DestroyWindow();
    delete m_pEdit;
    m_pEdit = NULL;
  }
  if (m_pDateTime)
  {
    m_pDateTime->DestroyWindow();
    delete m_pDateTime;
    m_pDateTime = NULL;
  }
  if (m_pIPAddress)
  {
    m_pIPAddress->DestroyWindow();
    delete m_pIPAddress;
    m_pIPAddress = NULL;
  }
  if (m_pSpin)
  {
    m_pSpin->DestroyWindow();
    delete m_pSpin;
    m_pSpin = NULL;
  }
  if (m_pButton)
  {
    m_pButton->DestroyWindow();
    delete m_pButton;
    m_pButton = NULL;
  }

  //Free up the font object we have been using  
  m_Font.DeleteObject();

  m_nControlItem = -1;
}

void CListOptionsCtrl::CreateNewChildControl(int nIndex)
{
  //Validate our parameters
  ASSERT(nIndex != -1);
  
  m_nControlItem = nIndex;

  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);

  //Make a copy of the current font being used by the control
  ASSERT(m_Font.m_hObject == NULL);
  CFont* pFont = GetFont();
  LOGFONT lf;
  pFont->GetLogFont(&lf);
  VERIFY(m_Font.CreateFontIndirect(&lf));

  //Allocate the main control
  AFXASSUME(pItemData->m_pRuntimeClass1);
  CString sComboText;
  CString sEditText;
  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsCombo)))
  {
    //Get the current text in the combo item
    sComboText = GetComboText(nIndex);

    //Create the new combo box
    m_pCombo = static_cast<CListOptionsCombo*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pCombo);
    ASSERT(m_pCombo->IsKindOf(RUNTIME_CLASS(CListOptionsCombo)));  //Your class must be derived from CListOptionsCombo
    m_pCombo->SetListBuddy(this);
    m_pCombo->SetListIndex(nIndex);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsEdit)))
  {
    //Get the current text in the edit box item
    sEditText = GetEditText(nIndex);

    //Create the new edit box
    m_pEdit = static_cast<CListOptionsEdit*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pEdit);
    ASSERT(m_pEdit->IsKindOf(RUNTIME_CLASS(CListOptionsEdit)));  //Your class must be derived from CListOptionsEdit
    m_pEdit->SetListBuddy(this);
    m_pEdit->SetListIndex(nIndex);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsDateCtrl)))
  {
    //Create the new date time control
    m_pDateTime = static_cast<CListOptionsDateCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pDateTime);
    ASSERT(m_pDateTime->IsKindOf(RUNTIME_CLASS(CListOptionsDateCtrl)));  //Your class must be derived from CListOptionsDateCtrl
    m_pDateTime->SetListBuddy(this);
    m_pDateTime->SetListIndex(nIndex);
    SYSTEMTIME st;
    GetDateTime(nIndex, st);
    m_pDateTime->SetDateTime(st);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsIPAddressCtrl)))
  {
    //Create the new IP Address control
    m_pIPAddress = static_cast<CListOptionsIPAddressCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pIPAddress);
    ASSERT(m_pIPAddress->IsKindOf(RUNTIME_CLASS(CListOptionsIPAddressCtrl)));  //Your class must be derived from CListOptionsIPAddressCtrl
    m_pIPAddress->SetListBuddy(this);
    m_pIPAddress->SetListIndex(nIndex);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)))
  {
    //Create the new static
    m_pButton = static_cast<CListOptionsBrowseButton*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pButton);
    ASSERT(m_pButton->IsKindOf(RUNTIME_CLASS(CListOptionsBrowseButton)));  //Your class must be derived from CListOptionsStatic
    m_pButton->SetListBuddy(this);
    m_pButton->SetListIndex(nIndex);

    if (pItemData->m_Type == CListOptionsItemData::ColorBrowser)
    {
      //Get the current color from the control and let the button know about it
      COLORREF color = GetColor(nIndex);
      m_pButton->SetColor(color);
    }
    else if (pItemData->m_Type == CListOptionsItemData::FontBrowser)
    {
      GetFontItem(nIndex, &lf);
      m_pButton->SetFontItem(&lf);
    }
    else 
    {
      ASSERT(pItemData->m_Type == CListOptionsItemData::OpaqueBrowser);
    }
  }
  else
    ASSERT(FALSE); //Your class must be derived from one of the classes in the previous statements

  //allocate the secondary control
  if (pItemData->m_pRuntimeClass2)
  {
    if (pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CListOptionsSpinCtrl)))
    {
      m_pSpin = static_cast<CListOptionsSpinCtrl*>(pItemData->m_pRuntimeClass2->CreateObject());
      ASSERT(m_pSpin);
      ASSERT(m_pSpin->IsKindOf(RUNTIME_CLASS(CListOptionsSpinCtrl)));  //Your class must be derived from CListOptionsSpinCtrl
      m_pSpin->SetListBuddy(this);
      m_pSpin->SetListIndex(nIndex);
    }
    else
    {
      ASSERT(pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)));
      m_pButton = static_cast<CListOptionsBrowseButton*>(pItemData->m_pRuntimeClass2->CreateObject());
      ASSERT(m_pButton);
      ASSERT(m_pButton->IsKindOf(RUNTIME_CLASS(CListOptionsBrowseButton)));  //Your class must be derived from CListOptionsBrowseButton
      m_pButton->SetListBuddy(this);
      m_pButton->SetListIndex(nIndex);
    }
  }

  //Update the rects for item
  CRect rLine;
  GetItemRect(nIndex, &rLine, LVIR_BOUNDS);

  //Get the width of the 2nd column
  HD_ITEM hdItem;
  hdItem.mask = HDI_WIDTH;
  m_ctrlHeader.GetItem(1, &hdItem);

  CRect r;
  r.top = rLine.top;
  r.left = rLine.right - hdItem.cxy;

  //Now create the main control
  ASSERT(pItemData->m_pRuntimeClass1);
  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsCombo)))
  {
    if (m_pButton)
      r.right = rLine.right - m_pButton->GetWidth();
    else
      r.right = rLine.right;
    r.bottom = r.top + m_pCombo->GetDropDownHeight(); //Ask the combo box for the height to use
    m_pCombo->Create(m_pCombo->GetWindowStyle(), r, this, LIST_OPTIONS_COMBOBOX_ID);
    ASSERT(m_pCombo->GetCount()); //You forget to add string items to the combo box in your OnCreate message handler!

    //set the font the combo box should use based on the font in the list control, 
    m_pCombo->SetFont(&m_Font);

    //Also select the right text into the combo box
    DWORD dwComboStyle = m_pCombo->GetStyle();
    BOOL bComboHasEdit = (((dwComboStyle & CBS_DROPDOWN) | (dwComboStyle & CBS_SIMPLE)) != 0);
    if ((dwComboStyle & CBS_DROPDOWNLIST) == CBS_DROPDOWNLIST)
      bComboHasEdit = FALSE;
    if (bComboHasEdit)
      m_pCombo->SetWindowText(sComboText);
    else
      m_pCombo->SelectString(-1, sComboText);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pCombo->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsEdit)))
  {
    if (m_pButton)
      r.right = rLine.right - m_pButton->GetWidth();
    else
      r.right = rLine.right;
    r.bottom = r.top + m_pEdit->GetHeight(rLine.Height());
    VERIFY(m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), sEditText, m_pEdit->GetWindowStyle(), r, this, LIST_OPTIONS_EDITBOX_ID));

    //set the font the edit box should use based on the font in the list control
    m_pEdit->SetFont(&m_Font);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pEdit->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsDateCtrl)))
  {
    r.right = rLine.right;
    r.bottom = rLine.bottom;
    VERIFY(m_pDateTime->Create(m_pDateTime->GetWindowStyle(), r, this, LIST_OPTIONS_DATETIMECTRL_ID));

    //set the font the date time control should use based on the font in the list control
    m_pDateTime->SetFont(&m_Font);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pDateTime->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsIPAddressCtrl)))
  {
    r.right = rLine.right;
    r.bottom = rLine.bottom;

    VERIFY(m_pIPAddress->Create(m_pIPAddress->GetWindowStyle(), r, this, LIST_OPTIONS_IPADDRESSCTRL_ID));

    //set the font the IP Address control should use based on the font in the list control
    m_pIPAddress->SetFont(&m_Font);

    DWORD dwAddress = GetIPAddress(nIndex);
    m_pIPAddress->SetAddress(dwAddress);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pIPAddress->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)))
  {
    CreateBrowseButton(pItemData->m_pRuntimeClass1, rLine, hdItem.cxy);
  }
  else
    ASSERT(FALSE); //Your class must be derived from one of the classes in the statements above

  //Actually create the secondary control
  if (pItemData->m_pRuntimeClass2)
  {
    if (pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CListOptionsSpinCtrl)))
      CreateSpinCtrl(pItemData->m_pRuntimeClass2, rLine, r);
    else
    {
      ASSERT(pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)));
      CreateBrowseButton(pItemData->m_pRuntimeClass2, rLine, hdItem.cxy);

      if (m_pEdit)
        m_pEdit->SetButtonBuddy(m_pButton);
      else if (m_pCombo)
        m_pCombo->SetButtonBuddy(m_pButton);
    }
  }
}

void CListOptionsCtrl::UpdateListControlValueFromChildControl(int nIndex)
{
  if (m_pCombo)
  {
    CString sText;
    m_pCombo->GetWindowText(sText);
    SetComboText(m_nControlItem, sText);
    m_pCombo->OnShowNewValue();
  }
  else if (m_pEdit)
  {
    CString sText;
    m_pEdit->GetWindowText(sText);
    SetEditText(m_nControlItem, sText);
    m_pEdit->OnShowNewValue();
  }
  else if (m_pDateTime)
  {
    SYSTEMTIME st1;
    if (m_pDateTime->GetTime(&st1) == GDT_VALID)
      m_pDateTime->SetDateTime(st1);

    SYSTEMTIME st2;
    m_pDateTime->GetDateTime(st2);
    SetDateTime(m_nControlItem, st2);
    SetEditText(m_nControlItem, m_pDateTime->GetDisplayText(st2));
    m_pDateTime->OnShowNewValue();
  }
  else if (m_pIPAddress)
  {
    DWORD dwAddress;
    if (m_pIPAddress->GetAddress(dwAddress) == 4)
    {
      SetIPAddress(m_nControlItem, dwAddress);
      SetEditText(m_nControlItem, m_pIPAddress->GetDisplayText(dwAddress));
      m_pIPAddress->OnShowNewValue();
    }
  }
  else if (m_pButton)
  {
    CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
    AFXASSUME(pItemData);
    if (pItemData->m_Type == CListOptionsItemData::ColorBrowser)
    {
      COLORREF color = m_pButton->GetColor();
      SetColor(m_nControlItem, color);
      m_pButton->OnShowNewValue();
    }
    else if (pItemData->m_Type == CListOptionsItemData::FontBrowser)
    {
      LOGFONT lf;
      GetFontItem(nIndex, &lf);
      m_pButton->SetFontItem(&lf);
      SetFontItem(m_nControlItem, &lf);
      m_pButton->OnShowNewValue();
    }
  }
}

BOOL CListOptionsCtrl::AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, DWORD_PTR dwItemData)
{
  //Just call the combo box version as currently there is no difference
  BOOL bSuccess = AddComboBox(nIndex, pRuntimeClassEditCtrl, dwItemData);

  //Update the type in the item data
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Type = CListOptionsItemData::EditBox;

  return bSuccess;
}

BOOL CListOptionsCtrl::AddEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassSpinCtrl, DWORD_PTR dwItemData)
{
  //Add the edit box
  BOOL bSuccess = AddEditBox(nIndex, pRuntimeClassEditCtrl, dwItemData);

  //Add the spin ctrl
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  ASSERT(pRuntimeClassSpinCtrl);
  pItemData->m_pRuntimeClass2 = pRuntimeClassSpinCtrl;
  pItemData->m_Type = CListOptionsItemData::Spin;
  pItemData->m_dwItemData = dwItemData;

  return bSuccess;
}

BOOL CListOptionsCtrl::AddFileEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Add the edit box
  BOOL bSuccess = AddEditBox(nIndex, pRuntimeClassEditCtrl, dwItemData);

  //Add the browse button
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  ASSERT(pRuntimeClassButton);
  pItemData->m_pRuntimeClass2 = pRuntimeClassButton;
  pItemData->m_Type = CListOptionsItemData::FileBrowser;

  return bSuccess;
}

BOOL CListOptionsCtrl::AddColorSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData, BOOL bDrawColorForIcon)
{
  //Add the browse button as the primary control
  BOOL bSuccess = AddEditBox(nIndex, pRuntimeClassButton, dwItemData);

  //Setup the browser type
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Type = CListOptionsItemData::ColorBrowser;
  pItemData->m_bDrawColorForIcon = bDrawColorForIcon ? true : false;
  
  return bSuccess;
}

COLORREF CListOptionsCtrl::GetColor(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  return pItemData->m_Color;
}

void CListOptionsCtrl::SetColor(int nIndex, COLORREF color)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Color = color;

  //Also update the text while we are at it
  CString sColor;
  sColor.Format(_T("&%02x%02x%02x"), GetRValue(color), GetGValue(color), GetBValue(color));
  sColor.MakeUpper();
  SetEditText(nIndex, sColor);
}

BOOL CListOptionsCtrl::AddDateTime(int nIndex, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData)
{
  //Add the date time control as the primary control
  BOOL bSuccess = AddComboBox(nIndex, pRuntimeClassDateTime, dwItemData);

  //Setup the item type
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Type = CListOptionsItemData::DateTimeCtrl;
  
  return bSuccess;
}

void CListOptionsCtrl::GetDateTime(int nIndex, SYSTEMTIME& st)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  memcpy(&st, &pItemData->m_DateTime, sizeof(SYSTEMTIME));
}

void CListOptionsCtrl::SetDateTime(int nIndex, const SYSTEMTIME& st)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  memcpy(&pItemData->m_DateTime, &st, sizeof(SYSTEMTIME));

  //Also update the text while we are at it
  CListOptionsDateCtrl* pTempDateTime = static_cast<CListOptionsDateCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
  AFXASSUME(pTempDateTime);
  ASSERT(pTempDateTime->IsKindOf(RUNTIME_CLASS(CListOptionsDateCtrl)));  //Your class must be derived from CListOptionsDateCtrl
  CString sDateTime(pTempDateTime->GetDisplayText(st));
  SetEditText(nIndex, sDateTime);
  delete pTempDateTime;
}

BOOL CListOptionsCtrl::AddIPAddress(int nIndex, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData)
{
  //Add the date time control as the primary control
  BOOL bSuccess = AddComboBox(nIndex, pRuntimeClassDateTime, dwItemData);

  //Setup the item type
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Type = CListOptionsItemData::IPAddressCtrl;
  
  return bSuccess;
}

DWORD CListOptionsCtrl::GetIPAddress(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  return pItemData->m_dwIPAddress;
}

void CListOptionsCtrl::SetIPAddress(int nIndex, DWORD dwAddress)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_dwIPAddress = dwAddress;

  //Also update the text while we are at it
  CListOptionsIPAddressCtrl* pTempIPAddress = static_cast<CListOptionsIPAddressCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
  AFXASSUME(pTempIPAddress);
  ASSERT(pTempIPAddress->IsKindOf(RUNTIME_CLASS(CListOptionsIPAddressCtrl)));  //Your class must be derived from CListOptionsIPAddressCtrl
  CString sIPAddress(pTempIPAddress->GetDisplayText(dwAddress));
  SetEditText(nIndex, sIPAddress);
  delete pTempIPAddress;
}

BOOL CListOptionsCtrl::AddOpaque(int nIndex, CRuntimeClass* pRuntimeClass1, CRuntimeClass* pRuntimeClass2, DWORD_PTR dwItemData)
{
  //Add the first class
  BOOL bSuccess = AddEditBox(nIndex, pRuntimeClass1, dwItemData);

  //Add the second class
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  pItemData->m_pRuntimeClass2 = pRuntimeClass2;

  //Setup the browser type
  pItemData->m_Type = CListOptionsItemData::OpaqueBrowser;
  
  return bSuccess;
}

DWORD_PTR CListOptionsCtrl::GetOpaque(int nIndex)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  return pItemData->m_dwItemData;
}

void CListOptionsCtrl::SetOpaque(int nIndex, DWORD_PTR dwItemData)
{
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_dwItemData = dwItemData;
}

BOOL CListOptionsCtrl::AddFontSelector(int nIndex, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Add the browse button as the primary control
  BOOL bSuccess = AddEditBox(nIndex, pRuntimeClassButton, dwItemData);

  //Setup the browser type
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  pItemData->m_Type = CListOptionsItemData::FontBrowser;
  
  return bSuccess;
}

void CListOptionsCtrl::GetFontItem(int nIndex, LOGFONT* pLogFont)
{
  //Validate our parameters
  ASSERT(pLogFont);
  
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  memcpy(pLogFont, &pItemData->m_Font, sizeof(LOGFONT));
}

void CListOptionsCtrl::SetFontItem(int nIndex, const LOGFONT* pLogFont)
{
  //Validate our parameters
  AFXASSUME(pLogFont);
  
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  AFXASSUME(pItemData);
  memcpy(&pItemData->m_Font, pLogFont, sizeof(LOGFONT));

  //Also update the text while we are at it
  SetEditText(nIndex, pLogFont->lfFaceName);
}

CString CListOptionsCtrl::GetFileEditText(int nIndex)
{
  //Just call the edit box version as currently there is no difference
  return GetEditText(nIndex);
}

void CListOptionsCtrl::SetFileEditText(int nIndex, const CString& sEditText)
{
  //Just call the edit box version as currently there is no difference
  SetEditText(nIndex, sEditText);
}

BOOL CListOptionsCtrl::AddFolderEditBox(int nIndex, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Let the File edit box code do all the work
  BOOL bSuccess = AddFileEditBox(nIndex, pRuntimeClassEditCtrl, pRuntimeClassButton, dwItemData);

  //Setup the correct edit type in the item data
  CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nIndex));
  pItemData->m_Type = CListOptionsItemData::FolderBrowser;

  return bSuccess;
}

CString CListOptionsCtrl::GetFolderEditText(int nIndex)
{
  //Just call the edit box version as currently there is no difference
  return GetEditText(nIndex);
}

void CListOptionsCtrl::SetFolderEditText(int nIndex, const CString& sEditText)
{
  //Just call the edit box version as currently there is no difference
  SetEditText(nIndex, sEditText);
}

void CListOptionsCtrl::CreateSpinCtrl(CRuntimeClass* pRuntimeClassSpinCtrl, CRect rItem, CRect rPrimaryControl)
{
  //Validate our parameters
  AFXASSUME(pRuntimeClassSpinCtrl);
  
  if (pRuntimeClassSpinCtrl->IsDerivedFrom(RUNTIME_CLASS(CListOptionsSpinCtrl)))
  {
    //work out the rect for this spin control
    CRect r;
    r.top = rPrimaryControl.top;
    r.bottom = rPrimaryControl.bottom;
    r.left = rPrimaryControl.right;
    r.right = rItem.right;

    //Create the new spin control
    AFXASSUME(m_pSpin);
    m_pSpin->SetEditBuddy(m_pEdit);
    
    //Create the spin control
    VERIFY(m_pSpin->Create(m_pSpin->GetWindowStyle(), r, this, LIST_OPTIONS_SPINCTRL_ID));

    //Setup the buddy and the default range
    m_pSpin->SetBuddy(m_pEdit);
    int nLower = 0;
    int nUpper = 0;
    m_pSpin->GetDefaultRange(nLower, nUpper);
    m_pSpin->SetRange32(nLower, nUpper); 

    //set the font the edit box should use based on the font in the list control
    m_pSpin->SetFont(&m_Font);
  }
  else
    ASSERT(FALSE); //Your class must be derived from CListOptionsSpinCtrl
}

void CListOptionsCtrl::CreateBrowseButton(CRuntimeClass* pRuntimeClassBrowseButton, CRect rItem, int /*nColumnWidth*/)
{
  //Validate our parameters
  AFXASSUME(pRuntimeClassBrowseButton);
  
  if (pRuntimeClassBrowseButton->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)))
  {
    if (m_pEdit)
    {
      //work out the rect for the button
      CRect rEdit;
      m_pEdit->GetWindowRect(&rEdit);
      ScreenToClient(&rEdit);
      CRect r;
      r.top = rItem.top;
      r.bottom = rEdit.bottom;
      r.right = rItem.right;
      r.left = r.right - m_pButton->GetWidth(); //Ask the browse button for the width to use

      //Create the new browse button
      ASSERT(m_pButton);
      m_pButton->SetEditBuddy(m_pEdit);
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, LIST_OPTIONS_BROWSEBUTTONCTRL_ID));
      m_pButton->SetOwner(m_pEdit);
    }
    else if (m_pCombo)
    {
      //work out the rect for the button
      CRect rCombo;
      m_pCombo->GetWindowRect(&rCombo);
      ScreenToClient(&rCombo);
      CRect r;
      r.top = rItem.top;
      r.bottom = rCombo.bottom;
      r.right = rItem.right;
      r.left = r.right - m_pButton->GetWidth(); //Ask the browse button for the width to use

      //Create the new browse button
      ASSERT(m_pButton);
      m_pButton->SetComboBuddy(m_pCombo);
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, LIST_OPTIONS_BROWSEBUTTONCTRL_ID));
      m_pButton->SetOwner(m_pCombo);
    }
    else
    {
      //work out the rect for the button
      CRect r;
      r.top = rItem.top;
      r.bottom = rItem.bottom;
      r.right = rItem.right;
      r.left = r.right - m_pButton->GetWidth(); //Ask the browse button for the width to use

      //Create the browse button
      ASSERT(m_pButton);
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, LIST_OPTIONS_BROWSEBUTTONCTRL_ID));
    }

    //set the font the edit box should use based on the font in the list control
    m_pButton->SetFont(&m_Font);
  }
  else
    ASSERT(FALSE); //Your class must be derived from CListOptionsBrowseButton
}

CString CListOptionsCtrl::GetEditText(int nIndex)
{
  //Just call the combo box version as currently there is no difference
  return GetComboText(nIndex);
}

void CListOptionsCtrl::SetEditText(int nIndex, const CString& sEditText)
{
  //Just call the combo box version as currently there is no difference
  SetComboText(nIndex, sEditText);
}

void CListOptionsCtrl::OnDestroy() 
{
  //Destroy the old combo or edit box if need be
  DestroyOldChildControl();

  //Let the parent class do its thing
	CListCtrl::OnDestroy();
}

void CListOptionsCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if (!(pScrollBar && pScrollBar->IsKindOf(RUNTIME_CLASS(CListOptionsSpinCtrl))))
  {
    //Clean up any controls currently open we used (assuming it is a standard
    //scroll message and not from one of our spin controls)
    if (m_nControlItem != -1)
    {
      UpdateListControlValueFromChildControl(m_nControlItem);
      DestroyOldChildControl();
    }

    //Let the parent class do its thing
	  CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
  }
}

void CListOptionsCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  //Clean up any controls currently open we used
  if (m_nControlItem != -1)
  {
    UpdateListControlValueFromChildControl(m_nControlItem);
    DestroyOldChildControl();
  }
	
  //Let the parent class do its thing
	CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CListOptionsCtrl::OnClick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  //Get the current mouse location and convert it to client
  //coordinates.
  DWORD pos = GetMessagePos();
  CPoint pt(static_cast<short>(LOWORD(pos)), static_cast<short>(HIWORD(pos)));
  ScreenToClient(&pt);

  //Get indexes of the first and last visible items in listview
  //control.
  int nIndex = GetTopIndex();
  int nLastVisibleIndex = nIndex + GetCountPerPage();
  int nItems = GetItemCount();
  if (nLastVisibleIndex > nItems)
    nLastVisibleIndex = nItems;

  //Loop until number visible items has been reached.
  int nFoundIndex = -1;
  while (nIndex <= nLastVisibleIndex)
  {
    //Get the bounding rectangle of an item. If the mouse
    //location is within the bounding rectangle of the item,
    //you know you have found the item that was being clicked.
    CRect r;
    GetItemRect(nIndex, &r, LVIR_BOUNDS);
    if (r.PtInRect(pt))
    {
      nFoundIndex = nIndex;
      SetItemState(nIndex, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
      break;
    }

    // Get the next item in listview control.
    nIndex++;
  }


  //If the mouse was over the label or icon and the item is a combo box
  //or edit box and editing is currently not active then create the 
  //new control
  if (nFoundIndex != -1)
  {
    //Destroy the old combo or edit box if need be
    if (m_nControlItem != -1)
    {
      UpdateListControlValueFromChildControl(m_nControlItem);
      DestroyOldChildControl();
      m_nControlItem = -1;
    }

    CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nFoundIndex));
	  if (pItemData && pItemData->m_pRuntimeClass1 && m_nControlItem == -1)
      CreateNewChildControl(nFoundIndex);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      PostMessage(WM_LOC_SETFOCUS_TO_CHILD);
  }
	
	*pResult = 0;

  return FALSE;
}

int CListOptionsCtrl::GetSelectedItem()
{
  for (int i=0; i<GetItemCount(); i++)
  {
    if (GetItemState(i, LVIS_SELECTED) & LVIS_SELECTED)
      return i;
  }

  return -1;
}

void CListOptionsCtrl::OnCBNKillFocus()
{
  HandleLoseFocusLogic(GetFocus());
}

void CListOptionsCtrl::HandleLoseFocusLogic(CWnd* pNewWnd)
{
  //Clean up any controls currently open if we are losing focus to something else
  BOOL bForeignWnd = ((m_nControlItem != -1) && (pNewWnd != m_pCombo) && (pNewWnd != m_pEdit) &&
                      (pNewWnd != m_pButton) && (pNewWnd != m_pDateTime) && (pNewWnd != m_pIPAddress));
  if (bForeignWnd && m_pCombo)
    bForeignWnd = !m_pCombo->IsRelatedWnd(pNewWnd);
  if (bForeignWnd && m_pDateTime)
    bForeignWnd = !m_pDateTime->IsRelatedWnd(pNewWnd);
  if (bForeignWnd && m_pIPAddress)
    bForeignWnd = !m_pIPAddress->IsRelatedWnd(pNewWnd);

  if (bForeignWnd)
  {
    int nSelItem = GetSelectedItem();
    if (nSelItem != -1)
    {
      UpdateListControlValueFromChildControl(GetSelectedItem());
      DestroyOldChildControl();
    }
  }
}

void CListOptionsCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //Call the helper function which does the main work
  HandleLoseFocusLogic(pNewWnd);

  //Let the parent class do its thing
	CListCtrl::OnKillFocus(pNewWnd);
}

void CListOptionsCtrl::HandleChildControlLosingFocus()
{
  //Clean up any controls currently open we used
  //if we are losing focus to something else
  UpdateListControlValueFromChildControl(GetSelectedItem());
  DestroyOldChildControl();
}

BOOL CListOptionsCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMLVCUSTOMDRAW* pCustomDraw = reinterpret_cast<NMLVCUSTOMDRAW*>(pNMHDR);
  switch (pCustomDraw->nmcd.dwDrawStage) 
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW; //Tell the control that we are interested in item notifications
      break;
		}	
    case CDDS_ITEMPREPAINT:
    {
      //We want sub item notifications
      *pResult = CDRF_NOTIFYSUBITEMDRAW;

      break;
    }
    case CDDS_SUBITEM | CDDS_ITEMPREPAINT:
    {
      int nItem = static_cast<int>(pCustomDraw->nmcd.dwItemSpec);
      CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nItem));
      if (pItemData)
      {
        if (pCustomDraw->iSubItem == 0)
        {
          BOOL bUpdatedFont = TRUE;

          //Select the right font
          if (pItemData->m_bPromptBoldText && pItemData->m_bPromptItalicText && pItemData->m_bPromptUnderlineText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicUnderlineBoldFont);
          else if (pItemData->m_bPromptBoldText && pItemData->m_bPromptItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicBoldFont);
          else if (pItemData->m_bPromptUnderlineText && pItemData->m_bPromptItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicUnderlineFont);
          else if (pItemData->m_bPromptUnderlineText)
            SelectObject(pCustomDraw->nmcd.hdc, m_UnderlineFont);
          else if (pItemData->m_bPromptBoldText)
            SelectObject(pCustomDraw->nmcd.hdc, m_BoldFont);
          else if (pItemData->m_bPromptItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicFont);
          else
            bUpdatedFont = FALSE;

          //And color
          pCustomDraw->clrText = pItemData->m_PromptTextColor;
          pCustomDraw->clrTextBk = pItemData->m_PromptBackgroundColor;

          *pResult = bUpdatedFont ? CDRF_NEWFONT : CDRF_DODEFAULT;
        }
        else if (pCustomDraw->iSubItem == 1)
        {
          BOOL bUpdatedFont = TRUE;

          //Select the right font
          if (pItemData->m_bValueBoldText && pItemData->m_bValueItalicText && pItemData->m_bValueUnderlineText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicUnderlineBoldFont);
          else if (pItemData->m_bValueBoldText && pItemData->m_bValueItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicBoldFont);
          else if (pItemData->m_bValueUnderlineText && pItemData->m_bValueItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicUnderlineFont);
          else if (pItemData->m_bValueUnderlineText)
            SelectObject(pCustomDraw->nmcd.hdc, m_UnderlineFont);
          else if (pItemData->m_bValueBoldText)
            SelectObject(pCustomDraw->nmcd.hdc, m_BoldFont);
          else if (pItemData->m_bValueItalicText)
            SelectObject(pCustomDraw->nmcd.hdc, m_ItalicFont);
          else
            bUpdatedFont = FALSE;

          //And color
          pCustomDraw->clrText = pItemData->m_ValueTextColor;
          pCustomDraw->clrTextBk = pItemData->m_ValueBackgroundColor;

          *pResult = bUpdatedFont ? CDRF_NEWFONT : CDRF_DODEFAULT;
        }
        else
          *pResult = CDRF_DODEFAULT;
      }
      else
        *pResult = CDRF_DODEFAULT;
      break;
    }
    case CDDS_ITEMPOSTPAINT:
    {
      int nItem = static_cast<int>(pCustomDraw->nmcd.dwItemSpec);

      BOOL bDrawColor = FALSE;
      CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(GetItemData(nItem));
      if (pItemData && pItemData->m_Type == CListOptionsItemData::ColorBrowser && pItemData->m_bDrawColorForIcon)
        bDrawColor = TRUE;

      if (bDrawColor)
      {  
        //Draw the icon of the tree view item using the specified bitmap
        CDC dc;
        dc.Attach(pCustomDraw->nmcd.hdc);

        //First work out the position of the icon
				CRect rIconRect;
				GetItemRect(nItem, rIconRect, LVIR_ICON);

        //Draw it using the IL
        dc.FillSolidRect(&rIconRect, pItemData->m_Color);

        //Release the DC
        dc.Detach();
      }

      *pResult = CDRF_DODEFAULT;
      break;
    }
    default:
    {
      break;
    }
  }
  
  return TRUE; //Allow the message to be reflected again
}


IMPLEMENT_DYNCREATE(CListOptionsComboEdit, CEdit)

CListOptionsComboEdit::CListOptionsComboEdit() : m_pListCtrl(NULL)
{
}

BEGIN_MESSAGE_MAP(CListOptionsComboEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CListOptionsComboEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pListCtrl);
    if (m_pListCtrl->m_pButton)
      m_pListCtrl->m_pButton->SetFocus();  
    else
		  m_pListCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

UINT CListOptionsComboEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB;
}


IMPLEMENT_DYNCREATE(CListOptionsCombo, CComboBox)

CListOptionsCombo::CListOptionsCombo() : m_pListCtrl(NULL),
                                         m_nListCtrlIndex(-1),
                                         m_pButtonCtrl(NULL),
                                         m_bDoNotDestroyUponLoseFocus(FALSE)
{
}

BEGIN_MESSAGE_MAP(CListOptionsCombo, CComboBox)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_CTLCOLOREDIT, OnCtlColor)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

UINT CListOptionsCombo::OnGetDlgCode() 
{
	return CComboBox::OnGetDlgCode() | DLGC_WANTTAB;
}

void CListOptionsCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pListCtrl);
    if (m_pListCtrl->m_pButton)
      m_pListCtrl->m_pButton->SetFocus();  
    else
		  m_pListCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CListOptionsCombo::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;
}

int CListOptionsCombo::GetDropDownHeight()
{
  return 100;
}

BOOL CListOptionsCombo::IsRelatedWnd(CWnd* pChild)
{
  BOOL bAncestor = FALSE;
  if (pChild)
  {
    CWnd* pWnd = pChild;
    while (pWnd && !bAncestor)
    {
      pWnd = pWnd->GetParent();
      bAncestor = (pWnd == this);
    }
  }

  return bAncestor;
}

void CListOptionsCombo::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  ASSERT(m_pListCtrl);
	if ((pNewWnd != m_pButtonCtrl) && !IsRelatedWnd(pNewWnd))
  {
    ASSERT(m_pListCtrl);
    m_pListCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CComboBox::OnKillFocus(pNewWnd);
	}
}

LRESULT CListOptionsCombo::OnCtlColor(WPARAM /*wParam*/, LPARAM lParam)
{
  //Subclass the combo edit
  if (m_Edit.GetSafeHwnd() == NULL)
    m_Edit.SubclassWindow(reinterpret_cast<HWND>(lParam));
  
  //Let the base class do its thing
  return Default();
}

void CListOptionsCombo::OnDestroy()
{
  if (m_Edit.GetSafeHwnd() != NULL)
    m_Edit.UnsubclassWindow();

  //Let the base class do its thing
  CComboBox::OnDestroy();
}		


IMPLEMENT_DYNCREATE(CListOptionsFontNameCombo, CListOptionsCombo)

BEGIN_MESSAGE_MAP(CListOptionsFontNameCombo, CListOptionsCombo)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CListOptionsFontNameCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  //Let the parent class do its thing
	if (CListOptionsCombo::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  //Enumerate all the fonts
  CDC* pDC = GetDC();
  if (pDC)
  {
    EnumFonts(pDC->m_hDC, NULL, _EnumFontProc, reinterpret_cast<LPARAM>(this));
    ReleaseDC(pDC);
  }
	
	return 0;
}

int CListOptionsFontNameCombo::EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* /*lptm*/,
                                            DWORD /*dwType*/)
{
  //Add the font name to the combo box
  AddString(lplf->lfFaceName);

  return 1; //To continue enumeration
}

int CALLBACK CListOptionsFontNameCombo::_EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm,
                                                      DWORD dwType, LPARAM lpData)
{ 
  //Convert from the SDK world to the C++ world
  CListOptionsFontNameCombo* pThis = reinterpret_cast<CListOptionsFontNameCombo*>(lpData);
  AFXASSUME(pThis);
  return pThis->EnumFontProc(lplf, lptm, dwType);
} 

DWORD CListOptionsFontNameCombo::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT;
}


IMPLEMENT_DYNCREATE(CListOptionsBooleanCombo, CListOptionsCombo)

BEGIN_MESSAGE_MAP(CListOptionsBooleanCombo, CListOptionsCombo)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CListOptionsBooleanCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  //Let the parent class do its thing
	if (CListOptionsCombo::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  //Add the two boolean strings
  CString sText;
  VERIFY(sText.LoadString(IDS_LISTOPTIONS_TRUE));
  AddString(sText);
  VERIFY(sText.LoadString(IDS_LISTOPTIONS_FALSE));
  AddString(sText);
	
	return 0;
}


IMPLEMENT_DYNCREATE(CListOptionsEdit, CEdit)

CListOptionsEdit::CListOptionsEdit() : m_pListCtrl(NULL),
                                       m_bDoNotDestroyUponLoseFocus(FALSE),
                                       m_pButtonCtrl(NULL),
                                       m_nListCtrlIndex(-1)
{
}

BEGIN_MESSAGE_MAP(CListOptionsEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CListOptionsEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB;
}

void CListOptionsEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pListCtrl);
    if (m_pListCtrl->m_pButton)
      m_pListCtrl->m_pButton->SetFocus();  
    else
		  m_pListCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CListOptionsEdit::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL;
}

int CListOptionsEdit::GetHeight(int nItemHeight)
{
  return max(nItemHeight, 20);
}

void CListOptionsEdit::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  ASSERT(m_pListCtrl);
	if (pNewWnd != m_pButtonCtrl)
  {
    AFXASSUME(m_pListCtrl);
    m_pListCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CEdit::OnKillFocus(pNewWnd);
	}
}

CString CListOptionsEdit::GetBrowseForFolderCaption()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_LISTOPTIONS_DEFAULT_FOLDER_CAPTION));
  return sCaption;
}

CString CListOptionsEdit::GetBrowseForFileCaption()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_LISTOPTIONS_DEFAULT_FILE_CAPTION));
  return sCaption;
}

CString CListOptionsEdit::GetFileExtensionFilter()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_LISTOPTIONS_DEFAULT_FILE_EXTENSION_FILTER));
  return sCaption;
}

int CALLBACK CListOptionsEdit::SHBrowseSetSelProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
  if (uMsg == BFFM_INITIALIZED)
    ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);

  return 0;
}

void CListOptionsEdit::BrowseForFolder(const CString& sInitialFolder)
{
  //validate our parameters
  AFXASSUME(m_pListCtrl);

  //Bring up a standard directory chooser dialog
  TCHAR sDisplayName[_MAX_PATH];
  BROWSEINFO bi;
  bi.hwndOwner = m_pListCtrl->GetSafeHwnd();
  bi.pidlRoot = NULL;
  CString sCaption(GetBrowseForFolderCaption());
  bi.lpszTitle = sCaption;
  bi.pszDisplayName = sDisplayName;
  bi.ulFlags = BIF_RETURNONLYFSDIRS;
  bi.lpfn = SHBrowseSetSelProc;
  bi.lParam = reinterpret_cast<LPARAM>(sInitialFolder.operator LPCTSTR());
	LPITEMIDLIST pItemIDList = SHBrowseForFolder(&bi);

  if (pItemIDList)
  {
    //Retreive the path and update on screen
    TCHAR sPath[_MAX_PATH];
    if (SHGetPathFromIDList(pItemIDList, sPath))
      SetWindowText(sPath);

    //avoid memory leaks by deleting the PIDL
    //using the shells task allocator
    IMalloc* pMalloc;
    if (SUCCEEDED(SHGetMalloc(&pMalloc)))
    {
      pMalloc->Free(pItemIDList);
      pMalloc->Release();
    }
  }
}

void CListOptionsEdit::BrowseForFile(const CString& sInitialFile)
{
  //validate our paramters
  ASSERT(m_pListCtrl);

  //Create the special file save dialog
  CListOptionsFileDialog dlg(TRUE, NULL, sInitialFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetFileExtensionFilter(), m_pListCtrl);

  //Modify the title to the desired value
  CString sCaption(GetBrowseForFileCaption());
  dlg.m_ofn.lpstrTitle = sCaption;
  
  //bring up the dialog and if hit ok set the text in this control to the new filename
  if (dlg.DoModal() == IDOK)
    SetWindowText(dlg.GetPathName());
}


IMPLEMENT_DYNCREATE(CListOptionsSpinCtrl, CSpinButtonCtrl)

CListOptionsSpinCtrl::CListOptionsSpinCtrl() : m_pListCtrl(NULL),
                                               m_pEdit(NULL),
                                               m_nListCtrlIndex(-1)
{
}

BEGIN_MESSAGE_MAP(CListOptionsSpinCtrl, CSpinButtonCtrl)
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

DWORD CListOptionsSpinCtrl::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT;
}

void CListOptionsSpinCtrl::GetDefaultRange(int &lower, int& upper)
{
  lower = 0;
  upper = 100;
}

void CListOptionsSpinCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  AFXASSUME(m_pListCtrl);
	if (pNewWnd != m_pEdit)
    m_pListCtrl->HandleChildControlLosingFocus();
  else
  {
    //Let the parent class do its thing
	  CSpinButtonCtrl::OnKillFocus(pNewWnd);
	}
}


IMPLEMENT_DYNCREATE(CListOptionsBrowseButton, CButton)

CListOptionsBrowseButton::CListOptionsBrowseButton() : m_pListCtrl(NULL),
                                                       m_pEdit(NULL),
                                                       m_nListCtrlIndex(-1),
                                                       m_pCombo(NULL),
                                                       m_Color(RGB(0, 0, 0))
{
  memset(&m_Font, 0, sizeof(m_Font));
}

BEGIN_MESSAGE_MAP(CListOptionsBrowseButton, CButton)
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()

DWORD CListOptionsBrowseButton::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
}

CString CListOptionsBrowseButton::GetCaption()
{
  return _T("...");
}

int CListOptionsBrowseButton::GetWidth()
{
  AFXASSUME(m_pListCtrl);

  CDC* pDC = m_pListCtrl->GetDC();
  int nButtonWidth = 0;
	if (pDC)
  {
    //Get the button width
		CSize SizeText = pDC->GetTextExtent(_T("    "), 4); //We add space around text button
		pDC->LPtoDP(&SizeText);
		nButtonWidth = SizeText.cx;
    CString sText(GetCaption());
		SizeText = pDC->GetTextExtent(sText, sText.GetLength());
		pDC->LPtoDP(&SizeText);
		nButtonWidth += SizeText.cx;

    m_pListCtrl->ReleaseDC(pDC);
	}
	else
    nButtonWidth = 20;

  return nButtonWidth;
}

void CListOptionsBrowseButton::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  AFXASSUME(m_pListCtrl);
  if (m_pEdit)
  {
	  if ((pNewWnd != m_pListCtrl->m_pEdit) && !m_pEdit->m_bDoNotDestroyUponLoseFocus)
    {
      ASSERT(m_pListCtrl);
      m_pListCtrl->HandleChildControlLosingFocus();
    }
    else
    {
      //Let the parent class do its thing
	    CButton::OnKillFocus(pNewWnd);
	  }
  }
  else if (m_pCombo)
  {
	  if ((pNewWnd != m_pListCtrl->m_pCombo) && !m_pCombo->m_bDoNotDestroyUponLoseFocus)
    {
      ASSERT(m_pListCtrl);
      m_pListCtrl->HandleChildControlLosingFocus();
    }
    else
    {
      //Let the parent class do its thing
	    CButton::OnKillFocus(pNewWnd);
	  }
  }
  else
  {
    //Let the parent class do its thing
    CButton::OnKillFocus(pNewWnd);
  }
}

void CListOptionsBrowseButton::OnClicked() 
{
  //Validate our paramters
  AFXASSUME(m_pListCtrl);

	//Get the text currently in the control
  if (m_nListCtrlIndex != -1)
  {
    //Pull out the item data associated with the selected item
    CListOptionsItemData* pItemData = reinterpret_cast<CListOptionsItemData*>(m_pListCtrl->GetItemData(m_nListCtrlIndex));

    if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsEdit)))
    {
      AFXASSUME(m_pEdit);

      //Decide on what dialog to bring up, and call the appropiate virtual function
      if (pItemData->m_Type == CListOptionsItemData::FileBrowser)
      {
        m_pEdit->m_bDoNotDestroyUponLoseFocus = TRUE;
        CString sText;
        m_pEdit->GetWindowText(sText);
        m_pEdit->BrowseForFile(sText);
        m_pEdit->m_bDoNotDestroyUponLoseFocus = FALSE;
      }
      else if (pItemData->m_Type == CListOptionsItemData::FolderBrowser)
      {
        m_pEdit->m_bDoNotDestroyUponLoseFocus = TRUE;
        CString sText;
        m_pEdit->GetWindowText(sText);
        m_pEdit->BrowseForFolder(sText);
        m_pEdit->m_bDoNotDestroyUponLoseFocus = FALSE;
      }
      else if (pItemData->m_Type == CListOptionsItemData::OpaqueBrowser)
        BrowseForOpaque();
      else
        ASSERT(FALSE);
    }
    else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsCombo)))
    {
      ASSERT(m_pCombo);
      
      if (pItemData->m_Type == CListOptionsItemData::OpaqueBrowser)
        BrowseForOpaque();
    }
    else
    {
      ASSERT(pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CListOptionsBrowseButton)));

      if (pItemData->m_Type == CListOptionsItemData::ColorBrowser)
        BrowseForColor();
      else if (pItemData->m_Type == CListOptionsItemData::FontBrowser)
        BrowseForFont();
      else if (pItemData->m_Type == CListOptionsItemData::OpaqueBrowser)
        BrowseForOpaque();
      else
        ASSERT(FALSE);
    }
  }
}

void CListOptionsBrowseButton::BrowseForColor()
{
  //Validate our parameters
  ASSERT(m_pListCtrl);

  //Bring up a standard color selector dialog
  CColorDialog dialog(GetColor());
  dialog.m_cc.Flags |= CC_FULLOPEN;
  if (dialog.DoModal() == IDOK)
  {
    SetColor(dialog.GetColor());
    m_pListCtrl->SetColor(m_nListCtrlIndex, m_Color);

    //Ask the list control to reposition the button if need be  
    m_pListCtrl->RequestChildReposition();
  }
}

void CListOptionsBrowseButton::BrowseForFont()
{
  //Validate our parameters
  AFXASSUME(m_pListCtrl);

  //Bring up a standard color selector dialog
  CFontDialog dialog(&m_Font);
  if (dialog.DoModal() == IDOK)
  {
    dialog.GetCurrentFont(&m_Font);
    m_pListCtrl->SetFontItem(m_nListCtrlIndex, &m_Font);

    //Ask the list control to reposition the button if need be  
    m_pListCtrl->RequestChildReposition();
  }
}

void CListOptionsBrowseButton::BrowseForOpaque()
{
  ASSERT(FALSE); //Derived classes must implement this function if we are editing
                 //an opaque item. The code which "normally" display some CDialog 
                 //derived class to allow the item to be edited and then hive the
                 //data away somehow so that it can show the new value when the
                 //dialog is brought up again. Following is some pseudo code which 
                 //would do this.

  /*
  ASSERT(m_pListCtrl);

  //Bring up a our custom opaque editor dialog
  CMyOpaque* pQpaque = reinterpret_cast<CMyOpaque*>(m_pListCtrl->GetOpaque(m_nListCtrlIndex));
  CMyOpaqueDialog dialog;
  dialog.SetOpaque(pOpaque);
  if (dialog.DoModal() == IDOK)
  {
    pOpaque = dialog.GetOpaque();
    m_pListCtrl->SetOpaque(m_nListCtrlIndex, reinterpret_cast<DWORD_PTR>(pOpaque));
    m_pListCtrl->SetItemText(m_nListCtrlIndex, 1, pOpaque->m_sSomeName);

    //Ask the list control to reposition the button if need be  
    m_pListCtrl->RequestChildReposition();
  }
  */
}

void CListOptionsBrowseButton::SetColor(COLORREF color) 
{ 
  m_Color = color; 
}

void CListOptionsBrowseButton::GetFontItem(LOGFONT* pLogFont)
{
  //Validate our parameters
  ASSERT(pLogFont);
  
  memcpy(pLogFont, &m_Font, sizeof(LOGFONT));
}

void CListOptionsBrowseButton::SetFontItem(const LOGFONT* pLogFont)
{
  ASSERT(pLogFont);
  memcpy(&m_Font, pLogFont, sizeof(LOGFONT));
}


IMPLEMENT_DYNAMIC(CListOptionsFileDialog, CFileDialog)

CListOptionsFileDialog::CListOptionsFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, 
                                       CWnd* pParentWnd) :	CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

BEGIN_MESSAGE_MAP(CListOptionsFileDialog, CFileDialog)
END_MESSAGE_MAP()

void CListOptionsFileDialog::OnInitDone()
{
	CString sText;
  if (!sText.LoadString(IDS_LISTOPTIONS_OK))
    ASSERT(FALSE);
  LPTSTR pszBuffer = sText.GetBuffer(sText.GetLength());

  //modify the text on the IDOK button to ok
  CommDlg_OpenSave_SetControlText(GetParent()->m_hWnd, IDOK, pszBuffer);

  sText.ReleaseBuffer();
}


IMPLEMENT_DYNCREATE(CListOptionsDateCtrl, CDateTimeCtrl)

CListOptionsDateCtrl::CListOptionsDateCtrl() : m_pListCtrl(NULL),
                                               m_nListCtrlIndex(-1),
                                               m_bDoNotDestroyUponLoseFocus(FALSE)
{
}

BEGIN_MESSAGE_MAP(CListOptionsDateCtrl, CDateTimeCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CListOptionsDateCtrl::OnGetDlgCode() 
{
	return CDateTimeCtrl::OnGetDlgCode() | DLGC_WANTTAB;
}

void CListOptionsDateCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pListCtrl);
		m_pListCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CListOptionsDateCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | DTS_SHORTDATEFORMAT;
}

void CListOptionsDateCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  if (!IsRelatedWnd(pNewWnd))
  {
    ASSERT(m_pListCtrl);
    m_pListCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CDateTimeCtrl::OnKillFocus(pNewWnd);
	}
}

CString CListOptionsDateCtrl::GetDisplayText(const SYSTEMTIME& st)
{
  //What will be the return value from this function
  CString sDisplayText;

  //First ask the API to give us the buffer size
  int nChars = ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, NULL, 0);
  BOOL bWantStop = FALSE;
  while (nChars && !bWantStop)
  {
    LPTSTR pszBuffer = sDisplayText.GetBuffer(nChars);
    nChars = ::GetDateFormat(LOCALE_USER_DEFAULT, DATE_SHORTDATE, &st, NULL, pszBuffer, nChars);
    sDisplayText.ReleaseBuffer();
    if (nChars == 0)
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        //Expand the buffer, and loop around again
        nChars *= 2;
      }
      else
        bWantStop = TRUE;
    }
    else
      bWantStop = TRUE;
  }

  return sDisplayText;
}

BOOL CListOptionsDateCtrl::IsRelatedWnd(CWnd* pChild)
{
  return (GetMonthCalCtrl() == pChild);
}


IMPLEMENT_DYNCREATE(CListOptionsTimeCtrl, CListOptionsDateCtrl)

BEGIN_MESSAGE_MAP(CListOptionsTimeCtrl, CListOptionsDateCtrl)
END_MESSAGE_MAP()

CString CListOptionsTimeCtrl::GetDisplayText(const SYSTEMTIME& st)
{
  //What will be the return value from this function
  CString sDisplayText;

  //First ask the API to give us the buffer size
  int nChars = ::GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
  BOOL bWantStop = FALSE;
  while (nChars && !bWantStop)
  {
    LPTSTR pszBuffer = sDisplayText.GetBuffer(nChars);
    nChars = ::GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, pszBuffer, nChars);
    sDisplayText.ReleaseBuffer();
    if (nChars == 0)
    {
      if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
      {
        //Expand the buffer, and loop around again
        nChars *= 2;
      }
      else
        bWantStop = TRUE;
    }
    else
      bWantStop = TRUE;
  }

  return sDisplayText;
}

DWORD CListOptionsTimeCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | DTS_TIMEFORMAT;
}


IMPLEMENT_DYNCREATE(CListOptionsIPAddressCtrl, CIPAddressCtrl)

CListOptionsIPAddressCtrl::CListOptionsIPAddressCtrl() : m_pListCtrl(NULL),
                                                         m_nListCtrlIndex(-1),
                                                         m_bDoNotDestroyUponLoseFocus(FALSE),
                                                         m_dwAddress(0)
{
}

BEGIN_MESSAGE_MAP(CListOptionsIPAddressCtrl, CIPAddressCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CListOptionsIPAddressCtrl::OnGetDlgCode() 
{
	return CIPAddressCtrl::OnGetDlgCode() | DLGC_WANTTAB;
}

void CListOptionsIPAddressCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pListCtrl);
		m_pListCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CIPAddressCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CListOptionsIPAddressCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE;
}

void CListOptionsIPAddressCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  if (!IsRelatedWnd(pNewWnd))
  {
    //update the list control and close this window
    ASSERT(m_pListCtrl);
    m_pListCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CIPAddressCtrl::OnKillFocus(pNewWnd);
	}
}

CString CListOptionsIPAddressCtrl::GetDisplayText(DWORD dwAddress)
{
  CString sAddress;
  sAddress.Format(_T("%d.%d.%d.%d"), (dwAddress & 0xFF000000) >> 24, 
                  (dwAddress & 0xFF0000) >> 16, (dwAddress & 0xFF00) >> 8,
                  (dwAddress & 0xFF));
  return sAddress;
}

BOOL CListOptionsIPAddressCtrl::IsRelatedWnd(CWnd* pChild)
{
  BOOL bAncestor = FALSE;
  if (pChild)
  {
    CWnd* pWnd = pChild;
    while (pWnd && !bAncestor)
    {
      pWnd = pWnd->GetParent();
      bAncestor = (pWnd == this);
    }
  }

  return bAncestor;
}


BEGIN_MESSAGE_MAP(CListOptionsHeaderCtrl, CHeaderCtrl)
  ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKA, OnBeginTrack)
  ON_NOTIFY_REFLECT_EX(HDN_BEGINTRACKW, OnBeginTrack)
END_MESSAGE_MAP()

CListOptionsHeaderCtrl::CListOptionsHeaderCtrl() : m_pListCtrl(NULL)
{
}

BOOL CListOptionsHeaderCtrl::OnBeginTrack(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  //Clean up any controls currently open we used
  AFXASSUME(m_pListCtrl);
  if (m_pListCtrl->m_nControlItem != -1)
  {
    m_pListCtrl->UpdateListControlValueFromChildControl(m_pListCtrl->m_nControlItem);
    m_pListCtrl->DestroyOldChildControl();
  }
	
	*pResult = 0;

  return FALSE;
}


void DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlListOptions->GetEditText(nIndex);
  else
    pCtrlListOptions->SetEditText(nIndex, sText);
}

void DDX_ListEdit(CDataExchange* pDX, int nIDC, int nIndex, int& nValue)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
	AFXASSUME(pCtrlListOptions);
	ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));
	
	CString sText;
	if (pDX->m_bSaveAndValidate)
	{
		sText = pCtrlListOptions->GetEditText(nIndex);
		nValue = _ttoi(sText);
	}
	else
	{
		sText.Format(_T("%d"), nValue);
		pCtrlListOptions->SetEditText(nIndex, sText);
	}
}

void DDX_ListCombo(CDataExchange* pDX, int nIDC, int nIndex, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlListOptions->GetComboText(nIndex);
  else
    pCtrlListOptions->SetComboText(nIndex, sText);
}

void DDX_ListFileEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlListOptions->GetFileEditText(nIndex);
  else
    pCtrlListOptions->SetFileEditText(nIndex, sText);
}

void DDX_ListFolderEdit(CDataExchange* pDX, int nIDC, int nIndex, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlListOptions->GetFolderEditText(nIndex);
  else
    pCtrlListOptions->SetFolderEditText(nIndex, sText);
}

void DDX_ListColor(CDataExchange* pDX, int nIDC, int nIndex, COLORREF& color)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    color = pCtrlListOptions->GetColor(nIndex);
  else
    pCtrlListOptions->SetColor(nIndex, color);
}

void DDX_ListFont(CDataExchange* pDX, int nIDC, int nIndex, LOGFONT* pLogFont)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlListOptions->GetFontItem(nIndex, pLogFont);
  else
    pCtrlListOptions->SetFontItem(nIndex, pLogFont);
}

void DDX_ListBoolean(CDataExchange* pDX, int nIDC, int nIndex, BOOL& bValue)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlListOptions->GetBoolean(nIndex, bValue);
  else
    pCtrlListOptions->SetBoolean(nIndex, bValue);
}

void DDX_ListDateTime(CDataExchange* pDX, int nIDC, int nIndex, SYSTEMTIME& st)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlListOptions->GetDateTime(nIndex, st);
  else
    pCtrlListOptions->SetDateTime(nIndex, st);
}

void DDX_ListIPAddress(CDataExchange* pDX, int nIDC, int nIndex, DWORD& dwAddress)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CListOptionsCtrl* pCtrlListOptions = static_cast<CListOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlListOptions);
  ASSERT(pCtrlListOptions->IsKindOf(RUNTIME_CLASS(CListOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    dwAddress = pCtrlListOptions->GetIPAddress(nIndex);
  else
    pCtrlListOptions->SetIPAddress(nIndex, dwAddress);
}
