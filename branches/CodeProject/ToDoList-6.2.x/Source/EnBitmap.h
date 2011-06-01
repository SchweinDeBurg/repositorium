// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - added AbstractSpoon Software copyright notice and license information
// - taken out from the original ToDoList package for better sharing
// - reformatted with using Artistic Style 2.01 and the following options:
//      --indent=tab=3
//      --indent=force-tab=3
//      --indent-switches
//      --max-instatement-indent=2
//      --brackets=break
//      --add-brackets
//      --pad-oper
//      --unpad-paren
//      --pad-header
//      --align-pointer=type
//      --lineend=windows
//      --suffix=none
//*****************************************************************************

// EnBitmap.h: interface for the CEnBitmap class.
//
// credits: Peter Hendrix's CPicture implementation for the original IPicture code
//          Yves Maurer's GDIRotate implementation for the idea of working directly on 32 bit representations of bitmaps
//          Karl Lager's 'A Fast Algorithm for Rotating Bitmaps'
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
#define AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ColorDef.h"
#include <afxtempl.h>

/////////////////////////////////////////////////////////////////////////////////////
// base class for image processing

class C32BitImageProcessor
{
public:
	C32BitImageProcessor(BOOL bEnableWeighting = FALSE);
	virtual ~C32BitImageProcessor();

	virtual CSize CalcDestSize(CSize sizeSrc);
	virtual BOOL ProcessPixels(RGBX* pSrcPixels, CSize sizeSrc, RGBX* pDestPixels, CSize sizeDest,
		COLORREF crMask = -1);

	inline void CalcWeightedColor(RGBX* pPixels, CSize size, double dX, double dY, RGBX& rgbResult);

protected:
	BOOL m_bWeightingEnabled;
};

typedef CArray<C32BitImageProcessor*, C32BitImageProcessor*> C32BIPArray;

//////////////////////////////////////////////////////////////////////////////////////////

class CEnBitmap : public CBitmap
{
public:
	CEnBitmap(COLORREF crBkgnd = RGB(255, 255, 255));
	virtual ~CEnBitmap();

	BOOL LoadImage(LPCTSTR szImagePath, COLORREF crBack = -1);
	BOOL LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL, COLORREF crBack = -1);
	BOOL CopyImage(HBITMAP hBitmap);
	BOOL CopyImage(CBitmap* pBitmap);
	BOOL Copy(HIMAGELIST hImageList);

	static BOOL CopyImageFileToClipboard(LPCTSTR szImagePath, COLORREF crBack = -1);
	BOOL CopyToClipboard(HWND hWnd = NULL) const;

	BOOL ProcessImage(C32BitImageProcessor* pProcessor, COLORREF crMask = -1);
	BOOL ProcessImage(C32BIPArray& aProcessors, COLORREF crMask = -1); // ordered list of processors

	// helpers
	static HBITMAP LoadImageFile(LPCTSTR szImagePath, COLORREF crBack = -1);
	static HBITMAP LoadImageResource(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst = NULL, COLORREF crBack = 0);
	static BOOL GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize);
	static IPicture* LoadFromBuffer(BYTE* pBuff, int nSize);

protected:
	COLORREF m_crBkgnd;

protected:
	RGBX* GetDIBits32();
	BOOL PrepareBitmapInfo32(BITMAPINFO& bi, HBITMAP hBitmap = NULL);

	static HBITMAP ExtractBitmap(IPicture* pPicture, COLORREF crBack);
	static int GetFileType(LPCTSTR szImagePath);
	static BOOL Fill(RGBX* pPixels, CSize size, COLORREF color);
};

// inline weighting function
inline void C32BitImageProcessor::CalcWeightedColor(RGBX* pPixels, CSize size, double dX, double dY, RGBX& rgbResult)
{
	ASSERT(m_bWeightingEnabled);

	// interpolate between the current pixel and its adjacent pixels to the right and down
	int nX = (int)dX;
	int nY = (int)dY;

	if (dX < 0 || dY < 0)
	{
		rgbResult = pPixels[max(0, nY) * size.cx + max(0, nX)]; // closest
		return;
	}

	double dXFraction = dX - nX;
	double dX1MinusFraction = 1 - dXFraction;

	double dYFraction = dY - nY;
	double dY1MinusFraction = 1 - dYFraction;

	int nXP1 = min(nX + 1, size.cx - 1);
	int nYP1 = min(nY + 1, size.cy - 1);

	RGBX* pRGB = &pPixels[nY * size.cx + nX]; // x, y
	RGBX* pRGBXP = &pPixels[nY * size.cx + nXP1]; // x + 1, y
	RGBX* pRGBYP = &pPixels[nYP1 * size.cx + nX]; // x, y + 1
	RGBX* pRGBXYP = &pPixels[nYP1 * size.cx + nXP1]; // x + 1, y + 1

	int nRed = (int)(dX1MinusFraction * dY1MinusFraction * pRGB->btRed +
		dXFraction * dY1MinusFraction * pRGBXP->btRed +
		dX1MinusFraction * dYFraction * pRGBYP->btRed +
		dXFraction * dYFraction * pRGBXYP->btRed);

	int nGreen = (int)(dX1MinusFraction * dY1MinusFraction * pRGB->btGreen +
		dXFraction * dY1MinusFraction * pRGBXP->btGreen +
		dX1MinusFraction * dYFraction * pRGBYP->btGreen +
		dXFraction * dYFraction * pRGBXYP->btGreen);

	int nBlue = (int)(dX1MinusFraction * dY1MinusFraction * pRGB->btBlue +
		dXFraction * dY1MinusFraction * pRGBXP->btBlue +
		dX1MinusFraction * dYFraction * pRGBYP->btBlue +
		dXFraction * dYFraction * pRGBXYP->btBlue);

	rgbResult.btRed = (BYTE)max(0, min(255, nRed));
	rgbResult.btGreen = (BYTE)max(0, min(255, nGreen));
	rgbResult.btBlue = (BYTE) max(0, min(255, nBlue));
}

#endif // !defined(AFX_ENBITMAP_H__1FDE0A4E_8AB4_11D6_95AD_EFA89432A428__INCLUDED_)
