// XEmphasisButton.cpp  Version 1.0 - article available at www.codeproject.com
//
// Author:  Hans Dietrich
//          hdietrich@gmail.com
//
// History
//     Version 1.0 - 2006 August 15
//     - Initial public release
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

#include "stdafx.h"
#include "XEmphasisButton.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

///////////////////////////////////////////////////////////////////////////////
// CXEmphasisButton message map
BEGIN_MESSAGE_MAP(CXEmphasisButton, CButton)
	//{{AFX_MSG_MAP(CXEmphasisButton)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

///////////////////////////////////////////////////////////////////////////////
// ctor
CXEmphasisButton::CXEmphasisButton() :
	m_eEmphasis(NONE),
	m_bUnderline(FALSE),
	m_bBold(FALSE),
	m_bOverControl(FALSE)
{
}

///////////////////////////////////////////////////////////////////////////////
// dtor
CXEmphasisButton::~CXEmphasisButton()
{
	// free gdi objects
	m_fontNormal.DeleteObject();
	m_fontBold.DeleteObject();
	m_fontUnderline.DeleteObject();
	m_fontBoldAndUnderline.DeleteObject();
}

///////////////////////////////////////////////////////////////////////////////
// DefWindowProc
LRESULT CXEmphasisButton::DefWindowProc(UINT message, WPARAM wParam, LPARAM lParam) 
{
	if (message == BM_SETCHECK)
	{
		if (wParam == BST_CHECKED)
		{
			if (m_eEmphasis == BOLD || m_eEmphasis == BOLD_AND_UNDERLINE)
			{
				// button should be bolded
				if (!m_bBold)
				{
					if (m_bUnderline)
						SetFont(&m_fontBoldAndUnderline);
					else
						SetFont(&m_fontBold);
					m_bBold = TRUE;
				}
			}
		}
		else if (wParam == BST_UNCHECKED)
		{
			if (m_eEmphasis == BOLD || m_eEmphasis == BOLD_AND_UNDERLINE)
			{
				// button should not be bolded
				if (m_bBold)
				{
					if (m_bUnderline)
						SetFont(&m_fontUnderline);
					else
						SetFont(&m_fontNormal);
					m_bBold = FALSE;
				}
			}
		}
	}
	else if (message == WM_MOUSEMOVE)
	{
		if (!m_bOverControl)		// cursor has just moved over control
		{
			m_bOverControl = TRUE;
			
			if (m_eEmphasis == UNDERLINE || m_eEmphasis == BOLD_AND_UNDERLINE)
			{
				// button should be underlined
				if (!m_bUnderline)
				{
					if (m_bBold)
						SetFont(&m_fontBoldAndUnderline);
					else
						SetFont(&m_fontUnderline);

					m_bUnderline = TRUE;
				}

				SetTimer(1, 100, NULL);	// use timer to detect when mouse leaves
			}
		}
	}
	else if (message == WM_TIMER)
	{
		if (m_bOverControl)	// only do this if previously over button
		{
			CRect rect;
			GetWindowRect(&rect);

			CPoint point;
			GetCursorPos(&point);	// always screen coordinates

			if (!rect.PtInRect(point))
			{
				// not over button, so remove underlining
				if (m_bUnderline)
				{
					if (m_bBold)
						SetFont(&m_fontBold);
					else
						SetFont(&m_fontNormal);

					m_bUnderline = FALSE;
				}
				m_bOverControl = FALSE;
				KillTimer(wParam);
			}
		}
	}

	return CButton::DefWindowProc(message, wParam, lParam);
}

///////////////////////////////////////////////////////////////////////////////
// PreSubclassWindow
void CXEmphasisButton::PreSubclassWindow() 
{
	CButton::PreSubclassWindow();

	// set up fonts
	CFont *pFont = GetFont();
	if (pFont == NULL)
		pFont = GetParent()->GetFont();
	ASSERT(pFont);
	LOGFONT lf;
	pFont->GetLogFont(&lf);
	VERIFY(m_fontNormal.CreateFontIndirect(&lf));

	LONG lfWeight = lf.lfWeight;
	lf.lfWeight = FW_BOLD;
	VERIFY(m_fontBold.CreateFontIndirect(&lf));

	lf.lfUnderline = TRUE;
	VERIFY(m_fontBoldAndUnderline.CreateFontIndirect(&lf));

	lf.lfWeight = lfWeight;
	VERIFY(m_fontUnderline.CreateFontIndirect(&lf));
}

///////////////////////////////////////////////////////////////////////////////
// SetEmphasisFont
void CXEmphasisButton::SetEmphasisFont()
{
	if (GetCheck())
	{
		if (m_eEmphasis == BOLD || m_eEmphasis == BOLD_AND_UNDERLINE)
		{
			// button should be bolded
			if (!m_bBold)
			{
				if (m_bUnderline)
					SetFont(&m_fontBoldAndUnderline);
				else
					SetFont(&m_fontBold);

				m_bBold = TRUE;
			}
		}
	}
	else
	{
		if (m_eEmphasis == BOLD || m_eEmphasis == BOLD_AND_UNDERLINE)
		{
			// button should not be bolded
			if (m_bBold)
			{
				if (m_bUnderline)
					SetFont(&m_fontUnderline);
				else
					SetFont(&m_fontNormal);

				m_bBold = FALSE;
			}
		}
	}

	Invalidate();
}
