// AfxGadgets library.
// Copyright (c) 2004-2011 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 1996-2002 Keith Rule.

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
// http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// MemoryDC.cpp - implementation of the CMemoryDC class

//////////////////////////////////////////////////////////////////////////////////////////////
// PCH includes

#include "stdafx.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// other includes

#include "MemoryDC.h"

//////////////////////////////////////////////////////////////////////////////////////////////
// avoid unwanted ICL warnings

#if defined(__INTEL_COMPILER)
// remark #279: controlling expression is constant
#pragma warning(disable: 279)
// remark #981: operands are evaluated in unspecified order
#pragma warning(disable: 981)
#endif	// __INTEL_COMPILER

//////////////////////////////////////////////////////////////////////////////////////////////
// debugging support

#if defined(_DEBUG)
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif	// _DEBUG

//////////////////////////////////////////////////////////////////////////////////////////////
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
	if ((m_crBack & 0xFF000000) != 0)
	{
		m_crBack = m_pDC->GetBkColor();
	}
	if (prcDraw != NULL)
	{
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
	if (m_hPrevBmp != NULL)
	{
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
	try
	{
		// first invoke inherited dumper...
		CDC::Dump(dumpCtx);

		// ...and then dump own unique members
		dumpCtx << "m_pDC = " << m_pDC << "\n";
		dumpCtx << "m_crBack = " << m_crBack << "\n";
		dumpCtx << "m_rectDraw = " << m_rectDraw << "\n";
		dumpCtx << "m_bmpCanvas = " << m_bmpCanvas << "\n";
		dumpCtx << "m_hPrevBmp = " << m_hPrevBmp;
	}
	catch (CFileException* pXcpt)
	{
		pXcpt->ReportError();
		pXcpt->Delete();
	}
}

#endif	// _DEBUG

// end of file
