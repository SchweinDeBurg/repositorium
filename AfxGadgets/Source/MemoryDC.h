// AfxGadgets library.
// Copyright (c) 2004 by Elijah Zarezky,
// All rights reserved.
// Portions copyright (c) 1996-2002 Keith Rule.

// MemoryDC.h - interface of the CMemoryDC class

#if !defined(__MemoryDC_h)
#define __MemoryDC_h

class CMemoryDC: public CDC
{
	DECLARE_DYNAMIC(CMemoryDC)

// construction/destruction
public:
	CMemoryDC(CDC* pDC, COLORREF crBack = 0xFF000000, const RECT* prcDraw = NULL);
	virtual ~CMemoryDC(void);

// operations
public:
	BOOL BlitToScreen(void);

// attributes
public:
	CDC* m_pDC;
	COLORREF m_crBack;
	CRect m_rectDraw;
	CBitmap m_bmpCanvas;
	HBITMAP m_hPrevBmp;

// diagnostic services
#if defined(_DEBUG)
public:
	virtual void AssertValid(void) const;
	virtual void Dump(CDumpContext& dumpCtx) const;
#endif
};

#endif	// __MemoryDC_h

// end of file
