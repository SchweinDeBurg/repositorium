/* ==========================================================================
    File :          RuleRichEdit.cpp

    Class :         CRulerRichEdit

    Author :        Johan Rosengren, Abstrakt Mekanik AB
                    Iain Clarke

    Date :          2004-04-17

    Purpose :       "CRulerRichEdit" is derived from "CWnd".

    Description :   The class, in addition to the normal "CWnd",
                    handles horizontal scrollbar messages - forcing an
                    update of the parent (to synchronize the ruler). The
                    change notification is called for the same reason.
                    "WM_GETDLGCODE" is handled, we want all keys in a
                    dialog box instantiation.

    Usage :         This class is only useful as a child of the
                    "CRulerRichEditCtrl".

========================================================================*/

#include "StdAfx.h"
#include "RulerRichEdit.h"

#include "../../../CodeProject/Source/RichEditHelper.h"
#include "../../../CodeProject/Source/AutoFlag.h"
#include "../../../CodeProject/Source/EnBitmap.h"
#include "../../../CodeProject/Source/Misc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CRulerRichEdit

CRulerRichEdit::CRulerRichEdit() : m_bPasteSimple(FALSE), m_bIMEComposing(FALSE)
/* ============================================================
    Function :      CRulerRichEdit::CRulerRichEdit
    Description :   constructor
    Access :        Public

    Return :        void
    Parameters :    none

    Usage :

   ============================================================*/
{
	EnableToolTips();
}

CRulerRichEdit::~CRulerRichEdit()
/* ============================================================
    Function :      CRulerRichEdit::~CRulerRichEdit
    Description :   destructor
    Access :        Public

    Return :        void
    Parameters :    none

    Usage :

   ============================================================*/
{
}

BEGIN_MESSAGE_MAP(CRulerRichEdit, CUrlRichEditCtrl)
//{{AFX_MSG_MAP(CRulerRichEdit)
	ON_WM_HSCROLL()
	ON_WM_GETDLGCODE()
	ON_WM_LBUTTONDBLCLK()
//}}AFX_MSG_MAP
	ON_MESSAGE(WM_DROPFILES, OnDropFiles)
	ON_MESSAGE(WM_IME_STARTCOMPOSITION, OnIMEStartComposition)
	ON_MESSAGE(WM_IME_ENDCOMPOSITION, OnIMEEndComposition)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRulerRichEdit message handlers

void CRulerRichEdit::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
/* ============================================================
    Function :      CRulerRichEdit::OnHScroll
    Description :   Handles the "WM_HSCROLL" message.
    Access :        Protected

    Return :        void
    Parameters :    UINT nSBCode             -   Type of operation
                    UINT nPos                -   New position
                    CScrollBar* pScrollBar   -   Pointer to scrollbar

    Usage :         Called from MFC. Updates the ruler.

   ============================================================*/
{
	CUrlRichEditCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

	if (nSBCode == SB_THUMBTRACK)
	{
		SCROLLINFO	si;
		ZeroMemory(&si, sizeof(SCROLLINFO));
		si.cbSize = sizeof(SCROLLINFO);
		GetScrollInfo(SB_HORZ, &si);

		si.nPos = nPos;
		SetScrollInfo(SB_HORZ, &si);

		// notify parent
		GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_HSCROLL), (LPARAM)GetSafeHwnd());
	}
}

LRESULT CRulerRichEdit::OnDropFiles(WPARAM wp, LPARAM lp)
{
	CWaitCursor cursor;

	if (Misc::KeyIsPressed(VK_SHIFT)) // insert
	{
		// restore selection
		SetSel(m_crDropSel);

		TCHAR szFileName[_MAX_PATH];
		::DragQueryFile((HDROP)wp, 0, szFileName, _MAX_PATH);

		// if CTRL is also down AND it's an image file then insert as bits
		if (Misc::KeyIsPressed(VK_CONTROL) && CEnBitmap::CopyImageFileToClipboard(szFileName))
		{
			Paste(FALSE);
			return TRUE;
		}
		else // insert as OLE object
		{
			CReFileObject fo(*this);
			return fo.Insert(szFileName);
		}
	}
	else
	{
		return CUrlRichEditCtrl::OnDropFiles(wp, lp);   // link
	}
}

HRESULT CRulerRichEdit::GetDragDropEffect(BOOL fDrag, DWORD grfKeyState, LPDWORD pdwEffect)
{
	if (!fDrag) // allowable dest effects
	{
		BOOL bEnable = !(GetStyle() & ES_READONLY) && IsWindowEnabled();

		if (!bEnable)
		{
			*pdwEffect = DROPEFFECT_NONE;
		}
		else
		{
			DWORD dwEffect = DROPEFFECT_NONE;
			BOOL bFileDrop = ((*pdwEffect & DROPEFFECT_LINK) == DROPEFFECT_LINK);
			BOOL bShift = (grfKeyState & MK_SHIFT) == MK_SHIFT;
			BOOL bCtrl = (grfKeyState & MK_CONTROL) == MK_CONTROL;

			if (bFileDrop)
			{
				// if SHIFT is down then show this as a copy because we are embedding
				if (bShift)
				{
					dwEffect = DROPEFFECT_COPY | DROPEFFECT_MOVE;
				}

				else // show as a move to match CUrlRichEditCtrl
				{
					dwEffect = DROPEFFECT_MOVE;
				}

				TrackDragCursor();
			}
			else
			{
				if (bCtrl)
				{
					dwEffect = DROPEFFECT_COPY;
				}
				else
				{
					dwEffect = DROPEFFECT_MOVE;
				}
			}

			// make sure allowed type
			if ((dwEffect & *pdwEffect) == dwEffect)
			{
				*pdwEffect = dwEffect;
			}
		}
	}
	return S_OK;
}

BOOL CRulerRichEdit::SetParaFormat(PARAFORMAT2& pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT2);
	BOOL bResult = (BOOL)::SendMessage(m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&pf);

	// Windows seems not to sent EN_CHANGE when using PARAFORMAT2
	// so we must send it ourselves
	GetParent()->SendMessage(WM_COMMAND, MAKEWPARAM(GetDlgCtrlID(), EN_CHANGE), (LPARAM)GetSafeHwnd());

	return bResult;
}

BOOL CRulerRichEdit::SetParaFormat(PARAFORMAT& pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT);
	return (BOOL)::SendMessage(m_hWnd, EM_SETPARAFORMAT, 0, (LPARAM)&pf);
}

BOOL CRulerRichEdit::GetParaFormat(PARAFORMAT2& pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT2);
	return (BOOL)::SendMessage(m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
}

BOOL CRulerRichEdit::GetParaFormat(PARAFORMAT& pf)
{
	ASSERT(::IsWindow(m_hWnd));
	pf.cbSize = sizeof(PARAFORMAT);
	return (BOOL)::SendMessage(m_hWnd, EM_GETPARAFORMAT, 0, (LPARAM)&pf);
}

CLIPFORMAT CRulerRichEdit::GetAcceptableClipFormat(LPDATAOBJECT lpDataOb, CLIPFORMAT format)
{
	if (m_bPasteSimple)
	{
		return CF_TEXT;
	}

	static CLIPFORMAT cfRtf = (CLIPFORMAT)::RegisterClipboardFormat(CF_RTF);
	static CLIPFORMAT cfRtfObj = (CLIPFORMAT)::RegisterClipboardFormat(CF_RETEXTOBJ);
// 	static CLIPFORMAT cfOutlookObj1 = (CLIPFORMAT)::RegisterClipboardFormat("CFSTR_FILECONTENTS");
// 	static CLIPFORMAT cfOutlookObj2 = (CLIPFORMAT)::RegisterClipboardFormat("CFSTR_FILEDESCRIPTORA");

	CLIPFORMAT formats[] =
	{
		CF_HDROP,
		cfRtf,
		cfRtfObj,
		CF_BITMAP,
		CF_TEXT,
		CF_UNICODETEXT,
		CF_METAFILEPICT,
		CF_SYLK,
		CF_DIF,
		CF_TIFF,
		CF_OEMTEXT,
		CF_DIB,
		CF_PALETTE,
		CF_PENDATA,
		CF_RIFF,
		CF_WAVE,
//		cfOutlookObj1,
//		cfOutlookObj2,
		CF_ENHMETAFILE
	};

	const long nNumFmts = sizeof(formats) / sizeof(CLIPFORMAT);

	COleDataObject dataobj;
	dataobj.Attach(lpDataOb, FALSE);

	for (int nFmt = 0; nFmt < nNumFmts; nFmt++)
	{
		if (format && format == formats[nFmt])
		{
			return format;
		}

		if (dataobj.IsDataAvailable(formats[nFmt]))
		{
			return formats[nFmt];
		}
	}

	// all else
	return CF_HDROP;
}

UINT CRulerRichEdit::OnGetDlgCode()
{
	return DLGC_WANTALLKEYS;
}

void CRulerRichEdit::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));

	CUrlRichEditCtrl::OnLButtonDblClk(nFlags, point);
}

void CRulerRichEdit::Paste(BOOL bSimple)
{
	if (!bSimple)
	{
		CUrlRichEditCtrl::Paste();
	}
	else
	{
		CAutoFlag af(m_bPasteSimple, TRUE);

		CUrlRichEditCtrl::Paste();
	}
}

int CRulerRichEdit::OnToolHitTest(CPoint point, TOOLINFO* pTI) const
{
	return CUrlRichEditCtrl::OnToolHitTest(point, pTI);
	/*
		int nHit = MAKELONG(point.x, point.y);
		pTI->hwnd = m_hWnd;
		pTI->uId  = nHit;
		pTI->rect = CRect(CPoint(point.x-1,point.y-1),CSize(2,2));
		pTI->uFlags |= TTF_NOTBUTTON | TTF_ALWAYSTIP;
		pTI->lpszText = LPSTR_TEXTCALLBACK;

		return nHit;
	*/
}

LRESULT CRulerRichEdit::OnIMEStartComposition(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_bIMEComposing = TRUE;

	return Default();
}

LRESULT CRulerRichEdit::OnIMEEndComposition(WPARAM /*wp*/, LPARAM /*lp*/)
{
	m_bIMEComposing = FALSE;

	return Default();
}
