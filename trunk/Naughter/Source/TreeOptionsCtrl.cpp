/*
Module : TreeOptionsCtrl.cpp
Purpose: Implementation for an MFC class to implement a tree options control 
         similiar to the advanced tab as seen on the "Internet options" dialog in 
         Internet Explorer 4 and later

Created: PJN / 31-03-1999
History: PJN / 21-04-1999 Added full support for enabling / disabling all the item types
         PJN / 05-10-1999 Made class more self contained by internally managing the image list
         PJN / 07-10-1999 1. Added support for including combo boxes aswell as edit boxes into the
                          edit control.
                          2. Added support for customizing the image list to use
         PJN / 29-02-2000 Removed a VC 6 level 4 warning
         PJN / 03-04-2000 Added support for navigation into and out of the combo boxes and edit controls
                          inside the control
         PJN / 10-05-2000 1. Fixed a bug where the text was not being transferred to the control when the 
                          in inplace combo or edit box is active and the tree control gets destroyed.
                          2. Added support for having check box items as children of other check box items
                          3. Setting the check box state of a parent now also sets the check box state 
                          for all child box children.
                          4. Setting the check box state afects the check box state of the parent if that
                          parent is also a check box.
         PJN / 30-05-2000 Code now uses ON_NOTIFY_REFLECT_EX instead of ON_NOTIFY_REFLECT. This allows 
                          derived classes to handle the reflected messages also.
         PJN / 03-07-2000 Now includes support for edit boxes with accompanying spin controls
         PJN / 25-04-2001 1. Creation of the image list is now virtual. This allows customisation such as
                          being able to use high color bitmaps
                          2. Added an option which determines if check box state should be changed when you 
                          click anywhere on an item or just over the checkbox icon
                          3. Updated copyright message
         PJN / 12-08-2001 1. Fixed an issue in GetComboText and GetEditText where the code was modifying the 
                          contents of the combo/edit box when it was being read. This was because that 
                          function is doing double duty as it is called when the child control is about to be 
                          created in place, and you want to remove the text from the tree control and put it 
                          in the child control. Thanks to "Jef" for spotting this.
                          2. Made the code in SetComboText more robust. Thanks to "Jef" for this also.
                          3. Added a DDX method for integers in a edit box. Thanks to Colin Urquhart for this.
                          4. Added an extra member to CTreeOptionsItemData to be used as an item data. This
                          allows you to avoid having to implement multiple derived classes and instead use
                          the item data's I now provide to stash away pointers etc.
         PJN / 27-08-2001 1. Provided a "GetUserItemData" member to provide access to the item data provided 
                          by the class. Thanks to Colin Urquhart for this.
                          2. Fixed a redraw problem which occured when you scrolled using an wheel enabled 
                          mouse. Thanks to "Jef" for spotting this.
                          3. Provided an AutoSelect option which automatically sets focus to child control 
                          in the tree control. Thanks to "Jef" for this suggestion.
                          4. Added full support for CBS_DROPDOWN and CBS_SIMPLE style combo boxes. Thanks
                          to "Jef" for this suggestion.
         PJN / 16-08-2001 1. Provided support for specify a color via CColorDialog.
                          2. Controls are now created to fill the full width of the tree control
                          3. Provided support for specifying a font via CFontDialog
                          4. Provided support for specifying a font name from a combo box
                          5. Provided support for specifying a boolean value from a combo box
         PJN / 27-11-2001 1. Fixed a bug where the message map for OnMouseWheel was setup incorrectly. It 
                          should have been ON_WM_MOUSEWHEEL instead of ON_MESSAGE!!!.
                          2. Allowed passing in the hAfter item for the InsertItem calls. All parms are defaulted 
                          as to not affect any current code.
                          3. Made possible the use of radio button groups followed by other items (in which case 
                          the group is considered complete).
                          4. Added a couple of utility functions at the bottom of the cpp file. Thanks to Mike
                          Funduc for all these updates.
         PJN / 05-11-2001 1. Minor code tidy up following development of the author's CTreeOptionsCtrl class
         PJN / 13-12-2001 1. Fixed an assertion in OnClick. Thanks to "flipposwitch" for spotting the problem
         PJN / 14-02-2002 1. Now allows item data to be associated with any item added to the control
                          2. Fixed issue with return value from GetUserItemData
         PJN / 02-06-2002 1. Moved sample app to VC 6 to facilitate support for IP Address control and date and
                          time controls.
                          2. Fixed a bug where the child controls can get orphaned when a node in the tree 
                          control is expaned or contracted. Thanks to Lauri Ott for spotting this problem.
                          3. Now fully supports the CDateTimeCtrl for editing of dates and times
                          4. Now fully supports the CIPAddressCtrl for editing of IP addresses
                          5. Custom draw support for color browser items is now configurable via an additional
                          parameter of the AddColorBrowser method
         PJN / 24-09-2002 1. Updated documentation which incorrectly stated that the parent of a check box item 
                          must be a group item as inserted with InsertGroup. Thanks to Kögl Christoph for 
                          spotting this.
                          2. Fixed an issue with "IMPLEMENT_DYNAMIC(CDateTimeCtrl..." not being declared propertly. 
                          Some users reported that it worked ok, while others said that my fix was causing link 
                          problems. The problem should be sorted out for good now. Thanks to Kögl Christoph for 
                          reporting this.
                          3. Renamed the SetImageListToUse function to "SetImageListResourceIDToUse".
                          4. Provided a GetImageListResourceIDToUse function to match up with the Set function.
                          5. Provided a method to allow the user item data to be changed after an item has 
                          been created.
                          6. Provided some documentation info how how to safely use item data in the control. 
                          Thanks to Kögl Christoph for reporting this.
                          7. Fixed a potential memory leak in AddComboBox if the function is invoked twice without 
                          an explicit delete of the item first. Thanks to Kögl Christoph for reporting this.
                          8. Fixed sometypos in the documentation. It incorectly stated that the return type of 
                          member functions InsertGroup, InsertCheckBox, and InsertRadioButton is BOOL when in fact 
                          it is HTREEITEM.  Thanks to Kögl Christoph for reporting this.
                          9. Improved the look of the disabled checked check button. Thanks to Kögl Christoph for 
                          reporting this.
                          10. Improved the look of the disabled radio button which is selected. Thanks to Kögl 
                          Christoph for reporting this.
         PJN / 17-10-2002 1. Added a method to add an "Opaque Browser" to the Tree options control. An
                          Opaque Browser is where the tree options control allows a end user specified
                          structure to be edited by the tree options control without it explicitly
                          knowing what it is editing. 
         PJN / 25-10-2002 1. Updated the download to include the missing files OpaqueShow.cpp/h. Thanks to Kögl 
                          Christoph for reporting this.
                          2. Made the class more const-correct. e.g. the Get... member functions are now const. 
                          Thanks to Kögl Christoph for reporting this. Also updated the documentation for this.
                          3. Updated the documentation to refer to the "Opaque Browser" support.
         PJN / 28-10-2002 1. Fixed a bug where upon a combo losing focus it will also result in the associated
                          button control would also be destroyed. Thanks to Kögl Christoph for reporting this
                          problem. Fixed this bug should also fix an intermittent release bug which was occurring
                          in this area.
         PJN / 30-10-2002 1. Made a number of other methods const. Thanks to Kögl Christoph for reporting this.
         PJN / 15-11-2002 1. Now allows the Field Data separator i.e. ": " to be configured. Please note that the
                          characters you pick should be avoided in the descriptive text you display for an item 
                          as it is used as the divider between the descriptive text and the actual data to be 
                          edited. Thanks to Kögl Christoph for this update.
                          2. Fixed an access violation in CTreeOptionsCtrl::OnSelchanged when there is no selected
                          item in the control. Thanks to Kögl Christoph for this update.
         PJN / 06-03-2003 1. Fixed a memory leak which can occur when the control is used in a property sheet. 
                          Thanks to David Rainey for reporting this problem.
                          2. Fixed another memory leak in the destructor of the CTreeOptionsCtrl class when the
                          test app is closed
         PJN / 14-05-2003 1. Fixed a bug where the OnSelChanged function was getting in the way when the control was 
                          being cleared down, leading to an ASSERT. Thanks to Chen Fu for reporting this problem.
         PJN / 07-06-2003 1. Fixed a bug where the date time control was not reflecting the changes when the child
                          control was displayed. Thanks to Tom Serface for reporting this problem.
         PJN / 17-07-2003 1. Made SetRadioButton methods in CTreeOptionsCtrl virtual to allow further client 
                          customisation.
         PJN / 26-07-2003 1. Fix an issue where the child controls get drawn in the wrong place, if the item is just
                          on the vertical (either top or bottom) edge of the client area. Thanks to Kögl Christoph
                          for reporting this problem and providing the fix.
         PJN / 31-07-2003 1. Now class does not require the use of the built in image list. This means that now the class
                          behaves like its parent class CTreeCtrl in that management of the image list is entirely
                          outside of the class and client code calls SetImageList as per normal.
                          2. Class now draws the radio and check box icons using custom draw.
                          3. Class now takes advantage of XP Visual Themes when available.
                          4. Made a number of additional functions virtual to allow more types of client customisation
         PJN / 02-08-2003 1. Now supports multi-line edit controls as a child control
         PJN / 18-09-2003 1. Fixed a bug when the control draws the custom draw icons when the control does not have
                          an image list attached. Thanks to Benoit Lariviere for reporting this problem.
                          2. Fixed a bug where the control draws the custom draw icons in the wrong position when
                          the control has horizontal scroll bars. Thanks to Benoit Lariviere for reporting this problem.
         PJN / 11-12-2003 1. Implemented the function SetUseThemes which was missing. Thanks to Marcel Scherpenisse 
                          for reporting this problem. Also updated the sample app to enable toggling of this setting. 
                          2. Improved the default look of radio buttons and check boxes when the control is not
                          XP themed. This is achieved using the virtual function GetItemIconPosition. Thanks to Marcel 
                          Scherpenisse for reporting this issue. This update also means that by default radio buttons
                          and check boxes are by default draw as 16*16 icons centred in the icon position for each 
                          item. This can of course by overridden by derived classes.
         PJN / 05-02-2004 1. Made HandleChildControlLosingFocus public so that it can be called from outside the class. This
                          is useful in the following scenario reported by RnySmile: I noticed a bug happened only if it is 
                          used in a Property Page. I added an Edit item, and when user leaves that item with a child 
                          control active while tabbing away to another tab, the data of that item will lost. The solution 
                          is to ensure HandleChildControlLosingFocus is called before a DDX_Tree... function is called
                          if a child control is active.
         PJN / 05-05-2004 1. Fixed some compiler warnings when the code is compiled using VC 7.x. Please
                          note that to implement these fixes, the code will now require the Platform SDK
                          to be installed if you compile the code on VC 6.
         PJN / 26-01-2005 1. Both of the sample apps shipped with the class now include XP manifest files, meaning that the
                          full look and feel of XP Themes are shown. Thanks to Piotr Zagawa for reporting this issue.
         PJN / 16-03-2005 1. Fix for crash when you have one of the edit controls in a IP address control selected and 
                          you then select another item in the tree options control. Hard to know if this is a bug in the
                          control on in Windows itself. Either way, the workaround fix is to ensure that the child windows
                          of an IP Address control are destroyed before we destroy the IP address control itself. Thanks
                          to Steve Hayes for reporting this problem.
         PJN / 11-07-2006 1.  Updated copyright details
                          2.  Removed unused constructor and destructor methods from some of the classes in the sample app.
                          3.  Replaced calls to ZeroMemory with memset
                          4.  Addition of CTREEOPTIONSCTRL_EXT_CLASS macro to allow the code to be easily added to an 
                          extension dll.
                          5.  Class now uses private messages based at WM_USER instead of WM_APP
                          6.  Optimized CTreeOptionsCtrl constructor code.
                          7.  Optimized CTreeOptionsCombo constructor code.
                          8.  Optimized CTreeOptionsDateCtrl constructor code 
                          9.  Reworked CTreeOptionsDateCtrl::GetDisplayText logic
                          10. Reworked CTreeOptionsTimeCtrl::GetDisplayText logic
                          11. Optimized CTreeOptionsIPAddressCtrl constructor code
                          12. Optimized CTreeOptionsEdit constructor code
                          13. Optimized CTreeOptionsSpinCtrl constructor code
                          14. Optimized CTreeOptionsBrowseButton constructor code
                          15. Replaced calls to CopyMemory with memcpy
                          16. Optimized CMultiLineEditFrameWnd constructor code
                          17. Removed common control 6 manifest as this conflicts with VC 2005 compilation.
                          18. Updated the code to clean compile on VC 2005.
                          19. Updated documentation to use the same style as the web site.
         PJN / 23-12-2007 1. Updated copyright details.
                          2. Removed VC 6 style AppWizard comments from the code.
                          3. Optimized CTreeOptionsItemData constructor code
                          4. Boolean member variables of CTreeOptionsItemData have now been made "bool" instead of "BOOL".
                          5. Fixed an issue where if you create a combo box with a style of CBS_DROPDOWN instead of 
                          CBS_DROPDOWNLIST, then the combo box would not be properly deactivated when it loses focus to a 
                          control outside of the TreeOptionsCtrl while the cursor is in the combo box edit field. Thanks
                          to Tobias Wolf for reporting this issue.
                          6. Fixed a crash where you select a combo box item, select something in it and then hit tab.
                          7. Focus is now correctly transferred to a tree options button if you hit tab on a tree options
                          combo box.
                          8. GetBrowseForFolderCaption, GetBrowseForFileCaption and GetFileExtensionFilter all now by 
                          default use a string resource.
         PJN / 21-06-2008 1. Updated copyright details
                          2. Code now compiles cleanly using Code Analysis (/analyze)
                          3. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          4. The code now only supports VC 2005 or later. 
                          5. Replaced all calls to CopyMemory with memcpy
                          6. Fixed a bug in the CTreeOptionsCtrl constructor when checking the validity of the function
                          pointers for XP Theming.
                          7. Fixed a bug in the sample app's COpaqueShoeButton2::BrowseForOpaque function
         PJN / 20-07-2008 1. Added two missing files to the download zip file specifically the correct sln and vcproj 
                          files. Thanks to Ingmar Koecher for reporting this issue.
                          2. Radio buttons and check boxes now by default use a more 3d style of drawing when XP theming
                          is not active. You can revert back to a more flat style of drawing these items by 
                          SetFlatStyleRadioButtons(TRUE) and SetFlatStyleCheckBoxes(TRUE) respectively. Thanks to 
                          Ingmar Koecher for reporting this issue.
         PJN / 29-04-2012 1. Updated copyright details.
                          2. Updated sample project settings to more modern defaults
                          3. Code now compiles cleanly using Code Analysis (/analyze)
                          4. When the parent node of a group of checkboxes is also a checkbox (e.g. the Node "Security" 
                          in the test app) the parent is checked / unchecked depending on the status of its children 
                          whenever a child is modified in the GUI. This previously worked ok when you changed the 
                          checkmark of a child in the GUI by mouse or keyboard, but it doesn't work if the change is 
                          made programmatically (via SetCheckBox or by means of the referenced boolean variable). The 
                          code has now been updated to handle this anomaly. Thanks to Michael Oerder for providing this
                          nice update.

Copyright (c) 1999 - 2012 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

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
#include "TreeOptionsCtrlRes.h"
#ifndef _SHLOBJ_H_
#pragma message("To avoid this message, please put shlobj.h in your pre compiled header (normally stdafx.h)")
#include <shlobj.h>
#endif
#include "TreeOptionsCtrl.h"
#ifndef TMSCHEMA_H
#pragma message("To avoid this message, please put tmschema.h in your pre compiled header (normally stdafx.h)")
#include <tmschema.h>
#endif


//////////////// Macros / Locals /////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

const UINT TREE_OPTIONS_COMBOBOX_ID = 100;
const UINT TREE_OPTIONS_EDITBOX_ID  = 101;
const UINT TREE_OPTIONS_SPINCTRL_ID = 102;
const UINT TREE_OPTIONS_BROWSEBUTTONCTRL_ID = 103;
const UINT TREE_OPTIONS_STATIC_ID = 104;
const UINT TREE_OPTIONS_DATETIMECTRL_ID  = 105;
const UINT TREE_OPTIONS_IPADDRESSCTRL_ID  = 106;

#define WM_TOC_SETFOCUS_TO_CHILD WM_USER + 1
#define WM_TOC_REPOSITION_CHILD_CONTROL WM_USER + 2


//////////////// Implementation //////////////////////////////////////

IMPLEMENT_DYNAMIC(CTreeOptionsCtrl, CTreeCtrl)

BEGIN_MESSAGE_MAP(CTreeOptionsCtrl, CTreeCtrl)
	ON_WM_LBUTTONDOWN()
	ON_WM_CHAR()
	ON_WM_DESTROY()
	ON_WM_VSCROLL()
	ON_WM_HSCROLL()
	ON_WM_KEYDOWN()
	ON_WM_KILLFOCUS()
	ON_WM_LBUTTONDOWN()
  ON_WM_MOUSEWHEEL()
  ON_MESSAGE(WM_TOC_SETFOCUS_TO_CHILD, OnSetFocusToChild)
  ON_MESSAGE(WM_TOC_REPOSITION_CHILD_CONTROL, OnRepositionChild)
	ON_NOTIFY_REFLECT_EX(NM_CLICK, OnClick)
	ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelchanged)
  ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT_EX(TVN_DELETEITEM, OnDeleteItem)
  ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, OnCustomDraw)
  ON_CBN_KILLFOCUS(TREE_OPTIONS_COMBOBOX_ID, OnCBNKillFocus)
END_MESSAGE_MAP()

CTreeOptionsCtrl::CTreeOptionsCtrl() : m_pCombo(NULL),
                                       m_pEdit(NULL),
                                       m_pMultiLineEditFrameWnd(NULL),
                                       m_pSpin(NULL),
                                       m_pButton(NULL),
                                       m_pDateTime(NULL),
                                       m_pIPAddress(NULL),
                                       m_hControlItem(NULL),
                                       m_bToggleOverIconOnly(FALSE),
                                       m_bAutoSelect(FALSE),
                                       m_sSeparator(_T(": ")),
                                       m_bDoNotDestroy(FALSE),
                                       m_bThemed(TRUE),
                                       m_bFlatStyleRadioButtons(FALSE),
                                       m_bFlatStyleCheckBoxes(FALSE)
{
  //Dynamically pull in the uxtheme functions
  m_hUXTheme = LoadLibrary(_T("UxTheme.dll"));
  if (m_hUXTheme)
  {
    m_lpfnOpenThemeData = reinterpret_cast<LPOPENTHEMEDATA>(GetProcAddress(m_hUXTheme, "OpenThemeData"));
    m_lpfnDrawThemeBackground = reinterpret_cast<LPDRAWTHEMEBACKGROUND>(GetProcAddress(m_hUXTheme, "DrawThemeBackground"));
    m_lpfnDrawThemeEdge = reinterpret_cast<LPDRAWTHEMEEDGE>(GetProcAddress(m_hUXTheme, "DrawThemeEdge"));
    m_lpfnCloseThemeData = reinterpret_cast<LPCLOSETHEMEDATA>(GetProcAddress(m_hUXTheme, "CloseThemeData"));

    if ((m_lpfnOpenThemeData == NULL) || (m_lpfnDrawThemeBackground == NULL) || 
        (m_lpfnDrawThemeEdge == NULL) || (m_lpfnCloseThemeData == NULL))
    {
      m_lpfnOpenThemeData = NULL;
      m_lpfnDrawThemeBackground = NULL;
      m_lpfnDrawThemeEdge = NULL;
      m_lpfnCloseThemeData = NULL;

      FreeLibrary(m_hUXTheme);
      m_hUXTheme = NULL;
    }
  }
  else
  {
    m_lpfnOpenThemeData = NULL;
    m_lpfnDrawThemeBackground = NULL;
    m_lpfnDrawThemeEdge = NULL;
    m_lpfnCloseThemeData = NULL;
  }
}

CTreeOptionsCtrl::~CTreeOptionsCtrl()
{
  DestroyOldChildControl();

  ASSERT(m_pCombo == NULL);
  ASSERT(m_pEdit == NULL);
  ASSERT(m_pMultiLineEditFrameWnd == NULL);
  ASSERT(m_pSpin == NULL);
  ASSERT(m_pButton == NULL);
  ASSERT(m_pDateTime == NULL);
  ASSERT(m_pIPAddress == NULL);

  if (m_hUXTheme)
  {
    FreeLibrary(m_hUXTheme);
    m_hUXTheme = NULL;
  }
}

LRESULT CTreeOptionsCtrl::OnSetFocusToChild(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  if (m_pCombo)
    m_pCombo->SetFocus();
  else if (m_pMultiLineEditFrameWnd)
    m_pMultiLineEditFrameWnd->SetFocus();
  else if (m_pEdit)
    m_pEdit->SetFocus();
  else if (m_pDateTime)
    m_pDateTime->SetFocus();
  else if (m_pIPAddress)
    m_pIPAddress->SetFocus();
  
  return 0L;  
}

LRESULT CTreeOptionsCtrl::OnRepositionChild(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  HTREEITEM hItem = GetSelectedItem();
  if (hItem)
  {
    UpdateTreeControlValueFromChildControl(hItem);
    DestroyOldChildControl();
    CreateNewChildControl(hItem);
  }

  return 0L;
}

DWORD_PTR CTreeOptionsCtrl::GetUserItemData(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  return pItemData->m_dwItemData;
}

BOOL CTreeOptionsCtrl::SetUserItemData(HTREEITEM hItem, DWORD_PTR dwData)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_dwItemData = dwData;
  return TRUE;
}

BOOL CTreeOptionsCtrl::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
  //Clean up any controls currently open we used
  if (m_hControlItem)
  {
    UpdateTreeControlValueFromChildControl(m_hControlItem);
    DestroyOldChildControl();
  }

  //Let the parent class do its thing
  return CTreeCtrl::OnMouseWheel(nFlags, zDelta, pt);
}

BOOL CTreeOptionsCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	
	*pResult = 0;

  //Free up the memory we had allocated in the item data (if needed)
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(pNMTreeView->itemOld.hItem));
  if (pItemData)
    delete pItemData;

  return FALSE;
}

void CTreeOptionsCtrl::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags )
{
	if (nChar == VK_RIGHT)
	{
		HTREEITEM hItem = GetSelectedItem();
		if (GetItemData(hItem) && m_hControlItem != NULL)
		{ 
      // if we have a children and VK_RIGHT -> Focus on it !
			CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
			if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsCombo)))
			{
				if (m_pCombo->IsWindowVisible())
					m_pCombo->SetFocus();
			}
			else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsEdit)))
			{
        if (m_pMultiLineEditFrameWnd)
					m_pMultiLineEditFrameWnd->SetFocus();
        else
        {
				  if (m_pEdit->IsWindowVisible())
					  m_pEdit->SetFocus();
        }
			}
			else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsDateCtrl)))
			{
				if (m_pDateTime->IsWindowVisible())
					m_pDateTime->SetFocus();
			}
			else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsIPAddressCtrl)))
			{
				if (m_pIPAddress->IsWindowVisible())
					m_pIPAddress->SetFocus();
			}
			else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)))
			{
				if (m_pButton->IsWindowVisible())
					m_pButton->SetFocus();
			}
			else
      {
				//Pass on to the parent since we didn't handle it
  			CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
      }
		}
		else
    {
	    //Pass on to the parent since we didn't handle it
		  CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
    }
	}
	else
  {
	  //Pass on to the parent since we didn't handle it
	  CTreeCtrl::OnKeyDown(nChar, nRepCnt, nFlags);
  }
}

BOOL CTreeOptionsCtrl::HandleCheckBox(HTREEITEM hItem, BOOL bCheck)
{
  //What will be the return value from this method (Assume the worst)
  BOOL bSuccess = FALSE;

  //Turn of redraw to Q all the changes we're going to make here
  SetRedraw(FALSE);

  //Toggle the state
  bSuccess = SetCheckBoxIntern(hItem, !bCheck);

  //If the item has children, then iterate through them and for all items
  //which are check boxes set their state to be the same as the parent
  HTREEITEM hChild = GetNextItem(hItem, TVGN_CHILD);
  while (hChild)
  {
    if (IsCheckBox(hChild))
      SetCheckBoxIntern(hChild, !bCheck);  

    //Move on to the next item
    hChild = GetNextItem(hChild, TVGN_NEXT);
  }

  //Get the parent item and if it is a checkbox, then iterate through 
  //all its children and if all the checkboxes are checked, then also
  //automatically check the parent. If no checkboxes are checked, then
  //also automatically uncheck the parent.
  HTREEITEM hParent = GetNextItem(hItem, TVGN_PARENT);
  if (hParent && IsCheckBox(hParent))
  {
    BOOL bNoCheckBoxesChecked = TRUE;
    BOOL bAllCheckBoxesChecked = TRUE;
    hChild = GetNextItem(hParent, TVGN_CHILD);
    while (hChild)
    {
      if (IsCheckBox(hChild))
      {
        BOOL bThisChecked;
        VERIFY(GetCheckBox(hChild, bThisChecked));
        bNoCheckBoxesChecked = bNoCheckBoxesChecked && !bThisChecked;
        bAllCheckBoxesChecked = bAllCheckBoxesChecked && bThisChecked;
      }

      //Move on to the next item
      hChild = GetNextItem(hChild, TVGN_NEXT);
    }

    if (bNoCheckBoxesChecked)
    {
      SetCheckBoxIntern(hParent, FALSE);
      SetSemiCheckBox(hParent, FALSE);
    }
    else if (bAllCheckBoxesChecked)
    {
      SetCheckBoxIntern(hParent, TRUE);
      SetSemiCheckBox(hParent, FALSE);
    }
    else
    {
      SetCheckBoxIntern(hParent, TRUE);
      SetSemiCheckBox(hParent, TRUE);
    }
  }

  //Reset the redraw flag
  SetRedraw(TRUE);

  return bSuccess;
}

void CTreeOptionsCtrl::OnLButtonDown(UINT nFlags, CPoint point) 
{
  UINT uFlags = 0;
	HTREEITEM hItem = HitTest(point, &uFlags);

  //If the mouse was over the label, icon or to the left or right of the item ?
  BOOL bHit = FALSE;
  if (m_bToggleOverIconOnly)
    bHit = uFlags == TVHT_ONITEMICON;
  else
    bHit = (uFlags & TVHT_ONITEM) || (uFlags & TVHT_ONITEMINDENT) || (uFlags & TVHT_ONITEMRIGHT);

	if (bHit)
	{
    if (IsCheckBox(hItem))
    {
      BOOL bEnable;
      VERIFY(GetCheckBoxEnable(hItem, bEnable));
      if (bEnable)
      {
        //Turn of redraw to Q all the changes we're going to make here
        SetRedraw(FALSE);

        //Toggle the state of check items
        BOOL bCheck;
        VERIFY(GetCheckBox(hItem, bCheck));
        HandleCheckBox(hItem, bCheck);
      }
    }
    else if (IsRadioButton(hItem))
    {
      BOOL bEnable;
      VERIFY(GetRadioButtonEnable(hItem, bEnable));
      if (bEnable)
      {
        //Check the radio button if not already checked
        BOOL bCheck;
        VERIFY(GetRadioButton(hItem, bCheck));
        if (!bCheck)
          VERIFY(SetRadioButton(hItem));
      }
    }

    //Pass on to the parent now that we that we have done our stuff
  	CTreeCtrl::OnLButtonDown(nFlags, point);
	}
  else
  {
    //Pass on to the parent since we didn't handle it
  	CTreeCtrl::OnLButtonDown(nFlags, point);
  }
}

void CTreeOptionsCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
 	if (nChar == VK_SPACE)
  {
		HTREEITEM hItem = GetSelectedItem();

    //If the space key is hit, and the item is a combo item, then toggle the check value
    if (IsCheckBox(hItem))
    {
      BOOL bEnable;
      VERIFY(GetCheckBoxEnable(hItem, bEnable));
      if (bEnable)
      {
        //Turn of redraw to Q all the changes we're going to make here
        SetRedraw(FALSE);

        BOOL bCheck;
        VERIFY(GetCheckBox(hItem, bCheck));
        HandleCheckBox(hItem, bCheck);
      }
      else
      {
        //Pass on to the parent since we didn't handle it
        CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
      }
    }
    else if (IsRadioButton(hItem)) //If the item is a radio item, then check it and uncheck all other items
    {
      BOOL bEnable;
      VERIFY(GetRadioButtonEnable(hItem, bEnable));
      if (bEnable)
      {
        //Check the radio button if not already checked
        BOOL bCheck;
        VERIFY(GetRadioButton(hItem, bCheck));
        if (!bCheck)
          VERIFY(SetRadioButton(hItem));
      }
      else
      {
        //Pass on to the parent since we didn't handle it
        CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
      }
    }
    else
    {
      //Pass on to the parent since we didn't handle it
      CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
    }
  }
  else
  {
    //Pass on to the parent since we didn't handle it
  	CTreeCtrl::OnChar(nChar, nRepCnt, nFlags);
  }
}

HTREEITEM CTreeOptionsCtrl::InsertGroup(LPCTSTR lpszItem, int nImage, HTREEITEM hParent, HTREEITEM hAfter, DWORD_PTR dwItemData)
{
  HTREEITEM hItem = InsertItem(lpszItem, nImage, nImage, hParent, hAfter);
  CTreeOptionsItemData* pItemData = new CTreeOptionsItemData;
  pItemData->m_pRuntimeClass1 = NULL;
  pItemData->m_Type = CTreeOptionsItemData::Normal;
  pItemData->m_dwItemData = dwItemData;
  SetItemData(hItem, (DWORD_PTR) pItemData);

  return hItem;
}

HTREEITEM CTreeOptionsCtrl::InsertCheckBox(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck, HTREEITEM hAfter, DWORD_PTR dwItemData)
{
  ASSERT((hParent == TVI_ROOT) || IsGroup(hParent) || IsCheckBox(hParent)); //The parent of a check box must be a group item or another check box

  HTREEITEM hItem = InsertItem(lpszItem, hParent, hAfter);
  CTreeOptionsItemData* pItemData = new CTreeOptionsItemData;
  pItemData->m_pRuntimeClass1 = NULL;
  pItemData->m_Type = CTreeOptionsItemData::CheckBox;
  pItemData->m_dwItemData = dwItemData;
  SetItemData(hItem, (DWORD_PTR) pItemData);
  SetCheckBoxIntern(hItem, bCheck);

  return hItem;
}

HTREEITEM CTreeOptionsCtrl::InsertRadioButton(LPCTSTR lpszItem, HTREEITEM hParent, BOOL bCheck, HTREEITEM hAfter, DWORD_PTR dwItemData)
{
  ASSERT(IsGroup(hParent)); //The parent of a radio item must be a group item

  HTREEITEM hItem = InsertItem(lpszItem, hParent, hAfter);
  CTreeOptionsItemData* pItemData = new CTreeOptionsItemData;
  pItemData->m_pRuntimeClass1 = NULL;
  pItemData->m_Type = CTreeOptionsItemData::RadioButton;
  pItemData->m_dwItemData = dwItemData;
  SetItemData(hItem, (DWORD_PTR) pItemData);
  if (bCheck)
  {
    //if requested to, check the newly added radio button
    SetRadioButton(hItem);
  }

  return hItem;
}

BOOL CTreeOptionsCtrl::IsGroup(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::Normal;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsCheckBox(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::CheckBox;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsRadioButton(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::RadioButton;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsEditBox(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::EditBox;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsColorItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::ColorBrowser;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsFontItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::FontBrowser;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsFileItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::FileBrowser;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsFolderItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::FolderBrowser;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsDateTimeItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::DateTimeCtrl;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsIPAddressItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::IPAddressCtrl;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::IsOpaqueItem(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData)
    return pItemData->m_Type == CTreeOptionsItemData::OpaqueBrowser;
  else
    return FALSE;
}

BOOL CTreeOptionsCtrl::SetCheckBox(HTREEITEM hItem, BOOL bCheck)
{
	return HandleCheckBox(hItem,!bCheck);
}

BOOL CTreeOptionsCtrl::SetCheckBoxIntern(HTREEITEM hItem, BOOL bCheck)
{
  //Validate our parameters
  ASSERT(IsCheckBox(hItem)); //Must be a check box to check it

  //What will be the return value from this method (Assume the worst)
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::CheckBox)
  {
    bSuccess = TRUE;
    pItemData->m_bChecked = bCheck ? true : false;

    //Force a repaint of the item
    CRect r;
    GetItemRect(hItem, &r, FALSE);
    InvalidateRect(&r);
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::SetSemiCheckBox(HTREEITEM hItem, BOOL bSemi)
{
  //Validate our parameters
  ASSERT(IsCheckBox(hItem)); //Must be a check box to check it

  //What will be the return value from this method (Assume the worst)
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::CheckBox)
  {
    bSuccess = TRUE;
    pItemData->m_bSemiChecked = bSemi ? true : false;

    //Force a repaint of the item
    CRect r;
    GetItemRect(hItem, &r, FALSE);
    InvalidateRect(&r);
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::GetCheckBox(HTREEITEM hItem, BOOL& bCheck) const
{
  //What will be the return value from this method (Assume the worst)
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::CheckBox)
  {
    bSuccess = TRUE;
    bCheck = pItemData->m_bChecked;
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::SetRadioButton(HTREEITEM hParent, int nIndex)
{
  //Validate our parameters
  ASSERT(IsGroup(hParent)); //Parent item must be a group item

  //Iterate through the child items and turn on the specified one and turn off all the other ones
  HTREEITEM hChild = GetNextItem(hParent, TVGN_CHILD);

  //Turn of redraw to Q all the changes we're going to make here
  SetRedraw(FALSE);
  
  int i=0;
  BOOL bCheckedSomeItem = FALSE;
  while (hChild)
  {
    //if we reach a non radio button then break out of the loop
    if (!IsRadioButton(hChild))
      break;

    if (i == nIndex)
    {
      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hChild));
      if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
      {
        pItemData->m_bChecked = TRUE;
        bCheckedSomeItem = TRUE;

        //Force a repaint of the item
        CRect r;
        GetItemRect(hChild, &r, FALSE);
        InvalidateRect(&r);
      }
    }
    else
    {
      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hChild));
      if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
      {
        pItemData->m_bChecked = FALSE;

        //Force a repaint of the item
        CRect r;
        GetItemRect(hChild, &r, FALSE);
        InvalidateRect(&r);
      }
    }

    //Move on to the next item
    hChild = GetNextItem(hChild, TVGN_NEXT);
    ++i;
  }
  ASSERT(bCheckedSomeItem); //You specified an index which does not exist

  //Reset the redraw flag
  SetRedraw(TRUE);

  return TRUE;
}

BOOL CTreeOptionsCtrl::SetRadioButton(HTREEITEM hItem)
{
  //Validate our parameters
  ASSERT(IsRadioButton(hItem)); //Must be a radio item to check it

  //Iterate through the sibling items and turn them all off except this one
  HTREEITEM hParent = GetNextItem(hItem, TVGN_PARENT);
  ASSERT(IsGroup(hParent)); //Parent item must be a group item

  //Iterate through the child items and turn on the specified one and turn off all the other ones
  HTREEITEM hChild = GetNextItem(hParent, TVGN_CHILD);

  //Turn of redraw to Q all the changes we're going to make here
  SetRedraw(FALSE);

  while (hChild)
  {
    //if we reach a non radio button then break out of the loop
    if (!IsRadioButton(hChild))
      break;

    if (hChild == hItem)
    {
      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hChild));
      ASSERT(pItemData);
      ASSERT(pItemData->m_Type == CTreeOptionsItemData::RadioButton);
      pItemData->m_bChecked = TRUE;

      //Force a repaint of the item
      CRect r;
      GetItemRect(hChild, &r, FALSE);
      InvalidateRect(&r);
    }
    else
    {
      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hChild));
      AFXASSUME(pItemData);
      ASSERT(pItemData->m_Type == CTreeOptionsItemData::RadioButton);
      pItemData->m_bChecked = FALSE;

      //Force a repaint of the item
      CRect r;
      GetItemRect(hChild, &r, FALSE);
      InvalidateRect(&r);
    }

    //Move on to the next item
    hChild = GetNextItem(hChild, TVGN_NEXT);
  }

  //Reset the redraw flag
  SetRedraw(TRUE);

  return TRUE;
}

BOOL CTreeOptionsCtrl::GetRadioButton(HTREEITEM hParent, int& nIndex, HTREEITEM& hCheckItem) const
{
  ASSERT(IsGroup(hParent)); //Parent item must be a group item

  //Iterate through the child items and turn on the specified one and turn off all the other ones
  HTREEITEM hChild = GetNextItem(hParent, TVGN_CHILD);
  AFXASSUME(hChild); //Must have some child items

  //Find the checked item  
  nIndex = 0;
  BOOL bFound = FALSE;
  while (!bFound)
  {
    if (!IsRadioButton(hChild))  // Handle multiple groups
      nIndex = 0;

    GetRadioButton(hChild, bFound);

    //Move on to the next sibling if not found
    if (!bFound)
    {
      hChild = GetNextItem(hChild, TVGN_NEXT);
      AFXASSUME(hChild);
      ++nIndex;
    }
    else
    {
      hCheckItem = hChild;
      break; //This group is done
    }
  }

  return TRUE;
}

BOOL CTreeOptionsCtrl::GetRadioButton(HTREEITEM hItem, BOOL& bCheck) const
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
  {
    bSuccess = TRUE;
    bCheck = pItemData->m_bChecked;
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::SetGroupEnable(HTREEITEM hItem, BOOL bEnable)
{
  //Allows you to quickly enable / disable all the items in a group

  ASSERT(IsGroup(hItem)); //Must be group item

  //Iterate through the child items and enable / disable all the items
  HTREEITEM hChild = GetNextItem(hItem, TVGN_CHILD);

  //Turn of redraw to Q all the changes we're going to make here
  SetRedraw(FALSE);

  while (hChild)
  {
    CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hChild));
    AFXASSUME(pItemData);
    ASSERT(pItemData->m_Type == CTreeOptionsItemData::RadioButton || pItemData->m_Type == CTreeOptionsItemData::CheckBox);
    pItemData->m_bEnabled = bEnable ? true : false;

    //Force a repaint of the item
    CRect r;
    GetItemRect(hChild, &r, FALSE);
    InvalidateRect(&r);

    //Move onto the next child
    hChild = GetNextItem(hChild, TVGN_NEXT);
  }

  //Reset the redraw flag
  SetRedraw(TRUE);

  return TRUE;
}

BOOL CTreeOptionsCtrl::GetSemiCheckBox(HTREEITEM hItem, BOOL& bSemi) const
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
  {
    bSuccess = TRUE;
    bSemi = pItemData->m_bSemiChecked;
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::SetCheckBoxEnable(HTREEITEM hItem, BOOL bEnable)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::CheckBox)
  {
    bSuccess = TRUE;
    pItemData->m_bEnabled = bEnable ? true : false;

    //Force a repaint of the item
    CRect r;
    GetItemRect(hItem, &r, FALSE);
    InvalidateRect(&r);
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::SetRadioButtonEnable(HTREEITEM hItem, BOOL bEnable)
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
  {
    bSuccess = TRUE;
    pItemData->m_bEnabled = bEnable ? true : false;

    //Force a repaint of the item
    CRect r;
    GetItemRect(hItem, &r, FALSE);
    InvalidateRect(&r);
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::GetCheckBoxEnable(HTREEITEM hItem, BOOL& bEnable) const
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::CheckBox)
  {
    bSuccess = TRUE;
    bEnable = pItemData->m_bEnabled;
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::GetRadioButtonEnable(HTREEITEM hItem, BOOL& bEnable) const
{
  //Assume the worst
  BOOL bSuccess = FALSE;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pItemData && pItemData->m_Type == CTreeOptionsItemData::RadioButton)
  {
    bSuccess = TRUE;
    bEnable = pItemData->m_bEnabled;
  }
  
  return bSuccess;
}

BOOL CTreeOptionsCtrl::AddComboBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClass, DWORD_PTR dwItemData)
{
  ASSERT(pRuntimeClass);

  //Delete the old item data in the item if there is one already
  CTreeOptionsItemData* pOldItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  if (pOldItemData)
    delete pOldItemData;

  //A pointer to the runtime class is stored in the Item data which itself is an 
  //internal structure we maintain per tree item
  CTreeOptionsItemData* pItemData = new CTreeOptionsItemData;
  pItemData->m_dwItemData = dwItemData;
  pItemData->m_pRuntimeClass1 = pRuntimeClass;
  pItemData->m_Type = CTreeOptionsItemData::ComboBox;

  return SetItemData(hItem, (DWORD_PTR) pItemData);
}

CString CTreeOptionsCtrl::GetComboText(HTREEITEM hItem) const
{
  CString sText(GetItemText(hItem));
  int nSeparator = sText.Find(m_sSeparator);
  CString sComboText;
  if (nSeparator != -1)
    sComboText = sText.Right(sText.GetLength() - nSeparator - m_sSeparator.GetLength());

  return sComboText;
}

void CTreeOptionsCtrl::RemoveChildControlText(HTREEITEM hItem)
{
  CString sText(GetItemText(hItem));
  int nSeparator = sText.Find(m_sSeparator);
  if (nSeparator != -1)
    sText = sText.Left(nSeparator);
  SetItemText(hItem, sText);
}

void CTreeOptionsCtrl::SetComboText(HTREEITEM hItem, const CString& sComboText)
{
  CString sText(GetItemText(hItem));
  int nSeparator = sText.Find(m_sSeparator);
  if (nSeparator == -1)
  {
    sText += m_sSeparator;
    sText += sComboText;
  }
  else
  {
    sText = sText.Left(nSeparator + m_sSeparator.GetLength());
    sText += sComboText;
  }
  SetItemText(hItem, sText);
}

BOOL CTreeOptionsCtrl::_EnumIPAddressWindows(HWND hWnd, LPARAM /*lParam*/)
{
  //Destroy this child window
  ::SendMessage(hWnd, WM_CLOSE, 0, 0L);

  //Continue enumeration of the child windows
  return TRUE;
}

void CTreeOptionsCtrl::DestroyOldChildControl()
{
  if (m_pMultiLineEditFrameWnd)
  {
    m_pMultiLineEditFrameWnd->SendMessage(WM_CLOSE);
    m_pMultiLineEditFrameWnd = NULL;
  }
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
  if (m_pDateTime)
  {
    m_pDateTime->DestroyWindow();
    delete m_pDateTime;
    m_pDateTime = NULL;
  }
  if (m_pIPAddress)
  {
    //Special case code here, make sure any ancestor windows of m_pIPAddress 
    //are first closed before we destroy the IP Address control. This avoids
    //errors deep down in the DefWindowProc for IP Address controls
    EnumChildWindows(m_pIPAddress->GetSafeHwnd(), _EnumIPAddressWindows, reinterpret_cast<LPARAM>(this));
    m_pIPAddress->DestroyWindow();
    delete m_pIPAddress;
    m_pIPAddress = NULL;
  }
  
  //Free up the font object we have been using
  m_Font.DeleteObject();

  m_hControlItem = NULL;
}

void CTreeOptionsCtrl::CreateNewChildControl(HTREEITEM hItem)
{
  //Validate our parameters
  AFXASSUME(hItem);
  
  m_hControlItem = hItem;

  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);

  //Make a copy of the current font being used by the control
  ASSERT(m_Font.m_hObject == NULL);
  CFont* pFont = GetFont();
  LOGFONT lf;
  pFont->GetLogFont(&lf);
  VERIFY(m_Font.CreateFontIndirect(&lf));

  //Allocate the main control
  AFXASSUME(pItemData->m_pRuntimeClass1 != NULL);
  CString sComboText;
  CString sEditText;
  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsCombo)))
  {
    //Get the current text in the combo item
    sComboText = GetComboText(hItem);

    //Now that we have the current text remove it from the tree control text
    RemoveChildControlText(hItem);

    //Create the new combo box
    m_pCombo = static_cast<CTreeOptionsCombo*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pCombo);
    ASSERT(m_pCombo->IsKindOf(RUNTIME_CLASS(CTreeOptionsCombo)));  //Your class must be derived from CTreeOptionsCombo
    m_pCombo->SetTreeBuddy(this);
    m_pCombo->SetTreeItem(hItem);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsEdit)))
  {
    //Get the current text in the edit box item
    sEditText = GetEditText(hItem);

    //Now that we have the current text remove it from the tree control text
    RemoveChildControlText(hItem);

    //Create the new edit box
    m_pEdit = static_cast<CTreeOptionsEdit*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pEdit);
    ASSERT(m_pEdit->IsKindOf(RUNTIME_CLASS(CTreeOptionsEdit)));  //Your class must be derived from CTreeOptionsEdit
    m_pEdit->SetTreeBuddy(this);
    m_pEdit->SetTreeItem(hItem);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsDateCtrl)))
  {
    //Get the current text in the edit box item
    sEditText = GetEditText(hItem);

    //Now that we have the current text remove it from the tree control text
    RemoveChildControlText(hItem);

    //Create the new edit box
    m_pDateTime = static_cast<CTreeOptionsDateCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pDateTime);
    ASSERT(m_pDateTime->IsKindOf(RUNTIME_CLASS(CTreeOptionsDateCtrl)));  //Your class must be derived from CTreeOptionsDateCtrl
    m_pDateTime->SetTreeBuddy(this);
    m_pDateTime->SetTreeItem(hItem);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsIPAddressCtrl)))
  {
    //Get the current text in the edit box item
    sEditText = GetEditText(hItem);

    //Now that we have the current text remove it from the tree control text
    RemoveChildControlText(hItem);

    //Create the new edit box
    m_pIPAddress = static_cast<CTreeOptionsIPAddressCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pIPAddress);
    ASSERT(m_pIPAddress->IsKindOf(RUNTIME_CLASS(CTreeOptionsIPAddressCtrl)));  //Your class must be derived from CTreeOptionsIPAddressCtrl
    m_pIPAddress->SetTreeBuddy(this);
    m_pIPAddress->SetTreeItem(hItem);
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)))
  {
    //work out the rects for item
    CRect rText;
    GetItemRect(hItem, &rText, TRUE);
    CRect rLine;
    GetItemRect(hItem, &rLine, FALSE);

    //Create the new static
    m_pButton = static_cast<CTreeOptionsBrowseButton*>(pItemData->m_pRuntimeClass1->CreateObject());
    ASSERT(m_pButton);
    ASSERT(m_pButton->IsKindOf(RUNTIME_CLASS(CTreeOptionsBrowseButton)));  //Your class must be derived from CTreeOptionsStatic
    m_pButton->SetTreeBuddy(this);
    m_pButton->SetTreeItem(hItem);

    if (pItemData->m_Type == CTreeOptionsItemData::ColorBrowser)
    {
      //Get the current color from the control and let the button know about it
      COLORREF color = GetColor(hItem);
      m_pButton->SetColor(color);
    }
    else if (pItemData->m_Type == CTreeOptionsItemData::FontBrowser)
    {
      GetFontItem(hItem, &lf);
      m_pButton->SetFontItem(&lf);
    }
    else 
    {
      ASSERT(pItemData->m_Type == CTreeOptionsItemData::OpaqueBrowser);
    }
  }
  else
    ASSERT(FALSE); //Your class must be derived from one of the classes in the previous statements

  //allocate the secondary control
  if (pItemData->m_pRuntimeClass2)
  {
    if (pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsSpinCtrl)))
    {
      m_pSpin = static_cast<CTreeOptionsSpinCtrl*>(pItemData->m_pRuntimeClass2->CreateObject());
      ASSERT(m_pSpin);
      ASSERT(m_pSpin->IsKindOf(RUNTIME_CLASS(CTreeOptionsSpinCtrl)));  //Your class must be derived from CTreeOptionsSpinCtrl
      m_pSpin->SetTreeBuddy(this);
      m_pSpin->SetTreeItem(hItem);
    }
    else
    {
      ASSERT(pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)));
      m_pButton = static_cast<CTreeOptionsBrowseButton*>(pItemData->m_pRuntimeClass2->CreateObject());
      ASSERT(m_pButton);
      ASSERT(m_pButton->IsKindOf(RUNTIME_CLASS(CTreeOptionsBrowseButton)));  //Your class must be derived from CTreeOptionsBrowseButton
      m_pButton->SetTreeBuddy(this);
      m_pButton->SetTreeItem(hItem);
    }
  }

  //Update the rects for item
  CRect rClient;
  GetClientRect(rClient);
  CRect rText;
  BOOL bVisible = GetItemRect(hItem, &rText, TRUE);
  if (!bVisible || (rText.bottom > rClient.bottom))
  {
    EnsureVisible(hItem);
    GetItemRect(hItem, &rText, TRUE);
  }
  CRect rLine;
  GetItemRect(hItem, &rLine, FALSE);

  CRect r;
  r.top = rText.top;
  r.left = rText.right + 2;

  //Now create the main control
  ASSERT(pItemData->m_pRuntimeClass1);
  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsCombo)))
  {
    if (m_pButton)
      r.right = rLine.right - m_pButton->GetWidth();
    else
      r.right = rLine.right;
    r.bottom = r.top + m_pCombo->GetDropDownHeight(); //Ask the combo box for the height to use
    m_pCombo->Create(m_pCombo->GetWindowStyle(), r, this, TREE_OPTIONS_COMBOBOX_ID);
    ASSERT(m_pCombo->GetCount()); //You forget to add string items to the combo box in your OnCreate message handler!

    //set the font the combo box should use based on the font in the tree control, 
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
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsEdit)))
  {
    if (m_pButton)
      r.right = rLine.right - m_pButton->GetWidth();
    else
      r.right = rLine.right;
    r.bottom = r.top + m_pEdit->GetHeight(rText.Height());
    
    DWORD dwEditStyle = m_pEdit->GetWindowStyle();
    if (dwEditStyle & ES_MULTILINE)
    {
      sEditText = pItemData->m_sMultiLineText;
      ClientToScreen(&r);
      GetDesktopWindow()->ScreenToClient(&r);

      //Create the frame window and in it plop the multi line edit control
      m_pMultiLineEditFrameWnd = new CMultiLineEditFrameWnd();
      VERIFY(m_pMultiLineEditFrameWnd->Create(NULL, _T(""), WS_POPUP, r, NULL, NULL, WS_EX_TOOLWINDOW));
      m_pEdit->SetMultiLineFrameWndBuddy(m_pMultiLineEditFrameWnd);
      VERIFY(m_pEdit->CreateEx(0, _T("Edit"), sEditText, dwEditStyle, CRect(0, 0, 0, 0), m_pMultiLineEditFrameWnd, TREE_OPTIONS_EDITBOX_ID));
      m_pMultiLineEditFrameWnd->GetClientRect(&rClient);
      m_pEdit->SetWindowPos(NULL, r.left, r.top, r.Width(), r.Height(), SWP_NOMOVE | SWP_NOZORDER | SWP_NOREDRAW | SWP_NOACTIVATE);
      m_pMultiLineEditFrameWnd->m_pEdit = m_pEdit;
      m_pMultiLineEditFrameWnd->ShowWindow(SW_SHOWNOACTIVATE);

      //set the font the edit box should use based on the font in the tree control
      m_pMultiLineEditFrameWnd->m_pEdit->SetFont(&m_Font);

      m_pEdit = NULL;

      //Auto select the control if configured to do so
      if (m_bAutoSelect)
        m_pMultiLineEditFrameWnd->SetFocus();
    }
    else
    {
      VERIFY(m_pEdit->CreateEx(WS_EX_CLIENTEDGE, _T("Edit"), sEditText, dwEditStyle, r, this, TREE_OPTIONS_EDITBOX_ID));

      //set the font the edit box should use based on the font in the tree control
      m_pEdit->SetFont(&m_Font);

      //Auto select the control if configured to do so
      if (m_bAutoSelect)
        m_pEdit->SetFocus();
    }
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsDateCtrl)))
  {
    r.right = rLine.right;
    r.bottom = rLine.bottom;
    VERIFY(m_pDateTime->Create(m_pDateTime->GetWindowStyle(), r, this, TREE_OPTIONS_DATETIMECTRL_ID));

    //set the font the date time control should use based on the font in the list control
    m_pDateTime->SetFont(&m_Font);

    //set the value in the control
    m_pDateTime->SetTime(pItemData->m_DateTime);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pDateTime->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsIPAddressCtrl)))
  {
    r.right = rLine.right;
    r.bottom = rLine.bottom;

    VERIFY(m_pIPAddress->Create(m_pIPAddress->GetWindowStyle(), r, this, TREE_OPTIONS_IPADDRESSCTRL_ID));

    //set the font the IP Address control should use based on the font in the list control
    m_pIPAddress->SetFont(&m_Font);

    DWORD dwAddress = GetIPAddress(hItem);
    m_pIPAddress->SetAddress(dwAddress);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      m_pIPAddress->SetFocus();
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)))
  {
    CreateBrowseButton(pItemData->m_pRuntimeClass1, rLine, rText);
  }
  else
    ASSERT(FALSE); //Your class must be derived from one of the classes in the statements above

  //Actually create the secondary control
  if (pItemData->m_pRuntimeClass2)
  {
    if (pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsSpinCtrl)))
      CreateSpinCtrl(pItemData->m_pRuntimeClass2, rLine, rText, r);
    else
    {
      ASSERT(pItemData->m_pRuntimeClass2->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)));
      CreateBrowseButton(pItemData->m_pRuntimeClass2, rLine, rText);

      if (m_pMultiLineEditFrameWnd)
        m_pMultiLineEditFrameWnd->m_pEdit->SetButtonBuddy(m_pButton);
      else if (m_pEdit)
        m_pEdit->SetButtonBuddy(m_pButton);
      else 
      {
        ASSERT(m_pCombo);
        m_pCombo->SetButtonBuddy(m_pButton);
      }
    }
  }
}

void CTreeOptionsCtrl::UpdateTreeControlValueFromChildControl(HTREEITEM hItem)
{
  if (m_pCombo)
  {
    CString sText;
    m_pCombo->GetWindowText(sText);
    SetComboText(m_hControlItem, sText);
  }
  else if (m_pEdit)
  {
    CString sText;
    m_pEdit->GetWindowText(sText);
    SetEditText(m_hControlItem, sText);
  }
  else if (m_pMultiLineEditFrameWnd)
  {
    CString sText;
    AFXASSUME(m_pMultiLineEditFrameWnd->m_pEdit != NULL);
    m_pMultiLineEditFrameWnd->m_pEdit->GetWindowText(sText);

    CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
    AFXASSUME(pItemData != NULL);
    pItemData->m_sMultiLineText = sText;
    sText = m_pMultiLineEditFrameWnd->m_pEdit->FormControlText(sText);
    SetEditText(m_hControlItem, sText);
  }
  else if (m_pDateTime)
  {
    SYSTEMTIME st1;
    if (m_pDateTime->GetTime(&st1) == GDT_VALID)
      m_pDateTime->SetDateTime(st1);

    SYSTEMTIME st2;
    m_pDateTime->GetDateTime(st2);
    SetDateTime(m_hControlItem, st2);
    SetEditText(m_hControlItem, m_pDateTime->GetDisplayText(st2));
  }
  else if (m_pIPAddress)
  {
    DWORD dwAddress;
    if (m_pIPAddress->GetAddress(dwAddress) == 4)
    {
      SetIPAddress(m_hControlItem, dwAddress);
      SetEditText(m_hControlItem, m_pIPAddress->GetDisplayText(dwAddress));
    }
  }
  else if (m_pButton)
  {
    CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
    AFXASSUME(pItemData);
    if (pItemData->m_Type == CTreeOptionsItemData::ColorBrowser)
    {
      COLORREF color = m_pButton->GetColor();
      SetColor(m_hControlItem, color);
    }
    else if (pItemData->m_Type == CTreeOptionsItemData::FontBrowser)
    {
      LOGFONT lf;
      GetFontItem(hItem, &lf);
      m_pButton->SetFontItem(&lf);
    }
  }
}

BOOL CTreeOptionsCtrl::AddEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, DWORD_PTR dwItemData)
{
  //Just call the combo box version as currently there is no difference
  BOOL bSuccess = AddComboBox(hItem, pRuntimeClassEditCtrl, dwItemData);

  //Update the type in the item data
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData != NULL);
  pItemData->m_Type = CTreeOptionsItemData::EditBox;

  return bSuccess;
}

BOOL CTreeOptionsCtrl::AddEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassSpinCtrl, DWORD_PTR dwItemData)
{
  //Add the edit box
  BOOL bSuccess = AddEditBox(hItem, pRuntimeClassEditCtrl, dwItemData);

  //Add the spin ctrl
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData != NULL);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  ASSERT(pRuntimeClassSpinCtrl);
  pItemData->m_pRuntimeClass2 = pRuntimeClassSpinCtrl;
  pItemData->m_Type = CTreeOptionsItemData::Spin;
  pItemData->m_dwItemData = dwItemData;

  return bSuccess;
}

BOOL CTreeOptionsCtrl::AddFileEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Add the edit box
  BOOL bSuccess = AddEditBox(hItem, pRuntimeClassEditCtrl, dwItemData);

  //Add the browse button
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData != NULL);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  ASSERT(pRuntimeClassButton);
  pItemData->m_pRuntimeClass2 = pRuntimeClassButton;
  pItemData->m_Type = CTreeOptionsItemData::FileBrowser;

  return bSuccess;
}

BOOL CTreeOptionsCtrl::AddColorSelector(HTREEITEM hItem, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData, BOOL bDrawColorForIcon)
{
  //Add the browse button as the primary control
  BOOL bSuccess = AddEditBox(hItem, pRuntimeClassButton, dwItemData);

  //Setup the browser type
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_Type = CTreeOptionsItemData::ColorBrowser;
  pItemData->m_bDrawColorForIcon = bDrawColorForIcon ? true : false;
  
  return bSuccess;
}

COLORREF CTreeOptionsCtrl::GetColor(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  return pItemData->m_Color;
}

void CTreeOptionsCtrl::SetColor(HTREEITEM hItem, COLORREF color)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_Color = color;

  //Also update the text while we are at it
  CString sColor;
  sColor.Format(_T("&%02x%02x%02x"), GetRValue(color), GetGValue(color), GetBValue(color));
  sColor.MakeUpper();
  SetEditText(hItem, sColor);
}

void CTreeOptionsCtrl::SetMultiLineEditText(HTREEITEM hItem, const CString& sEditText)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  ASSERT(pItemData->m_Type == CTreeOptionsItemData::EditBox);
  pItemData->m_sMultiLineText = sEditText;

  //Also update the text while we are at it (for that we need to create a copy of the edit control)
  CTreeOptionsEdit* pEdit = reinterpret_cast<CTreeOptionsEdit*>(pItemData->m_pRuntimeClass1->CreateObject());
  AFXASSUME(pEdit);
  ASSERT(pEdit->IsKindOf(RUNTIME_CLASS(CTreeOptionsEdit)));  //Your class must be derived from CTreeOptionsEdit
  CString sControlText(pEdit->FormControlText(sEditText));
  SetEditText(hItem, sControlText);
  delete pEdit;
}

CString CTreeOptionsCtrl::GetMultiLineEditText(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  return pItemData->m_sMultiLineText;
}

BOOL CTreeOptionsCtrl::AddFontSelector(HTREEITEM hItem, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Add the browse button as the primary control
  BOOL bSuccess = AddEditBox(hItem, pRuntimeClassButton, dwItemData);

  //Setup the browser type
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_Type = CTreeOptionsItemData::FontBrowser;
  
  return bSuccess;
}

void CTreeOptionsCtrl::GetFontItem(HTREEITEM hItem, LOGFONT* pLogFont) const
{
  ASSERT(pLogFont);
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  memcpy(pLogFont, &pItemData->m_Font, sizeof(LOGFONT));
}

void CTreeOptionsCtrl::SetFontItem(HTREEITEM hItem, const LOGFONT* pLogFont)
{
  AFXASSUME(pLogFont);
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  memcpy(&pItemData->m_Font, pLogFont, sizeof(LOGFONT));

  //Also update the text while we are at it
  SetEditText(hItem, pLogFont->lfFaceName);
}

CString CTreeOptionsCtrl::GetFileEditText(HTREEITEM hItem) const
{
  //Just call the edit box version as currently there is no difference
  return GetEditText(hItem);
}

void CTreeOptionsCtrl::SetFileEditText(HTREEITEM hItem, const CString& sEditText)
{
  //Just call the edit box version as currently there is no difference
  SetEditText(hItem, sEditText);
}

BOOL CTreeOptionsCtrl::AddFolderEditBox(HTREEITEM hItem, CRuntimeClass* pRuntimeClassEditCtrl, CRuntimeClass* pRuntimeClassButton, DWORD_PTR dwItemData)
{
  //Let the File edit box code do all the work
  BOOL bSuccess = AddFileEditBox(hItem, pRuntimeClassEditCtrl, pRuntimeClassButton, dwItemData);

  //Setup the correct edit type in the item data
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  pItemData->m_Type = CTreeOptionsItemData::FolderBrowser;

  return bSuccess;
}

CString CTreeOptionsCtrl::GetFolderEditText(HTREEITEM hItem) const
{
  //Just call the edit box version as currently there is no difference
  return GetEditText(hItem);
}

void CTreeOptionsCtrl::SetFolderEditText(HTREEITEM hItem, const CString& sEditText)
{
  //Just call the edit box version as currently there is no difference
  SetEditText(hItem, sEditText);
}

void CTreeOptionsCtrl::CreateSpinCtrl(CRuntimeClass* pRuntimeClassSpinCtrl, CRect rItem, CRect /*rText*/, CRect rPrimaryControl)
{
  //Validate our parameters
  AFXASSUME(pRuntimeClassSpinCtrl);
  
  if (pRuntimeClassSpinCtrl->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsSpinCtrl)))
  {
    //work out the rect this secondary control
    CRect r;
    r.top = rPrimaryControl.top;
    r.bottom = rPrimaryControl.bottom;
    r.left = rPrimaryControl.right;
    r.right = rItem.right;

    //Create the new spin control
    AFXASSUME(m_pSpin);
    m_pSpin->SetEditBuddy(m_pEdit);
    
    //Create the spin control
    VERIFY(m_pSpin->Create(m_pSpin->GetWindowStyle(), r, this, TREE_OPTIONS_SPINCTRL_ID));

    //Setup the buddy and the default range
    m_pSpin->SetBuddy(m_pEdit);
    int nLower = 0;
    int nUpper = 0;
    m_pSpin->GetDefaultRange(nLower, nUpper);
    m_pSpin->SetRange32(nLower, nUpper);

    //set the font the edit box should use based on the font in the tree control
    m_pSpin->SetFont(&m_Font);
  }
  else
    ASSERT(FALSE); //Your class must be derived from CTreeOptionsSpinCtrl
}

void CTreeOptionsCtrl::CreateBrowseButton(CRuntimeClass* pRuntimeClassBrowseButton, CRect rItem, CRect rText)
{
  //Validate our parameters
  AFXASSUME(pRuntimeClassBrowseButton);
  
  if (pRuntimeClassBrowseButton->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)))
  {
    if (m_pMultiLineEditFrameWnd)
    {
      //work out the rect for the button
      CRect rEdit;
      m_pMultiLineEditFrameWnd->GetWindowRect(&rEdit);
      ScreenToClient(&rEdit);
      CRect r;
      r.top = rItem.top;
      r.bottom = rItem.bottom;
      r.right = rItem.right;
      r.left = r.right - m_pButton->GetWidth(); //Ask the browse button for the width to use

      //Create the new browse button
      ASSERT(m_pButton);
      m_pButton->SetFrameBuddy(m_pMultiLineEditFrameWnd);
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, TREE_OPTIONS_BROWSEBUTTONCTRL_ID));
      m_pButton->SetOwner(m_pMultiLineEditFrameWnd);
    }
    else if (m_pEdit)
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
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, TREE_OPTIONS_BROWSEBUTTONCTRL_ID));
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
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, TREE_OPTIONS_BROWSEBUTTONCTRL_ID));
      m_pButton->SetOwner(m_pCombo);
    }
    else
    {
      //work out the rect for the button
      CRect r;
      r.top = rText.top;
      r.bottom = rText.bottom;
      r.right = rItem.right;
      r.left = r.right - m_pButton->GetWidth(); //Ask the browse button for the width to use

      //Create the browse button
      ASSERT(m_pButton);
      VERIFY(m_pButton->Create(m_pButton->GetCaption(), m_pButton->GetWindowStyle(), r, this, TREE_OPTIONS_BROWSEBUTTONCTRL_ID));
    }

    //set the font the edit box should use based on the font in the tree control
    m_pButton->SetFont(&m_Font);
  }
  else
    ASSERT(FALSE); //Your class must be derived from CTreeOptionsBrowseButton
}

CString CTreeOptionsCtrl::GetEditText(HTREEITEM hItem) const
{
  //Just call the combo box version as currently there is no difference
  return GetComboText(hItem);
}

void CTreeOptionsCtrl::SetEditText(HTREEITEM hItem, const CString& sEditText)
{
  //Just call the combo box version as currently there is no difference
  SetComboText(hItem, sEditText);
}

BOOL CTreeOptionsCtrl::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);

  if (!m_bDoNotDestroy)
  {
    //Destroy the old child control if need be
    if (m_hControlItem)
    {
      UpdateTreeControlValueFromChildControl(m_hControlItem);
      DestroyOldChildControl();
      m_hControlItem = NULL;
    }

    //Create the new control if need be
    if (pNMTreeView->itemNew.hItem != NULL)
    {
      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(pNMTreeView->itemNew.hItem));
      if (pItemData && pItemData->m_pRuntimeClass1)
        CreateNewChildControl(pNMTreeView->itemNew.hItem);
    }
  }

	*pResult = 0;

  return FALSE;
}

BOOL CTreeOptionsCtrl::OnItemExpanding(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  //Clean up any controls currently open (assuming it is a standard
  //scroll message and not from one of our spin controls)
  if (m_hControlItem)
  {
    UpdateTreeControlValueFromChildControl(m_hControlItem);
    DestroyOldChildControl();
  }

	*pResult = 0;

  return FALSE;
}

void CTreeOptionsCtrl::Clear()
{
  m_bDoNotDestroy = TRUE;
  HTREEITEM hRoot = GetRootItem();
  m_hControlItem = NULL;
	if (hRoot) 
    MemDeleteAllItems(hRoot);
  m_bDoNotDestroy = FALSE;
}

void CTreeOptionsCtrl::MemDeleteAllItems(HTREEITEM hParent) 
{ 
	HTREEITEM hItem = hParent; 
	while (hItem) 
	{ 
		HTREEITEM hNextItem = CTreeCtrl::GetNextItem(hItem, TVGN_NEXT); 
		if (ItemHasChildren(hItem)) 
			MemDeleteAllItems(GetChildItem(hItem)); 

		CTreeOptionsItemData* pItem = reinterpret_cast<CTreeOptionsItemData*>(CTreeCtrl::GetItemData(hItem)); 
		if (pItem) 
			delete pItem; 
    SetItemData(hItem, 0);  

    //let the base class do its thing
		CTreeCtrl::DeleteItem(hItem); 

    //Move on to the next item
		hItem = hNextItem; 
	} 
}

void CTreeOptionsCtrl::OnDestroy() 
{
  //Destroy the old combo or edit box if need be
  DestroyOldChildControl();

  //Delete all the items ourselves, rather than calling CTreeCtrl::DeleteAllItems
  Clear();

  //Let the parent class do its thing
	CTreeCtrl::OnDestroy();
}

BOOL CTreeOptionsCtrl::DeleteAllItems()
{
  Clear();

  //Let the base class do its thing
	return CTreeCtrl::DeleteAllItems();
}

void CTreeOptionsCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  if (!(pScrollBar && pScrollBar->IsKindOf(RUNTIME_CLASS(CTreeOptionsSpinCtrl))))
  {
    //Clean up any controls currently open we used (assuming it is a standard
    //scroll message and not from one of our spin controls)
    if (m_hControlItem)
    {
      UpdateTreeControlValueFromChildControl(m_hControlItem);
      DestroyOldChildControl();
    }

    //Let the parent class do its thing
	  CTreeCtrl::OnVScroll(nSBCode, nPos, pScrollBar);
  }
}

void CTreeOptionsCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
{
  //Clean up any controls currently open we used
  if (m_hControlItem)
  {
    UpdateTreeControlValueFromChildControl(m_hControlItem);
    DestroyOldChildControl();
  }
	
  //Let the parent class do its thing
	CTreeCtrl::OnHScroll(nSBCode, nPos, pScrollBar);
}

BOOL CTreeOptionsCtrl::OnClick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  //If the mouse was over the label or icon and the item is a combo box
  //or edit box and editing is currently not active then create the 
  //new control
  UINT uFlags=0;
  CPoint point = GetCurrentMessage()->pt;
  ScreenToClient(&point);
	HTREEITEM hItem = HitTest(point, &uFlags);
  if (hItem)
  {
    CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
	  if ((uFlags & TVHT_ONITEM) && pItemData && pItemData->m_pRuntimeClass1 && m_hControlItem == NULL)
      CreateNewChildControl(hItem);

    //Auto select the control if configured to do so
    if (m_bAutoSelect)
      PostMessage(WM_TOC_SETFOCUS_TO_CHILD);
  }
	
	*pResult = 0;

  return FALSE;
}

void CTreeOptionsCtrl::OnCBNKillFocus()
{
  HandleLoseFocusLogic(GetFocus());
}

void CTreeOptionsCtrl::HandleLoseFocusLogic(CWnd* pNewWnd)
{
  //Clean up any controls currently open if we are losing focus to something else
  BOOL bForeignWnd = (m_hControlItem && (pNewWnd != m_pCombo) && (pNewWnd != m_pEdit) && 
                      (pNewWnd != m_pDateTime) && (pNewWnd != m_pIPAddress) && 
                      (pNewWnd != m_pButton) && (pNewWnd != m_pMultiLineEditFrameWnd) &&
                      (pNewWnd != this));
  if (bForeignWnd && m_pCombo)
    bForeignWnd = !m_pCombo->IsRelatedWnd(pNewWnd);
  if (bForeignWnd && m_pDateTime)
    bForeignWnd = !m_pDateTime->IsRelatedWnd(pNewWnd);
  if (bForeignWnd && m_pIPAddress)
    bForeignWnd = !m_pIPAddress->IsRelatedWnd(pNewWnd);

  if (bForeignWnd)
  {
    UpdateTreeControlValueFromChildControl(GetSelectedItem());
    DestroyOldChildControl();
  }
}

void CTreeOptionsCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //Call the helper function which does the main work
  HandleLoseFocusLogic(pNewWnd);

  //Let the parent class do its thing
  CTreeCtrl::OnKillFocus(pNewWnd);
}

void CTreeOptionsCtrl::HandleChildControlLosingFocus()
{
  //Clean up any controls currently open we used
  //if we are losing focus to something else
  UpdateTreeControlValueFromChildControl(GetSelectedItem());
  DestroyOldChildControl();
}

CRect CTreeOptionsCtrl::GetItemIconPosition(const CRect& rItemIcon)
{
  //By default use an icon depth and width of 16. Derived classes are free to set their own depth
  //and width of icon to use. Please note that the rect returned should be less than the size of 
  //an image in the image list used as otherwise you will end up drawing over the boundary for the icon. 
  //The reason for this function is by default, the image list used is 19 pixels deep to ensure there is 
  //no vertical clipping of child controls, but using a depth of 19 pixels for the radio buttons and 
  //check boxes on a non XP themed control instance causes the icons to look "not quite right" because 
  //the GDI support function DrawFrameControl was optimized to work at the common size of 16*16. 
  CRect r;
  int nMidDown = (rItemIcon.Height() / 2) + rItemIcon.top;
  int nMidAcross = (rItemIcon.Width() / 2) + rItemIcon.left;

  r.top = nMidDown - 8;
  r.bottom = r.top + 16;
  r.left = nMidAcross - 8;
  r.right = r.left + 16;

  return r;
}

BOOL CTreeOptionsCtrl::GetItemIconOffset(int left, int right, int y, int& nStartIcon, int& nEndIcon)
{
  //What will be the return value
  BOOL bSuccess = FALSE;

  //Set the out parameters to default values
  nStartIcon = -1;
  nEndIcon = -1;

  CImageList* pImageList = GetImageList(TVSIL_NORMAL);
  if (pImageList)
  {
    for (int i=left; (i<right) && ((nStartIcon == -1) || (nEndIcon == -1)); i++)
    {
      UINT nFlags = 0;
      HitTest(CPoint(i, y), &nFlags);

      BOOL bOnIcon = ((nFlags & TVHT_ONITEMICON) != 0);
      if (bOnIcon && (nStartIcon == -1))
        nStartIcon = i;

      if ((nStartIcon != -1) && !bOnIcon && (nEndIcon == -1))
        nEndIcon = i;
    }

    if ((nStartIcon != -1) && (nEndIcon == -1))
    {
      int cx = 0;
      int cy = 0;
      bSuccess = ImageList_GetIconSize(pImageList->m_hImageList, &cx, &cy);
      nEndIcon = nStartIcon + cx;
    }
    else if ((nStartIcon == -1) && (nEndIcon != -1))
    {
      int cx = 0;
      int cy = 0;
      bSuccess = ImageList_GetIconSize(pImageList->m_hImageList, &cx, &cy);
      nStartIcon = nEndIcon - cx;
    }
    else if ((nStartIcon != -1) && (nEndIcon != -1))
    {
      int cx = 0;
      int cy = 0;
      bSuccess = ImageList_GetIconSize(pImageList->m_hImageList, &cx, &cy);
      if (bSuccess)
      {  
        if ((nEndIcon - nStartIcon) < cx)
          nStartIcon = nEndIcon - 16;
      }
    }
  }

  return bSuccess;
}

BOOL CTreeOptionsCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMTVCUSTOMDRAW* pCustomDraw = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);
  switch (pCustomDraw->nmcd.dwDrawStage) 
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW; //Tell the control that we are interested in item notifications
      break;
		}	
    case CDDS_ITEMPREPAINT:
    {
      //Just let me know about post painting
      *pResult = CDRF_NOTIFYPOSTPAINT;
      break;
    }
    case CDDS_ITEMPOSTPAINT:
    {
      HTREEITEM hItem = reinterpret_cast<HTREEITEM>(pCustomDraw->nmcd.dwItemSpec);

      CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
      if (pItemData)
      {
        //Work out the correct rect for the image
        CRect r;
        r.top = pCustomDraw->nmcd.rc.top;
        r.bottom = pCustomDraw->nmcd.rc.bottom;

        int nStartIcon;
        int nEndIcon;
        if (GetItemIconOffset(pCustomDraw->nmcd.rc.left, pCustomDraw->nmcd.rc.right, r.top+1, nStartIcon, nEndIcon))
        {
          //Allow for the indent
          r.left = nStartIcon;
          r.right = nEndIcon;

          //Create an MFC device context
          CDC dc;
          dc.Attach(pCustomDraw->nmcd.hdc);

          if (pItemData->m_Type == CTreeOptionsItemData::ColorBrowser && pItemData->m_bDrawColorForIcon)
          {
            //Draw the icon of the tree view item using the specified color
            dc.FillSolidRect(&r, GetColor(hItem));
          }
          else if (pItemData->m_Type == CTreeOptionsItemData::CheckBox)
          {
            //Determine if we should use XP themes
            HTHEME hTheme = NULL;
            if (m_bThemed && m_hUXTheme)
            {
              ASSERT(m_lpfnOpenThemeData);
              hTheme = m_lpfnOpenThemeData(NULL, L"BUTTON");
            }

            //Validate the function pointers if we are using XP themes
            if (hTheme)
            {
              ASSERT(m_lpfnDrawThemeBackground);
              ASSERT(m_lpfnDrawThemeEdge);
            }

            //Always start with a solid background
            if (pItemData->m_bSemiChecked)
              dc.FillSolidRect(&r, GetSysColor(COLOR_3DFACE));
            else  
              dc.FillSolidRect(&r, GetSysColor(COLOR_WINDOW));

            //Get the position for the radio button pixels
            r = GetItemIconPosition(r);

            if (pItemData->m_bEnabled)
            {
              if (pItemData->m_bChecked)
              {
                if (hTheme)
                {  
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_CHECKBOX, CBS_CHECKEDNORMAL, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_CHECKBOX, CBS_CHECKEDNORMAL, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleCheckBoxes ? DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_FLAT : DFCS_BUTTONCHECK | DFCS_CHECKED);
              }
              else
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, &r, NULL);
 						      m_lpfnDrawThemeEdge(hTheme, dc, BP_CHECKBOX, CBS_UNCHECKEDNORMAL, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleCheckBoxes ? DFCS_BUTTONCHECK | DFCS_FLAT : DFCS_BUTTONCHECK);
              }
            }
            else
            {
              if (pItemData->m_bChecked)
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_CHECKBOX, CBS_CHECKEDDISABLED, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_CHECKBOX, CBS_CHECKEDDISABLED, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleCheckBoxes ? DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_INACTIVE | DFCS_FLAT : DFCS_BUTTONCHECK | DFCS_CHECKED | DFCS_INACTIVE);
              }
              else
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_CHECKBOX, CBS_UNCHECKEDDISABLED, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_CHECKBOX, CBS_UNCHECKEDDISABLED, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleCheckBoxes ? DFCS_BUTTONCHECK | DFCS_INACTIVE | DFCS_FLAT : DFCS_BUTTONCHECK | DFCS_INACTIVE);
              }
            }

            if (hTheme)
            {
              ASSERT(m_lpfnCloseThemeData);
              m_lpfnCloseThemeData(hTheme);
            }
          }
          else if (pItemData->m_Type == CTreeOptionsItemData::RadioButton)
          {
            //Determine if we should use XP themes
            HTHEME hTheme = NULL;
            if (m_bThemed && m_hUXTheme)
            {
              ASSERT(m_lpfnOpenThemeData);
              hTheme = m_lpfnOpenThemeData(NULL, L"BUTTON");
            }

            //Validate the function pointers if we are using XP themes
            if (hTheme)
            {
              ASSERT(m_lpfnDrawThemeBackground);
              ASSERT(m_lpfnDrawThemeEdge);
            }

            //Always start with a solid background
            dc.FillSolidRect(&r, GetSysColor(COLOR_WINDOW));

            //Get the position for the radio button pixels
            r = GetItemIconPosition(r);

            if (pItemData->m_bEnabled)
            {
              if (pItemData->m_bChecked)
              {
                if (hTheme)
                {  
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_RADIOBUTTON, CBS_CHECKEDNORMAL, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_RADIOBUTTON, CBS_CHECKEDNORMAL, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleRadioButtons ? DFCS_BUTTONRADIO | DFCS_CHECKED | DFCS_FLAT : DFCS_BUTTONRADIO | DFCS_CHECKED);
              }
              else
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_RADIOBUTTON, CBS_UNCHECKEDNORMAL, &r, NULL);
 						      m_lpfnDrawThemeEdge(hTheme, dc, BP_RADIOBUTTON, CBS_UNCHECKEDNORMAL, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleRadioButtons ? DFCS_BUTTONRADIO | DFCS_FLAT : DFCS_BUTTONRADIO);
              }
            }
            else
            {
              if (pItemData->m_bChecked)
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_RADIOBUTTON, CBS_CHECKEDDISABLED, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_RADIOBUTTON, CBS_CHECKEDDISABLED, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleRadioButtons ? DFCS_BUTTONRADIO | DFCS_CHECKED | DFCS_INACTIVE | DFCS_FLAT : DFCS_BUTTONRADIO | DFCS_CHECKED | DFCS_INACTIVE);
              }
              else
              {
                if (hTheme)
                {
						      m_lpfnDrawThemeBackground(hTheme, dc, BP_RADIOBUTTON, CBS_UNCHECKEDDISABLED, &r, NULL);
						      m_lpfnDrawThemeEdge(hTheme, dc, BP_RADIOBUTTON, CBS_UNCHECKEDDISABLED, &r, 0, 0, NULL);
                }
                else
                  dc.DrawFrameControl(&r, DFC_BUTTON, m_bFlatStyleRadioButtons ? DFCS_BUTTONRADIO | DFCS_INACTIVE | DFCS_FLAT : DFCS_BUTTONRADIO | DFCS_INACTIVE);
              }
            }

            if (hTheme)
            {
              ASSERT(m_lpfnCloseThemeData);
              m_lpfnCloseThemeData(hTheme);
            }
          }

          //Release the DC before we return
          dc.Detach();
        }
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

BOOL CTreeOptionsCtrl::AddDateTime(HTREEITEM hItem, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData)
{
  //Add the date time control as the primary control
  BOOL bSuccess = AddComboBox(hItem, pRuntimeClassDateTime, dwItemData);

  //Setup the item type
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_Type = CTreeOptionsItemData::DateTimeCtrl;
  
  return bSuccess;
}

void CTreeOptionsCtrl::GetDateTime(HTREEITEM hItem, SYSTEMTIME& st) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  memcpy(&st, &pItemData->m_DateTime, sizeof(SYSTEMTIME));
}

void CTreeOptionsCtrl::SetDateTime(HTREEITEM hItem, const SYSTEMTIME& st)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  memcpy(&pItemData->m_DateTime, &st, sizeof(SYSTEMTIME));

  //Also update the text while we are at it
  CTreeOptionsDateCtrl* pTempDateTime = static_cast<CTreeOptionsDateCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
  AFXASSUME(pTempDateTime);
  ASSERT(pTempDateTime->IsKindOf(RUNTIME_CLASS(CTreeOptionsDateCtrl)));  //Your class must be derived from CTreeOptionsDateCtrl
  CString sDateTime(pTempDateTime->GetDisplayText(st));
  SetEditText(hItem, sDateTime);
  delete pTempDateTime;
}

BOOL CTreeOptionsCtrl::AddIPAddress(HTREEITEM hItem, CRuntimeClass* pRuntimeClassDateTime, DWORD_PTR dwItemData)
{
  //Add the date time control as the primary control
  BOOL bSuccess = AddComboBox(hItem, pRuntimeClassDateTime, dwItemData);

  //Setup the item type
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_Type = CTreeOptionsItemData::IPAddressCtrl;
  
  return bSuccess;
}

DWORD CTreeOptionsCtrl::GetIPAddress(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  return pItemData->m_dwIPAddress;
}

void CTreeOptionsCtrl::SetIPAddress(HTREEITEM hItem, DWORD dwAddress)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_dwIPAddress = dwAddress;

  //Also update the text while we are at it
  CTreeOptionsIPAddressCtrl* pTempIPAddress = static_cast<CTreeOptionsIPAddressCtrl*>(pItemData->m_pRuntimeClass1->CreateObject());
  AFXASSUME(pTempIPAddress);
  ASSERT(pTempIPAddress->IsKindOf(RUNTIME_CLASS(CTreeOptionsIPAddressCtrl)));  //Your class must be derived from CTreeOptionsIPAddressCtrl
  CString sIPAddress(pTempIPAddress->GetDisplayText(dwAddress));
  SetEditText(hItem, sIPAddress);
  delete pTempIPAddress;
}

BOOL CTreeOptionsCtrl::AddOpaque(HTREEITEM hItem, CRuntimeClass* pRuntimeClass1, CRuntimeClass* pRuntimeClass2, DWORD_PTR dwItemData)
{
  //Add the first class
  BOOL bSuccess = AddComboBox(hItem, pRuntimeClass1, dwItemData);

  //Add the second class
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  ASSERT(pItemData->m_pRuntimeClass1);
  ASSERT(pItemData->m_pRuntimeClass2 == NULL);
  pItemData->m_pRuntimeClass2 = pRuntimeClass2;

  //Setup the browser type
  pItemData->m_Type = CTreeOptionsItemData::OpaqueBrowser;
  
  return bSuccess;
}

DWORD_PTR CTreeOptionsCtrl::GetOpaque(HTREEITEM hItem) const
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  return pItemData->m_dwItemData;
}

void CTreeOptionsCtrl::SetOpaque(HTREEITEM hItem, DWORD_PTR dwItemData)
{
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(GetItemData(hItem));
  AFXASSUME(pItemData);
  pItemData->m_dwItemData = dwItemData;
}

HTREEITEM CTreeOptionsCtrl::CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
  //Get the details of the item to copy
  TV_INSERTSTRUCT tvstruct;
  tvstruct.item.hItem = hItem;
  tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
  GetItem(&tvstruct.item);
  CString sText(GetItemText(hItem));
  tvstruct.item.cchTextMax = sText.GetLength();
  tvstruct.item.pszText = sText.GetBuffer(tvstruct.item.cchTextMax);
  if (tvstruct.item.lParam)
    tvstruct.item.lParam = reinterpret_cast<LPARAM>(new CTreeOptionsItemData(*(reinterpret_cast<CTreeOptionsItemData*>(tvstruct.item.lParam))));

  //Insert the item at the proper location
  tvstruct.hParent = htiNewParent;
  tvstruct.hInsertAfter = htiAfter;
  tvstruct.item.mask |= TVIF_TEXT;
  HTREEITEM hNewItem = InsertItem(&tvstruct);

  //Don't forget to release the CString buffer  
  sText.ReleaseBuffer();

  return hNewItem;
}

HTREEITEM CTreeOptionsCtrl::CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
  HTREEITEM hNewItem = CopyItem(htiBranch, htiNewParent, htiAfter);
  HTREEITEM hChild = GetChildItem(htiBranch);
  while (hChild != NULL)
  {
    //recursively transfer all the items
    CopyBranch(hChild, hNewItem);
    hChild = GetNextSiblingItem(hChild);
  }
  return hNewItem;
}

void CTreeOptionsCtrl::SetUseThemes(BOOL bTheme)
{
  //Hive away the setting into a member variable
  m_bThemed = bTheme;

  //Force an update if the control has been created
  if (m_hWnd)
  {
    Invalidate();
    UpdateWindow();
  }
}

void CTreeOptionsCtrl::SetFlatStyleRadioButtons(BOOL bFlat)
{
  //Hive away the setting into a member variable
  m_bFlatStyleRadioButtons = bFlat;

  //Force an update if the control has been created
  if (m_hWnd)
  {
    Invalidate();
    UpdateWindow();
  }
}

void CTreeOptionsCtrl::SetFlatStyleCheckBoxes(BOOL bFlat)
{
  //Hive away the setting into a member variable
  m_bFlatStyleCheckBoxes = bFlat;

  //Force an update if the control has been created
  if (m_hWnd)
  {
    Invalidate();
    UpdateWindow();
  }
}


IMPLEMENT_DYNCREATE(CTreeOptionsComboEdit, CEdit)

CTreeOptionsComboEdit::CTreeOptionsComboEdit() : m_pTreeCtrl(NULL)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsComboEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
END_MESSAGE_MAP()

void CTreeOptionsComboEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pTreeCtrl);
    if (m_pTreeCtrl->m_pButton)
      m_pTreeCtrl->m_pButton->SetFocus();  
    else
		  m_pTreeCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

UINT CTreeOptionsComboEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB;
}


IMPLEMENT_DYNCREATE(CTreeOptionsCombo, CComboBox)

CTreeOptionsCombo::CTreeOptionsCombo() : m_pTreeCtrl(NULL),
                                         m_pButtonCtrl(NULL),
                                         m_hTreeCtrlItem(NULL),
                                         m_bDoNotDestroyUponLoseFocus(FALSE)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsCombo, CComboBox)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WM_CTLCOLOREDIT, OnCtlColor)
	ON_WM_DESTROY()
END_MESSAGE_MAP()

UINT CTreeOptionsCombo::OnGetDlgCode() 
{
	return CComboBox::OnGetDlgCode() | DLGC_WANTTAB;
}

void CTreeOptionsCombo::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pTreeCtrl);
    if (m_pTreeCtrl->m_pButton)
      m_pTreeCtrl->m_pButton->SetFocus();  
    else
		  m_pTreeCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CComboBox::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CTreeOptionsCombo::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST;
}

int CTreeOptionsCombo::GetDropDownHeight()
{
  return 100;
}

BOOL CTreeOptionsCombo::IsRelatedWnd(CWnd* pChild)
{
  BOOL bRelated = FALSE;
  if (pChild)
  {
    CWnd* pWnd = pChild;
    while (pWnd && !bRelated)
    {
      pWnd = pWnd->GetParent();
      bRelated = (pWnd == this);
    }
  }

  if (!bRelated)
  {
    AFXASSUME(m_pTreeCtrl);
    bRelated = (pChild == m_pTreeCtrl->m_pButton); 
    if (!bRelated)
      bRelated = (pChild == m_pTreeCtrl->m_pSpin); 
  }

  return bRelated;
}

void CTreeOptionsCombo::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  ASSERT(m_pTreeCtrl);
	if ((pNewWnd != m_pButtonCtrl) && !IsRelatedWnd(pNewWnd))
  {
    ASSERT(m_pTreeCtrl);
    m_pTreeCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CComboBox::OnKillFocus(pNewWnd);
	}
}

LRESULT CTreeOptionsCombo::OnCtlColor(WPARAM /*wParam*/, LPARAM lParam)
{
  //Subclass the combo edit
  if (m_Edit.GetSafeHwnd() == NULL)
    m_Edit.SubclassWindow(reinterpret_cast<HWND>(lParam));
  
  //Let the base class do its thing
  return Default();
}

void CTreeOptionsCombo::OnDestroy()
{
  if (m_Edit.GetSafeHwnd() != NULL)
    m_Edit.UnsubclassWindow();

  //Let the base class do its thing
  CComboBox::OnDestroy();
}


IMPLEMENT_DYNCREATE(CTreeOptionsFontNameCombo, CTreeOptionsCombo)

CTreeOptionsFontNameCombo::CTreeOptionsFontNameCombo()
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsFontNameCombo, CTreeOptionsCombo)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CTreeOptionsFontNameCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  //Let the parent class do its thing
	if (CTreeOptionsCombo::OnCreate(lpCreateStruct) == -1)
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

int CTreeOptionsFontNameCombo::EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* /*lptm*/, DWORD /*dwType*/)
{
  //Add the font name to the combo box
  AddString(lplf->lfFaceName);

  return 1; //To continue enumeration
}

int CALLBACK CTreeOptionsFontNameCombo::_EnumFontProc(CONST LOGFONT* lplf, CONST TEXTMETRIC* lptm, DWORD dwType, LPARAM lpData)
{ 
  //Convert from the SDK world to the C++ world
  CTreeOptionsFontNameCombo* pThis = reinterpret_cast<CTreeOptionsFontNameCombo*>(lpData);
  AFXASSUME(pThis);
  return pThis->EnumFontProc(lplf, lptm, dwType);
} 

DWORD CTreeOptionsFontNameCombo::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWNLIST | CBS_SORT;
}


IMPLEMENT_DYNCREATE(CTreeOptionsDateCtrl, CDateTimeCtrl)

CTreeOptionsDateCtrl::CTreeOptionsDateCtrl() : m_pTreeCtrl(NULL),
                                               m_bDoNotDestroyUponLoseFocus(FALSE),
                                               m_hTreeCtrlItem(NULL)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsDateCtrl, CDateTimeCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CTreeOptionsDateCtrl::OnGetDlgCode() 
{
	return CDateTimeCtrl::OnGetDlgCode() | DLGC_WANTTAB;
}

void CTreeOptionsDateCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pTreeCtrl);
		m_pTreeCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CTreeOptionsDateCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | DTS_SHORTDATEFORMAT;
}

void CTreeOptionsDateCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //Let the parent class do its thing
	CDateTimeCtrl::OnKillFocus(pNewWnd);

  //update the list control and close this window
  if (!IsRelatedWnd(pNewWnd))
  {
    ASSERT(m_pTreeCtrl);
    m_pTreeCtrl->HandleChildControlLosingFocus();
  }
}

CString CTreeOptionsDateCtrl::GetDisplayText(const SYSTEMTIME& st)
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

BOOL CTreeOptionsDateCtrl::IsRelatedWnd(CWnd* pChild)
{
  return (GetMonthCalCtrl() == pChild);
}


IMPLEMENT_DYNCREATE(CTreeOptionsTimeCtrl, CTreeOptionsDateCtrl)

CTreeOptionsTimeCtrl::CTreeOptionsTimeCtrl()
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsTimeCtrl, CTreeOptionsDateCtrl)
	ON_WM_CREATE()
END_MESSAGE_MAP()

CString CTreeOptionsTimeCtrl::GetDisplayText(const SYSTEMTIME& st)
{
  //What will be the return value from this function
  CString sDisplayText;

  //First ask the API to give us the buffer size
  int nChars = ::GetTimeFormat(LOCALE_USER_DEFAULT, 0, &st, NULL, NULL, 0);
  BOOL bWantStop = FALSE;
  while (nChars && !bWantStop)
  {
    //alloc the buffer to the required size
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

DWORD CTreeOptionsTimeCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE | DTS_TIMEFORMAT;
}


IMPLEMENT_DYNCREATE(CTreeOptionsIPAddressCtrl, CIPAddressCtrl)

CTreeOptionsIPAddressCtrl::CTreeOptionsIPAddressCtrl() : m_pTreeCtrl(NULL),
                                                         m_bDoNotDestroyUponLoseFocus(FALSE),
                                                         m_hTreeCtrlItem(NULL)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsIPAddressCtrl, CIPAddressCtrl)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CTreeOptionsIPAddressCtrl::OnGetDlgCode() 
{
	return CIPAddressCtrl::OnGetDlgCode() | DLGC_WANTTAB;
}

void CTreeOptionsIPAddressCtrl::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pTreeCtrl);
		m_pTreeCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CIPAddressCtrl::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CTreeOptionsIPAddressCtrl::GetWindowStyle()
{
  return WS_CHILD | WS_VISIBLE;
}

void CTreeOptionsIPAddressCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //Let the parent class do its thing
	CIPAddressCtrl::OnKillFocus(pNewWnd);

  if (!IsRelatedWnd(pNewWnd))
  {
    //update the list control and close this window
    ASSERT(m_pTreeCtrl);
    m_pTreeCtrl->HandleChildControlLosingFocus();
  }
}

CString CTreeOptionsIPAddressCtrl::GetDisplayText(DWORD dwAddress)
{
  CString sAddress;
  sAddress.Format(_T("%d.%d.%d.%d"), (dwAddress & 0xFF000000) >> 24, 
                  (dwAddress & 0xFF0000) >> 16, (dwAddress & 0xFF00) >> 8,
                  (dwAddress & 0xFF));
  return sAddress;
}

BOOL CTreeOptionsIPAddressCtrl::IsRelatedWnd(CWnd* pChild)
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


IMPLEMENT_DYNCREATE(CTreeOptionsBooleanCombo, CTreeOptionsCombo)

CTreeOptionsBooleanCombo::CTreeOptionsBooleanCombo()
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsBooleanCombo, CTreeOptionsCombo)
	ON_WM_CREATE()
END_MESSAGE_MAP()

int CTreeOptionsBooleanCombo::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
  //Let the parent class do its thing
	if (CTreeOptionsCombo::OnCreate(lpCreateStruct) == -1)
		return -1;
	
  //Add the two boolean strings
  CString sText;
  VERIFY(sText.LoadString(IDS_TREEOPTIONS_TRUE));
  AddString(sText);
  VERIFY(sText.LoadString(IDS_TREEOPTIONS_FALSE));
  AddString(sText);
	
	return 0;
}


IMPLEMENT_DYNCREATE(CTreeOptionsEdit, CEdit)

CTreeOptionsEdit::CTreeOptionsEdit() : m_pTreeCtrl(NULL),
                                       m_bDoNotDestroyUponLoseFocus(FALSE),
                                       m_pButtonCtrl(NULL),
                                       m_hTreeCtrlItem(NULL),
                                       m_pFrameWnd(NULL)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsEdit, CEdit)
	ON_WM_CHAR()
	ON_WM_GETDLGCODE()
	ON_WM_KILLFOCUS()
END_MESSAGE_MAP()

UINT CTreeOptionsEdit::OnGetDlgCode() 
{
	return CEdit::OnGetDlgCode() | DLGC_WANTTAB;
}

void CTreeOptionsEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if (nChar == VK_TAB)
  {
    AFXASSUME(m_pTreeCtrl);
    if (m_pTreeCtrl->m_pButton)
      m_pTreeCtrl->m_pButton->SetFocus();  
    else
		  m_pTreeCtrl->SetFocus();
  }
	else
	{
		//Pass on to the parent since we didn't handle it
		CEdit::OnChar(nChar, nRepCnt, nFlags);
	}
}

DWORD CTreeOptionsEdit::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | ES_LEFT | ES_AUTOHSCROLL;
}

int CTreeOptionsEdit::GetHeight(int nItemHeight)
{
  return max(nItemHeight, 20);
}

CString CTreeOptionsEdit::FormControlText(const CString& sText)
{
  //What will be the return value
  CString sRet;

  int EOLPos = sText.Find(_T("\r"));
  if (EOLPos != -1)
    sRet = sText.Left(EOLPos) + _T("...");
  else
    sRet = sText;

  return sRet;
}

void CTreeOptionsEdit::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  ASSERT(m_pTreeCtrl);
	if (pNewWnd != m_pButtonCtrl)
  {
    AFXASSUME(m_pTreeCtrl);
    m_pTreeCtrl->HandleChildControlLosingFocus();
  }
  else
  {
    //Let the parent class do its thing
	  CEdit::OnKillFocus(pNewWnd);
	}
}

CString CTreeOptionsEdit::GetBrowseForFolderCaption()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_TREEOPTIONS_DEFAULT_FOLDER_CAPTION));
  return sCaption;
}

CString CTreeOptionsEdit::GetBrowseForFileCaption()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_TREEOPTIONS_DEFAULT_FILE_CAPTION));
  return sCaption;
}

CString CTreeOptionsEdit::GetFileExtensionFilter()
{
  CString sCaption;
  VERIFY(sCaption.LoadString(IDS_TREEOPTIONS_DEFAULT_FILE_EXTENSION_FILTER));
  return sCaption;
}

int CALLBACK CTreeOptionsEdit::SHBrowseSetSelProc(HWND hWnd, UINT uMsg, LPARAM /*lParam*/, LPARAM lpData)
{
  if (uMsg == BFFM_INITIALIZED)
    ::SendMessage(hWnd, BFFM_SETSELECTION, TRUE, lpData);

  return 0;
}

void CTreeOptionsEdit::BrowseForFolder(const CString& sInitialFolder)
{
  //Validate our parameters
  AFXASSUME(m_pTreeCtrl);

  //Bring up a standard directory chooser dialog
  TCHAR sDisplayName[_MAX_PATH];
  BROWSEINFO bi;
  bi.hwndOwner = m_pTreeCtrl->GetSafeHwnd();
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

void CTreeOptionsEdit::BrowseForFile(const CString& sInitialFile)
{
  //Validate our parameters
  ASSERT(m_pTreeCtrl);

  //Create the special file save dialog
  CTreeOptionsFileDialog dlg(TRUE, NULL, sInitialFile, OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, GetFileExtensionFilter(), m_pTreeCtrl);

  //Modify the title to the desired value
  CString sCaption(GetBrowseForFileCaption());
  dlg.m_ofn.lpstrTitle = sCaption;
  
  //bring up the dialog and if hit ok set the text in this control to the new filename
  if (dlg.DoModal() == IDOK)
    SetWindowText(dlg.GetPathName());
}


IMPLEMENT_DYNCREATE(CTreeOptionsSpinCtrl, CSpinButtonCtrl)

CTreeOptionsSpinCtrl::CTreeOptionsSpinCtrl() : m_pTreeCtrl(NULL),
                                               m_pEdit(NULL),
                                               m_hTreeCtrlItem(NULL)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsSpinCtrl, CSpinButtonCtrl)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
END_MESSAGE_MAP()

void CTreeOptionsSpinCtrl::SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl) 
{ 
  //Validate our parameters
  ASSERT(pTreeCtrl);
  
  m_pTreeCtrl = pTreeCtrl; 
}

void CTreeOptionsSpinCtrl::SetEditBuddy(CTreeOptionsEdit* pEdit) 
{ 
  //Validate our parameters
  ASSERT(pEdit);
  
  m_pEdit = pEdit; 
}

DWORD CTreeOptionsSpinCtrl::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | UDS_ARROWKEYS | UDS_SETBUDDYINT | UDS_NOTHOUSANDS | UDS_ALIGNRIGHT;
}

void CTreeOptionsSpinCtrl::GetDefaultRange(int &nLower, int& nUpper)
{
  nLower = 0;
  nUpper = 100;
}

void CTreeOptionsSpinCtrl::OnKillFocus(CWnd* pNewWnd) 
{
  //update the list control and close this window
  AFXASSUME(m_pTreeCtrl);
	if (pNewWnd != m_pEdit)
    m_pTreeCtrl->HandleChildControlLosingFocus();
  else
  {
    //Let the parent class do its thing
	  CSpinButtonCtrl::OnKillFocus(pNewWnd);
	}
}


IMPLEMENT_DYNCREATE(CTreeOptionsBrowseButton, CButton)

CTreeOptionsBrowseButton::CTreeOptionsBrowseButton() : m_pTreeCtrl(NULL),
                                                       m_pEdit(NULL),
                                                       m_pCombo(NULL),
                                                       m_hTreeCtrlItem(NULL),
                                                       m_pFrame(NULL)

{
}

BEGIN_MESSAGE_MAP(CTreeOptionsBrowseButton, CButton)
	ON_WM_KILLFOCUS()
	ON_WM_CHAR()
	ON_CONTROL_REFLECT(BN_CLICKED, OnClicked)
END_MESSAGE_MAP()

void CTreeOptionsBrowseButton::SetTreeBuddy(CTreeOptionsCtrl* pTreeCtrl) 
{ 
  //Validate our parameters
  ASSERT(pTreeCtrl);
  
  m_pTreeCtrl = pTreeCtrl; 
}

void CTreeOptionsBrowseButton::SetEditBuddy(CTreeOptionsEdit* pEdit) 
{ 
  //Validate our parameters
  ASSERT(pEdit);
  
  m_pEdit = pEdit; 
}

void CTreeOptionsBrowseButton::SetFrameBuddy(CMultiLineEditFrameWnd* pFrameWnd)
{
  //Validate our parameters
  ASSERT(pFrameWnd);
  
  m_pFrame = pFrameWnd;
}

void CTreeOptionsBrowseButton::SetComboBuddy(CTreeOptionsCombo* pCombo)
{
  //Validate our parameters
  ASSERT(pCombo);
  
  m_pCombo = pCombo;
}

DWORD CTreeOptionsBrowseButton::GetWindowStyle()
{
  return WS_VISIBLE | WS_CHILD | BS_PUSHBUTTON;
}

CString CTreeOptionsBrowseButton::GetCaption()
{
  return _T("...");
}

int CTreeOptionsBrowseButton::GetWidth()
{ 
  //Validate our parameters
  AFXASSUME(m_pTreeCtrl);

  CDC* pDC = m_pTreeCtrl->GetDC();
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

    m_pTreeCtrl->ReleaseDC(pDC);
	}
	else
    nButtonWidth = 20;

  return nButtonWidth;
}

void CTreeOptionsBrowseButton::OnKillFocus(CWnd* pNewWnd) 
{
  //Validate our parameters
  AFXASSUME(m_pTreeCtrl);

  //update the tree control and close this window
  if (m_pEdit)
  {
	  if ((pNewWnd != m_pTreeCtrl->m_pEdit) && !m_pEdit->m_bDoNotDestroyUponLoseFocus)
    {
      ASSERT(m_pTreeCtrl);
      m_pTreeCtrl->HandleChildControlLosingFocus();
    }
    else
    {
      //Let the parent class do its thing
	    CButton::OnKillFocus(pNewWnd);
    }
  }
  else if (m_pCombo)
  {
	  if ((pNewWnd != m_pTreeCtrl->m_pCombo) && !m_pCombo->m_bDoNotDestroyUponLoseFocus)
    {
      ASSERT(m_pTreeCtrl);
      m_pTreeCtrl->HandleChildControlLosingFocus();
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

void CTreeOptionsBrowseButton::OnClicked() 
{
  //Validate our parameters
  AFXASSUME(m_pTreeCtrl);

	//Get the text currently in the control
  HTREEITEM hSelItem = m_pTreeCtrl->GetSelectedItem();
  AFXASSUME(hSelItem);

  //Pull out the item data associated with the selected item
  CTreeOptionsItemData* pItemData = reinterpret_cast<CTreeOptionsItemData*>(m_pTreeCtrl->GetItemData(hSelItem));
  AFXASSUME(pItemData);
  if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsEdit)))
  {
    if (m_pFrame)
      m_pFrame->SetFocus();
    else
    {
      AFXASSUME(m_pEdit);

      //Decide on what dialog to bring up, and call the appropiate virtual function
      if (pItemData->m_Type == CTreeOptionsItemData::FileBrowser)
      {
        m_pEdit->m_bDoNotDestroyUponLoseFocus = TRUE;
        CString sText;
        m_pEdit->GetWindowText(sText);
        m_pEdit->BrowseForFile(sText);
        m_pEdit->m_bDoNotDestroyUponLoseFocus = FALSE;
      }
      else if (pItemData->m_Type == CTreeOptionsItemData::FolderBrowser)
      {
        m_pEdit->m_bDoNotDestroyUponLoseFocus = TRUE;
        CString sText;
        m_pEdit->GetWindowText(sText);
        m_pEdit->BrowseForFolder(sText);
        m_pEdit->m_bDoNotDestroyUponLoseFocus = FALSE;
      }
      else if (pItemData->m_Type == CTreeOptionsItemData::OpaqueBrowser)
        BrowseForOpaque();
      else
        ASSERT(FALSE);
    }
  }
  else if (pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsCombo)))
  {
    ASSERT(m_pCombo);
    
    if (pItemData->m_Type == CTreeOptionsItemData::OpaqueBrowser)
      BrowseForOpaque();
  }
  else
  {
    ASSERT(pItemData->m_pRuntimeClass1->IsDerivedFrom(RUNTIME_CLASS(CTreeOptionsBrowseButton)));

    if (pItemData->m_Type == CTreeOptionsItemData::ColorBrowser)
      BrowseForColor();
    else if (pItemData->m_Type == CTreeOptionsItemData::FontBrowser)
      BrowseForFont();
    else if (pItemData->m_Type == CTreeOptionsItemData::OpaqueBrowser)
      BrowseForOpaque();
    else
      ASSERT(FALSE);
  }
}

void CTreeOptionsBrowseButton::BrowseForColor()
{
  //Validate our parameters
  ASSERT(m_pTreeCtrl);

  //Bring up a standard color selector dialog
  CColorDialog dialog(GetColor());
  dialog.m_cc.Flags |= CC_FULLOPEN;
  if (dialog.DoModal() == IDOK)
  {
    SetColor(dialog.GetColor());
    m_pTreeCtrl->SetColor(m_pTreeCtrl->GetSelectedItem(), m_Color);

    //Ask the tree control to reposition the button if need be  
    m_pTreeCtrl->PostMessage(WM_TOC_REPOSITION_CHILD_CONTROL);
  }
}

void CTreeOptionsBrowseButton::BrowseForFont()
{
  //Validate our parameters
  AFXASSUME(m_pTreeCtrl);

  //Bring up a standard color selector dialog
  CFontDialog dialog(&m_Font);
  if (dialog.DoModal() == IDOK)
  {
    dialog.GetCurrentFont(&m_Font);
    m_pTreeCtrl->SetFontItem(m_pTreeCtrl->GetSelectedItem(), &m_Font);

    //Ask the tree control to reposition the button if need be  
    m_pTreeCtrl->PostMessage(WM_TOC_REPOSITION_CHILD_CONTROL);
  }
}

void CTreeOptionsBrowseButton::BrowseForOpaque()
{
  ASSERT(FALSE); //Derived classes must implement this function if we are editing
                 //an opaque item. The code which "normally" display some CDialog 
                 //derived class to allow the item to be edited and then hive the
                 //data away somehow so that it can show the new value when the
                 //dialog is brought up again. Following is some pseudo code which 
                 //would do this.

  /*
  ASSERT(m_pTreeCtrl);

  //Bring up a our custom opaque editor dialog
  CMyOpaque* pQpaque = reinterpret_cast<CMyOpaque*>(m_pTreeCtrl->GetOpaque(m_hTreeCtrlItem));
  CMyOpaqueDialog dialog;
  dialog.SetOpaque(pOpaque);
  if (dialog.DoModal() == IDOK)
  {
    pOpaque = dialog.GetOpaque();
    m_pTreeCtrl->SetOpaque(m_hTreeCtrlItem, reinterpret_cast<DWORD_PTR>(pOpaque));
    m_pTreeCtrl->SetEditText(m_hTreeCtrlItem, pOpaque->m_sSomeName);
  }
  */
}

void CTreeOptionsBrowseButton::SetColor(COLORREF color) 
{ 
  m_Color = color; 
}

void CTreeOptionsBrowseButton::GetFontItem(LOGFONT* pLogFont)
{
  //Validate our parameters
  ASSERT(pLogFont);
  
  memcpy(pLogFont, &m_Font, sizeof(LOGFONT));
}

void CTreeOptionsBrowseButton::SetFontItem(const LOGFONT* pLogFont)
{
  //Validate our parameters
  ASSERT(pLogFont);
  
  memcpy(&m_Font, pLogFont, sizeof(LOGFONT));
}


IMPLEMENT_DYNAMIC(CTreeOptionsFileDialog, CFileDialog)

CTreeOptionsFileDialog::CTreeOptionsFileDialog(BOOL bOpenFileDialog, LPCTSTR lpszDefExt, LPCTSTR lpszFileName, DWORD dwFlags, LPCTSTR lpszFilter, 
                                               CWnd* pParentWnd) : CFileDialog(bOpenFileDialog, lpszDefExt, lpszFileName, dwFlags, lpszFilter, pParentWnd)
{
}

BEGIN_MESSAGE_MAP(CTreeOptionsFileDialog, CFileDialog)
END_MESSAGE_MAP()

void CTreeOptionsFileDialog::OnInitDone()
{
	CString sText;
  if (!sText.LoadString(IDS_TREEOPTIONS_OK))
    ASSERT(FALSE);
  LPTSTR pszBuffer = sText.GetBuffer(sText.GetLength());

  //modify the text on the IDOK button to ok
  CommDlg_OpenSave_SetControlText(GetParent()->m_hWnd, IDOK, pszBuffer);

  sText.ReleaseBuffer();
}


IMPLEMENT_DYNAMIC(CMultiLineEditFrameWnd, CFrameWnd)

CMultiLineEditFrameWnd::CMultiLineEditFrameWnd() : m_pEdit(NULL)
{
}

BEGIN_MESSAGE_MAP(CMultiLineEditFrameWnd, CFrameWnd)
	ON_WM_SETFOCUS()
	ON_WM_CLOSE()
END_MESSAGE_MAP()

void CMultiLineEditFrameWnd::OnSetFocus(CWnd* pOldWnd) 
{
  //Let the base class do its thing
	CFrameWnd::OnSetFocus(pOldWnd);

  //always focus the child window
  ASSERT(m_pEdit);  
  m_pEdit->SetFocus();
}

void CMultiLineEditFrameWnd::OnClose() 
{
  //Destroy the heap memory we have been using
  ASSERT(m_pEdit);
  delete m_pEdit;
  m_pEdit = NULL;

  //Let the base class do its thing
	CFrameWnd::OnClose();
}


void DDX_TreeCheck(CDataExchange* pDX, int nIDC, HTREEITEM hItem, BOOL& bCheck)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlTreeOptions->GetCheckBox(hItem, bCheck);
  else
    pCtrlTreeOptions->SetCheckBox(hItem, bCheck);
}

void DDX_TreeRadio(CDataExchange* pDX, int nIDC, HTREEITEM hParent, int& nIndex)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
  {
    HTREEITEM hCheckItem = NULL;
    pCtrlTreeOptions->GetRadioButton(hParent, nIndex, hCheckItem);
  }
  else
    pCtrlTreeOptions->SetRadioButton(hParent, nIndex);
}

void DDX_TreeEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlTreeOptions->GetEditText(hItem);
  else
    pCtrlTreeOptions->SetEditText(hItem, sText);
}

void DDX_TreeEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, int& nValue)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
	CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
	AFXASSUME(pCtrlTreeOptions);
	ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));
	
	CString sText;
	if (pDX->m_bSaveAndValidate)
	{
		sText = pCtrlTreeOptions->GetEditText(hItem);
		nValue = _ttoi(sText);
	}
	else
	{
		sText.Format(_T("%d"), nValue);
		pCtrlTreeOptions->SetEditText(hItem, sText);
	}
}

void DDX_TreeCombo(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlTreeOptions->GetComboText(hItem);
  else
    pCtrlTreeOptions->SetComboText(hItem, sText);
}

void DDX_TreeFileEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlTreeOptions->GetFileEditText(hItem);
  else
    pCtrlTreeOptions->SetFileEditText(hItem, sText);
}

void DDX_TreeFolderEdit(CDataExchange* pDX, int nIDC, HTREEITEM hItem, CString& sText)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    sText = pCtrlTreeOptions->GetFolderEditText(hItem);
  else
    pCtrlTreeOptions->SetFolderEditText(hItem, sText);
}

void DDX_TreeColor(CDataExchange* pDX, int nIDC, HTREEITEM hItem, COLORREF& color)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    color = pCtrlTreeOptions->GetColor(hItem);
  else
    pCtrlTreeOptions->SetColor(hItem, color);
}

void DDX_TreeFont(CDataExchange* pDX, int nIDC, HTREEITEM hItem, LOGFONT* pLogFont)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlTreeOptions->GetFontItem(hItem, pLogFont);
  else
    pCtrlTreeOptions->SetFontItem(hItem, pLogFont);
}

void DDX_TreeDateTime(CDataExchange* pDX, int nIDC, HTREEITEM hItem, SYSTEMTIME& st)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    pCtrlTreeOptions->GetDateTime(hItem, st);
  else
    pCtrlTreeOptions->SetDateTime(hItem, st);
}

void DDX_TreeIPAddress(CDataExchange* pDX, int nIDC, HTREEITEM hItem, DWORD& dwAddress)
{
	HWND hWndCtrl = pDX->PrepareCtrl(nIDC);
  CTreeOptionsCtrl* pCtrlTreeOptions = static_cast<CTreeOptionsCtrl*>(CWnd::FromHandlePermanent(hWndCtrl));
  AFXASSUME(pCtrlTreeOptions);
  ASSERT(pCtrlTreeOptions->IsKindOf(RUNTIME_CLASS(CTreeOptionsCtrl)));

  if (pDX->m_bSaveAndValidate)
    dwAddress = pCtrlTreeOptions->GetIPAddress(hItem);
  else
    pCtrlTreeOptions->SetIPAddress(hItem, dwAddress);
}

void DDX_TreeBoolean(CDataExchange* pDX, int nIDC, HTREEITEM hItem, BOOL& bValue)
{
  //Convert from the boolean to a string if we are transfering to the control
  CString sText;
  if (!pDX->m_bSaveAndValidate)
  {
    if (bValue)
      VERIFY(sText.LoadString(IDS_TREEOPTIONS_TRUE));
    else
      VERIFY(sText.LoadString(IDS_TREEOPTIONS_FALSE));
  }

  //Pass the buck to the combo DDX function
  DDX_TreeCombo(pDX, nIDC, hItem, sText);

  //Convert from the string to the boolean if we are transfering from the control
  if (pDX->m_bSaveAndValidate)
  {
    CString sCompare;
    VERIFY(sCompare.LoadString(IDS_TREEOPTIONS_TRUE));
    if (sText == sCompare)
      bValue = TRUE;
    else
      bValue = FALSE;
  }
}

