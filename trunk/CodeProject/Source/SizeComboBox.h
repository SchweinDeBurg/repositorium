/* ==========================================================================
	File :			SizeComboBox.h

	Class :			CSizeComboBox

	Author :		Johan Rosengren, Abstrakt Mekanik AB
					Iain Clarke

	Date :			2005-05-06

	Purpose :		"CSizeComboBox" is derived from "CComboBox" and is a simple 
					combobox for displaying a selection of font sizes.

	Description :	Simpel derived class with members to fill the box and 
					select an entry by contents instead of index.

	Usage :			Create as any combobox, and call "FillCombo" to fill 
					the control with a selection of font sizes. Call 
					"SelectFontSize" to select an entry by content.

   ========================================================================*/

#if !defined(AFX_SIZECOMBOBOX_H__8A810F5D_013B_43FE_9821_505F4AAF12D6__INCLUDED_)
#define AFX_SIZECOMBOBOX_H__8A810F5D_013B_43FE_9821_505F4AAF12D6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SizeComboBox.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CSizeComboBox window

class CSizeComboBox : public CComboBox
{
// Construction
public:
	CSizeComboBox();

// Attributes
public:

// Operations
public:
	void FillCombo();
	BOOL SelectSize( int size );

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSizeComboBox)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSizeComboBox();

	// Generated message map functions
protected:
	//{{AFX_MSG(CSizeComboBox)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SIZECOMBOBOX_H__8A810F5D_013B_43FE_9821_505F4AAF12D6__INCLUDED_)
