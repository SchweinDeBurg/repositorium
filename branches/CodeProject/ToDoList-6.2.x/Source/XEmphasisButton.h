// XEmphasisButton.h  Version 1.0 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// License:
//     This software is released into the public domain.  You are free to use
//     it in any way you like, except that you may not sell this source code.
//
//     This software is provided "as is" with no expressed or implied warranty.
//     I accept no liability for any damage or loss of business that this 
//     software may cause.
//
///////////////////////////////////////////////////////////////////////////////

#ifndef XEMPHASISBUTTON_H
#define XEMPHASISBUTTON_H

/////////////////////////////////////////////////////////////////////////////
// CXEmphasisButton window

class CXEmphasisButton : public CButton
{
// Construction
public:
	CXEmphasisButton();
	virtual ~CXEmphasisButton();

// Attributes
public:
	enum Emphasis
	{
		NONE = 0,
		BOLD,				// button text will be bold if GetCheck() is TRUE
		UNDERLINE,			// button text will be underlined if mouse over
		BOLD_AND_UNDERLINE
	};

	Emphasis GetEmphasis() { return m_eEmphasis; }
	void SetEmphasis(Emphasis eEmphasis) 
	{ 
		m_eEmphasis = eEmphasis; 
		SetEmphasisFont();
	}

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CXEmphasisButton)
protected:
	virtual LRESULT DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual void PreSubclassWindow();
	//}}AFX_VIRTUAL

// Implementation
public:

	// Generated message map functions
protected:
	Emphasis	m_eEmphasis;
	BOOL		m_bUnderline;
	BOOL		m_bBold;
	BOOL		m_bOverControl;

	CFont		m_fontNormal;
	CFont		m_fontBold;
	CFont		m_fontUnderline;
	CFont		m_fontBoldAndUnderline;

	void SetEmphasisFont();

	//{{AFX_MSG(CXEmphasisButton)
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif //XEMPHASISBUTTON_H
