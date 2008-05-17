// CJComboBoxEx.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Copied from MFC, Visual C++ 6, to extend functionality to Visual C++ 5 
// users, with some ideas taken from Luis Barreira's article 'Classes for new 
// IE4 controls' http://www.codeguru.com/controls/ie4_controls_classes.shtml
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 8/31/99 1:11a $
 * $Revision: 6 $
 * $Archive: /CodeJockey/Include/CJComboBoxEx.h $
 *
 * $History: CJComboBoxEx.h $
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/28/99    Time: 12:39a
 * Updated in $/CodeJockey/Include
 * Final pass for release 6.08.
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
 * User: Kirk Stowell Date: 6/25/99    Time: 12:05p
 * Created in $/CodeJockey/Include
 * Copied from MFC, Visual C++ 6. Added to extend functionality to Visual
 * C++ 5 users.
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJCOMBOBOXEX_H__
#define __CJCOMBOBOXEX_H__

// The CCJComboBoxEx class extends the combo box control by providing support 
// for image lists. By using CCJComboBoxEx to create combo box controls, you no 
// longer need to implement your own image drawing code. Instead, use CCJComboBoxEx 
// to access images from an image list.
class _CJX_EXT_CLASS CCJComboBoxEx : public CComboBox
{
	DECLARE_DYNAMIC(CCJComboBoxEx)

// Construction / destruction
public:

	// Constructs a CCJComboBoxEx object
	//
	CCJComboBoxEx();

	// Destroys a CCJCombboBoxEx object
	//
	virtual ~CCJComboBoxEx();

// Initialization
public:

	// Creates the combo box and attaches it to the CCJComboBoxEx object.
	//
	BOOL Create(
		// Specifies the combination of combo box styles 
		// applied to the combo box.
		DWORD dwStyle,
		// A reference to a CRect object orRECT structure, which 
		// is the position and size of the combo box
		const RECT& rect,
		// A pointer to a CWnd object that is the parent window of 
		// the combo box (usually a CDialog). It must not be NULL
		CWnd* pParentWnd,
		// Specifies the combo box’s control ID.
		UINT nID);

// Operators
public:

	// Removes an item from a ComboBoxEx control.
	// Returns the number of items remaining in the control. If iIndex 
	// is invalid, the function returns CB_ERR. 
	//
	int DeleteItem(
		// Zero-based index of the item to be removed. 
		int iIndex);

	// Retrieves item information for a given ComboBoxEx item
	// Returns nonzero if the operation was successful; otherwise 0.
	//
	BOOL GetItem(
		// A pointer to aCOMBOBOXEXITEM structure that will receive 
		// the item information. 
		COMBOBOXEXITEM* pCBItem);

	// Inserts a new item in a ComboBoxEx control.
	// Returns the index at which the new item was inserted if 
	// successful; otherwise -1
	int InsertItem(
		// A pointer to aCOMBOBOXEXITEM structure that will receive the 
		// item information. This structure contains callback flag values for the item.
		const COMBOBOXEXITEM* pCBItem);

	// Inserts a new item in a ComboBoxEx control.
	// Returns the index at which the new item was inserted if 
	// successful; otherwise -1
	int InsertItem(
		// item index.
		int iItem,
		// string resource.
		UINT nStringID,
		// ammount (in pixels) image is to be indented.
		int iIndent=0,
		// image index.
		int iImage=0,
		// selected image index.
		int iSelectedImage=0,
		// style mask.
		UINT mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_INDENT);

	// Inserts a new item in a ComboBoxEx control.
	// Returns the index at which the new item was inserted if 
	// successful; otherwise -1
	int InsertItem(
		// item index.
		int iItem,
		// item string.
		LPCTSTR lpszItem,
		// ammount (in pixels) image is to be indented.
		int iIndent=0,
		// image index.
		int iImage=0,
		// selected image index.
		int iSelectedImage=0,
		// style mask.
		UINT mask=CBEIF_TEXT|CBEIF_IMAGE|CBEIF_SELECTEDIMAGE|CBEIF_INDENT);

	// Sets the attributes for an item in a ComboBoxEx control. 
	// Returns nonzero if the operation was successful; otherwise 0.
	//
	BOOL SetItem(
		// A pointer to aCOMBOBOXEXITEM structure that will receive the 
		// item information
		const COMBOBOXEXITEM* pCBItem);

// Atrributes
public:

	// Retrieves the extended styles that are in use for a ComboBoxEx 
	// control. Returns the DWORD value that contains the extended styles 
	// that are used for the combo box control.
	//
	DWORD GetExtendedStyle() const;

	// Sets extended styles within a ComboBoxEx control.
	// Returns a DWORD value that contains the extended styles previously 
	// used for the control. 
	//
	DWORD SetExtendedStyle(
		// A DWORD value that indicates which styles in dwExStyles are to 
		// be affected. Only the extended styles in dwExMask will be changed. 
		// All other styles will be maintained as is. If this parameter is 
		// zero, then all of the styles in dwExStyles will be affected
		DWORD dwExMask,
		// A DWORD value that contains the combo box control extended styles 
		// to set for the control
		DWORD dwExStyles);

	// Determines if the user has changed the contents of the ComboBoxEx 
	// edit control by typing.
	// Returns nonzero if the user has typed in the control's 
	// edit box; otherwise 0. 
	BOOL HasEditChanged();

	// Retrieves the handle to the edit control portion of a ComboBoxEx 
	// control. Returns a pointer to a CEdit object.
	//
	CEdit* GetEditCtrl();

	// Retrieves a pointer to the child combo box control.
	// Returns a pointer to a CComboBox object.
	//
	CComboBox* GetComboBoxCtrl();

	// Retrieves a pointer to the image list assigned to a ComboBoxEx
	// control. Returns a pointer to a CImageList object. If it fails, 
	// this member function returns NULL.
	//
	CImageList* GetImageList() const;

	// Sets an image list for a ComboBoxEx control.
	// Returns a pointer to a CImageList object containing the images 
	// previously used by the CCJComboBoxEx control. NULL if no image list 
	//was previously set.
	//
	CImageList* SetImageList(
		// A pointer to a CImageList object containing the images to use with 
		// the CCJComboBoxEx control.
		CImageList* pImageList);

// Un-supported
public:
	
	// Un-supported base class message.
	//
	int Dir(UINT attr, LPCTSTR lpszWildCard);

	// Un-supported base class message.
	//
	int FindString(int nIndexStart, LPCTSTR lpszFind) const;

	// Un-supported base class message.
	//
	int AddString(LPCTSTR lpszString);

	// Un-supported base class message.
	//
	BOOL SetEditSel(int nStartChar, int nEndChar);

	// Un-supported base class message.
	//
	int InsertString(int nIndex, LPCTSTR lpszString);
};

/////////////////////////////////////////////////////////////////////////////
//	Inline Functions
/////////////////////////////////////////////////////////////////////////////

_CJXLIB_INLINE CCJComboBoxEx::CCJComboBoxEx()
	{ }
_CJXLIB_INLINE DWORD CCJComboBoxEx::GetExtendedStyle() const
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_GETEXTENDEDSTYLE, 0, 0); }
_CJXLIB_INLINE DWORD CCJComboBoxEx::SetExtendedStyle(DWORD dwExMask, DWORD dwExStyles)
	{ ASSERT(::IsWindow(m_hWnd)); return (DWORD) ::SendMessage(m_hWnd, CBEM_SETEXTENDEDSTYLE, (DWORD) dwExMask, (LPARAM) dwExStyles); }
_CJXLIB_INLINE BOOL CCJComboBoxEx::HasEditChanged()
	{ ASSERT(::IsWindow(m_hWnd)); return (BOOL) ::SendMessage(m_hWnd, CBEM_HASEDITCHANGED, 0, 0); }
_CJXLIB_INLINE CEdit* CCJComboBoxEx::GetEditCtrl()
	{ ASSERT(::IsWindow(m_hWnd)); return (CEdit*) CEdit::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETEDITCONTROL, 0, 0)); }
_CJXLIB_INLINE CComboBox* CCJComboBoxEx::GetComboBoxCtrl()
	{ ASSERT(::IsWindow(m_hWnd)); return (CComboBox*) CComboBox::FromHandle((HWND) ::SendMessage(m_hWnd, CBEM_GETCOMBOCONTROL, 0, 0)); }
_CJXLIB_INLINE CImageList* CCJComboBoxEx::SetImageList(CImageList* pImageList)
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, CBEM_SETIMAGELIST, 0, (LPARAM)pImageList->GetSafeHandle())); }
_CJXLIB_INLINE CImageList* CCJComboBoxEx::GetImageList() const
	{ ASSERT(::IsWindow(m_hWnd)); return CImageList::FromHandle((HIMAGELIST) ::SendMessage(m_hWnd, CBEM_GETIMAGELIST, 0, 0)); }

// While CCJComboBoxEx derives from CComboBox, there are some
// CB_messages the underlying ComboBoxEx control doesn't support.

_CJXLIB_INLINE int CCJComboBoxEx::Dir(UINT attr, LPCTSTR lpszWildCard)
	{ UNUSED_ALWAYS(attr); UNUSED_ALWAYS(lpszWildCard); ASSERT(FALSE); return CB_ERR; }
_CJXLIB_INLINE int CCJComboBoxEx::FindString(int nIndexStart, LPCTSTR lpszFind) const
	{ UNUSED_ALWAYS(nIndexStart); UNUSED_ALWAYS(lpszFind); ASSERT(FALSE); return CB_ERR; }
_CJXLIB_INLINE int CCJComboBoxEx::AddString(LPCTSTR lpszString)
	{ UNUSED_ALWAYS(lpszString); ASSERT(FALSE); return CB_ERR; }
_CJXLIB_INLINE BOOL CCJComboBoxEx::SetEditSel(int nStartChar, int nEndChar)
	{ UNUSED_ALWAYS(nStartChar); UNUSED_ALWAYS(nEndChar); ASSERT(FALSE); return FALSE; }
_CJXLIB_INLINE int CCJComboBoxEx::InsertString(int nIndex, LPCTSTR lpszString)
	{ UNUSED_ALWAYS(nIndex); UNUSED_ALWAYS(lpszString);	ASSERT(FALSE); return CB_ERR; }

#endif //__CJCOMBOBOXEX_H__

/////////////////////////////////////////////////////////////////////////////
