// AfxGadgets library.
// Copyright (c) 2004-2005 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 1996-2002 Keith Rule.

// MemoryDC.cpp - implementation of the CMemoryDC class

#include "stdafx.h"
#include "MemoryDC.h"

// object model
IMPLEMENT_DYNAMIC(CMemoryDC, CDC)

CMemoryDC::CMemoryDC(CDC* pDC, COLORREF crBack, const RECT* prcDraw):
m_pDC(pDC),
m_crBack(crBack),
m_hPrevBmp(NULL)
{
	// precondition
	ASSERT_VALID(m_pDC);
	ASSERT(!m_pDC->IsPrinting());

	// adjust and remember initialization values
	if ((m_crBack & 0xFF000000) != 0) {
		m_crBack = m_pDC->GetBkColor();
	}
	if (prcDraw != NULL) {
		m_rectDraw.CopyRect(prcDraw);
	}
	else {
		ASSERT(AfxIsValidAddress(prcDraw, sizeof(*prcDraw)));
		m_pDC->GetClipBox(&m_rectDraw);
	}

	// create and initialize the in-memory DC
	VERIFY(CreateCompatibleDC(m_pDC));
	SetMapMode(m_pDC->GetMapMode());
	m_bmpCanvas.CreateCompatibleBitmap(m_pDC, m_rectDraw.Width(), m_rectDraw.Height());
	m_hPrevBmp = static_cast<HBITMAP>(SelectObject(&m_bmpCanvas)->GetSafeHandle());
	CRect rectTemp(m_rectDraw);
	rectTemp.left = 0;
	rectTemp.right -= m_rectDraw.left;
	rectTemp.top = 0;
	rectTemp.bottom -= m_rectDraw.top;
	FillSolidRect(&rectTemp, m_crBack);
}

CMemoryDC::~CMemoryDC(void)
{
	// clean-up
	if (m_hPrevBmp != NULL) {
		SelectObject(m_hPrevBmp);
		m_bmpCanvas.DeleteObject();
	}
	VERIFY(DeleteDC());
}

BOOL CMemoryDC::BlitToScreen(void)
{
	BOOL fSuccess = m_pDC->BitBlt(m_rectDraw.left, m_rectDraw.top, m_rectDraw.Width(),
		m_rectDraw.Height(), this, 0, 0, SRCCOPY);
	return (fSuccess);
}

#if defined(_DEBUG)

void CMemoryDC::AssertValid(void) const
{
	// first perform inherited validity check...
	CDC::AssertValid();
	// ...and then verify our own state as well
	ASSERT_VALID(&m_bmpCanvas);
}

void CMemoryDC::Dump(CDumpContext& dumpCtx) const
{
	try {
		// first invoke inherited dumper...
		CDC::Dump(dumpCtx);
		// ...and then dump own unique members
		dumpCtx << "m_pDC = " << m_pDC;
		dumpCtx << "\nm_crBack = " << m_crBack;
		dumpCtx << "\nm_rectDraw = " << m_rectDraw;
		dumpCtx << "\nm_bmpCanvas = " << m_bmpCanvas;
		dumpCtx << "\nm_hPrevBmp = " << m_hPrevBmp;
	}
	catch (CFileException* pXcpt) {
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
