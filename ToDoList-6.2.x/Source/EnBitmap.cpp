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
// - added AbstractSpoon Software copyright notice and licenese information
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
// - merged with ToDoList version 6.2.2 sources
//*****************************************************************************

// EnBitmap.cpp: implementation of the CEnBitmap class (c) daniel godson 2002.
//
// credits: Peter Hendrix's CPicture implementation for the original IPicture code
//          Yves Maurer's GDIRotate implementation for the idea of working directly on 32 bit representations of bitmaps
//          Karl Lager's 'A Fast Algorithm for Rotating Bitmaps'
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "EnBitmap.h"

#include <afxpriv.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

const int HIMETRIC_INCH	= 2540;

enum
{
	FT_BMP,
	FT_ICO,
	FT_JPG,
	FT_GIF,
	FT_PNG,

	FT_UNKNOWN
};

///////////////////////////////////////////////////////////////////////

C32BitImageProcessor::C32BitImageProcessor(BOOL bEnableWeighting) : m_bWeightingEnabled(bEnableWeighting)
{
}

C32BitImageProcessor::~C32BitImageProcessor()
{
}

CSize C32BitImageProcessor::CalcDestSize(CSize sizeSrc)
{
	return sizeSrc; // default
}

BOOL C32BitImageProcessor::ProcessPixels(RGBX* pSrcPixels, CSize /*sizeSrc*/, RGBX* pDestPixels,
	CSize sizeDest, COLORREF /*crMask*/)
{
	CopyMemory(pDestPixels, pSrcPixels, sizeDest.cx * 4 * sizeDest.cy); // default
	return TRUE;
}

// C32BitImageProcessor::CalcWeightedColor(...) is inlined in EnBitmap.h

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CEnBitmap::CEnBitmap(COLORREF crBkgnd) : m_crBkgnd(crBkgnd)
{
}

CEnBitmap::~CEnBitmap()
{
}

BOOL CEnBitmap::LoadImage(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
	{
		return FALSE;
	}

	return Attach(LoadImageResource(uIDRes, szResourceType, hInst, crBack == -1 ? m_crBkgnd : crBack));
}

BOOL CEnBitmap::LoadImage(LPCTSTR szImagePath, COLORREF crBack)
{
	ASSERT(m_hObject == NULL);      // only attach once, detach on destroy

	if (m_hObject != NULL)
	{
		return FALSE;
	}

	return Attach(LoadImageFile(szImagePath, crBack == -1 ? m_crBkgnd : crBack));
}

HBITMAP CEnBitmap::LoadImageFile(LPCTSTR szImagePath, COLORREF crBack)
{
	int nType = GetFileType(szImagePath);

	switch (nType)
	{
	case FT_BMP:
		// the reason for this is that i suspect it is more efficient to load
		// bmps this way since it avoids creating device contexts etc that the
		// IPicture methods requires. that method however is still valuable
		// since it handles other image types and transparency
		return (HBITMAP)::LoadImage(NULL, szImagePath, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE | LR_LOADMAP3DCOLORS);

	case FT_UNKNOWN:
		break;

	default: // all the rest
		{
			IPicture* pPicture = NULL;

			HBITMAP hbm = NULL;
			HRESULT hr = OleLoadPicturePath(ATL::CT2OLE(szImagePath), NULL, 0, crBack, IID_IPicture, (LPVOID*)&pPicture);

			if (SUCCEEDED(hr) && pPicture)
			{
				hbm = ExtractBitmap(pPicture, crBack);
				pPicture->Release();
			}

			return hbm;
		}
	}

	return NULL;
}

HBITMAP CEnBitmap::LoadImageResource(UINT uIDRes, LPCTSTR szResourceType, HMODULE hInst, COLORREF crBack)
{
	BYTE* pBuff = NULL;
	int nSize = 0;
	HBITMAP hbm = NULL;

	// first call is to get buffer size
	if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, 0, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];

			// this loads it
			if (GetResource(MAKEINTRESOURCE(uIDRes), szResourceType, hInst, pBuff, nSize))
			{
				IPicture* pPicture = LoadFromBuffer(pBuff, nSize);

				if (pPicture)
				{
					hbm = ExtractBitmap(pPicture, crBack);
					pPicture->Release();
				}
			}

			delete [] pBuff;
		}
	}

	return hbm;
}

IPicture* CEnBitmap::LoadFromBuffer(BYTE* pBuff, int nSize)
{
	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuff, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;
	IPicture* pPicture = NULL;

	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID*)&pPicture);
		pStream->Release();
	}

	return pPicture; // caller releases
}

BOOL CEnBitmap::GetResource(LPCTSTR lpName, LPCTSTR lpType, HMODULE hInst, void* pResource, int& nBufSize)
{
	HRSRC       hResInfo;
	HANDLE      hRes;
	LPSTR       lpRes   = NULL;
	bool        bResult = FALSE;

	// Find the resource
	hResInfo = FindResource(hInst, lpName, lpType);

	if (hResInfo == NULL)
	{
		return false;
	}

	// Load the resource
	hRes = LoadResource(hInst, hResInfo);

	if (hRes == NULL)
	{
		return false;
	}

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(hInst, hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(hInst, hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		}

		UnlockResource(hRes);
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}

HBITMAP CEnBitmap::ExtractBitmap(IPicture* pPicture, COLORREF crBack)
{
	ASSERT(pPicture);

	if (!pPicture)
	{
		return NULL;
	}

	CBitmap bmMem;
	CDC dcMem;
	CDC* pDC = CWnd::GetDesktopWindow()->GetDC();

	if (dcMem.CreateCompatibleDC(pDC))
	{
		long hmWidth;
		long hmHeight;

		pPicture->get_Width(&hmWidth);
		pPicture->get_Height(&hmHeight);

		int nWidth	= MulDiv(hmWidth, pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
		int nHeight	= MulDiv(hmHeight, pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);

		if (bmMem.CreateCompatibleBitmap(pDC, nWidth, nHeight))
		{
			CBitmap* pOldBM = dcMem.SelectObject(&bmMem);

			if (crBack != -1)
			{
				dcMem.FillSolidRect(0, 0, nWidth, nHeight, crBack);
			}

			HRESULT hr = pPicture->Render(dcMem, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL);
			ASSERT(SUCCEEDED(hr));

			// cleanup
			dcMem.SelectObject(pOldBM);
		}
	}

	CWnd::GetDesktopWindow()->ReleaseDC(pDC);

	return (HBITMAP)bmMem.Detach();
}

BOOL CEnBitmap::CopyToClipboard(HWND hWnd) const
{
	if (!GetSafeHandle())
	{
		return FALSE;
	}

	if (::OpenClipboard(hWnd))
	{
		::EmptyClipboard();
		::SetClipboardData(CF_BITMAP, (HANDLE)GetSafeHandle());
		::CloseClipboard();

		return TRUE;
	}

	return FALSE;
}

BOOL CEnBitmap::CopyImageFileToClipboard(LPCTSTR szImagePath, COLORREF crBack)
{
	CEnBitmap bm;

	if (bm.LoadImage(szImagePath, crBack))
	{
		return bm.CopyToClipboard();
	}

	return FALSE;
}

int CEnBitmap::GetFileType(LPCTSTR szImagePath)
{
	CString sPath(szImagePath);
	sPath.MakeUpper();

	if (sPath.Find(_T(".BMP")) > 0)
	{
		return FT_BMP;
	}

	else if (sPath.Find(_T(".ICO")) > 0)
	{
		return FT_ICO;
	}

	else if (sPath.Find(_T(".JPG")) > 0 || sPath.Find(_T(".JPEG")) > 0)
	{
		return FT_JPG;
	}

	else if (sPath.Find(_T(".GIF")) > 0)
	{
		return FT_GIF;
	}

	else if (sPath.Find(_T(".PNG")) > 0)
	{
		return FT_PNG;
	}

	// else
	return FT_UNKNOWN;
}

BOOL CEnBitmap::ProcessImage(C32BitImageProcessor* pProcessor, COLORREF crMask)
{
	C32BIPArray aProcessors;

	aProcessors.Add(pProcessor);

	return ProcessImage(aProcessors, crMask);
}

BOOL CEnBitmap::ProcessImage(C32BIPArray& aProcessors, COLORREF crMask)
{
	ASSERT(GetSafeHandle());

	if (!GetSafeHandle())
	{
		return FALSE;
	}

	if (!aProcessors.GetSize())
	{
		return TRUE;
	}

	int nProcessor, nCount = aProcessors.GetSize();

	// retrieve src and final dest sizes
	BITMAP BM;

	if (!GetBitmap(&BM))
	{
		return FALSE;
	}

	CSize sizeSrc(BM.bmWidth, BM.bmHeight);
	CSize sizeDest(sizeSrc), sizeMax(sizeSrc);

	for (nProcessor = 0; nProcessor < nCount; nProcessor++)
	{
		sizeDest = aProcessors[nProcessor]->CalcDestSize(sizeDest);
		sizeMax = CSize(max(sizeMax.cx, sizeDest.cx), max(sizeMax.cy, sizeDest.cy));
	}

	// prepare src and dest bits
	RGBX* pSrcPixels = GetDIBits32();

	if (!pSrcPixels)
	{
		return FALSE;
	}

	RGBX* pDestPixels = new RGBX[sizeMax.cx * sizeMax.cy];

	if (!pDestPixels)
	{
		return FALSE;
	}

	Fill(pDestPixels, sizeMax, m_crBkgnd);

	BOOL bRes = TRUE;
	sizeDest = sizeSrc;

	// do the processing
	for (nProcessor = 0; bRes && nProcessor < nCount; nProcessor++)
	{
		// if its the second processor or later then we need to copy
		// the previous dest bits back into source.
		// we also need to check that sizeSrc is big enough
		if (nProcessor > 0)
		{
			if (sizeSrc.cx < sizeDest.cx || sizeSrc.cy < sizeDest.cy)
			{
				delete [] pSrcPixels;
				pSrcPixels = new RGBX[sizeDest.cx * sizeDest.cy];
			}

			CopyMemory(pSrcPixels, pDestPixels, sizeDest.cx * 4 * sizeDest.cy); // default
			Fill(pDestPixels, sizeDest, m_crBkgnd);
		}

		sizeSrc = sizeDest;
		sizeDest = aProcessors[nProcessor]->CalcDestSize(sizeSrc);

		bRes = aProcessors[nProcessor]->ProcessPixels(pSrcPixels, sizeSrc, pDestPixels, sizeDest, crMask);
	}

	// update the bitmap
	if (bRes)
	{
		// set the bits
		HDC hdc = GetDC(NULL);
		HBITMAP hbmSrc = ::CreateCompatibleBitmap(hdc, sizeDest.cx, sizeDest.cy);

		if (hbmSrc)
		{
			BITMAPINFO bi;

			if (PrepareBitmapInfo32(bi, hbmSrc))
			{
				if (SetDIBits(hdc, hbmSrc, 0, sizeDest.cy, pDestPixels, &bi, DIB_RGB_COLORS))
				{
					// delete the bitmap and attach new
					DeleteObject();
					bRes = Attach(hbmSrc);
				}
			}

			::ReleaseDC(NULL, hdc);

			if (!bRes)
			{
				::DeleteObject(hbmSrc);
			}
		}
	}

	delete [] pSrcPixels;
	delete [] pDestPixels;

	return bRes;
}

RGBX* CEnBitmap::GetDIBits32()
{
	BITMAPINFO bi;

	int nHeight = PrepareBitmapInfo32(bi);

	if (!nHeight)
	{
		return FALSE;
	}

	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];
	HDC hdc = GetDC(NULL);

	if (!GetDIBits(hdc, (HBITMAP)GetSafeHandle(), 0, nHeight, pBits, &bi, DIB_RGB_COLORS))
	{
		delete[] pBits;
		pBits = NULL;
	}

	::ReleaseDC(NULL, hdc);

	return (RGBX*)pBits;
}

BOOL CEnBitmap::PrepareBitmapInfo32(BITMAPINFO& bi, HBITMAP hBitmap)
{
	if (!hBitmap)
	{
		hBitmap = (HBITMAP)GetSafeHandle();
	}

	BITMAP BM;

	if (!::GetObject(hBitmap, sizeof(BM), &BM))
	{
		return FALSE;
	}

	bi.bmiHeader.biSize = sizeof(bi.bmiHeader);
	bi.bmiHeader.biWidth = BM.bmWidth;
	bi.bmiHeader.biHeight = -BM.bmHeight;
	bi.bmiHeader.biPlanes = 1;
	bi.bmiHeader.biBitCount = 32; // 32 bit
	bi.bmiHeader.biCompression = BI_RGB; // 32 bit
	bi.bmiHeader.biSizeImage = BM.bmWidth * 4 * BM.bmHeight; // 32 bit
	bi.bmiHeader.biClrUsed = 0;
	bi.bmiHeader.biClrImportant = 0;

	return BM.bmHeight;
}

BOOL CEnBitmap::CopyImage(HBITMAP hBitmap)
{
	ASSERT(hBitmap);

	if (!hBitmap)
	{
		return FALSE;
	}

	BITMAPINFO bi;
	int nHeight = PrepareBitmapInfo32(bi, hBitmap);

	if (!nHeight)
	{
		return FALSE;
	}

	BYTE* pBits = (BYTE*)new BYTE[bi.bmiHeader.biSizeImage];
	HDC hdc = GetDC(NULL);
	BOOL bRes = FALSE;

	if (GetDIBits(hdc, hBitmap, 0, nHeight, pBits, &bi, DIB_RGB_COLORS))
	{
		int nWidth = bi.bmiHeader.biSizeImage / (nHeight * 4);

		HBITMAP hbmDest = ::CreateCompatibleBitmap(hdc, nWidth, nHeight);

		if (hbmDest)
		{
			if (SetDIBits(hdc, hbmDest, 0, nHeight, pBits, &bi, DIB_RGB_COLORS))
			{
				DeleteObject();
				bRes = Attach(hbmDest);
			}
		}
	}

	::ReleaseDC(NULL, hdc);
	delete [] pBits;

	return bRes;
}

BOOL CEnBitmap::CopyImage(CBitmap* pBitmap)
{
	if (!pBitmap)
	{
		return FALSE;
	}

	return CopyImage((HBITMAP)pBitmap->GetSafeHandle());
}

BOOL CEnBitmap::Fill(RGBX* pPixels, CSize size, COLORREF color)
{
	if (!pPixels)
	{
		return FALSE;
	}

	if (color == -1 || color == RGB(255, 255, 255))
	{
		FillMemory(pPixels, size.cx * 4 * size.cy, 255);   // white
	}

	else if (color == 0)
	{
		FillMemory(pPixels, size.cx * 4 * size.cy, 0);   // black
	}

	else
	{
		// fill the first line with the color
		RGBX* pLine = &pPixels[0];
		int nSize = 1;

		pLine[0] = RGBX(color);

		while (1)
		{
			if (nSize > size.cx)
			{
				break;
			}

			// else
			int nAmount = min(size.cx - nSize, nSize) * 4;

			CopyMemory(&pLine[nSize], pLine, nAmount);
			nSize *= 2;
		}

		// use that line to fill the rest of the block
		int nRow = 1;

		while (1)
		{
			if (nRow > size.cy)
			{
				break;
			}

			// else
			int nAmount = min(size.cy - nRow, nRow) * size.cx * 4;

			CopyMemory(&pPixels[nRow * size.cx], pPixels, nAmount);
			nRow *= 2;
		}
	}

	return TRUE;
}

BOOL CEnBitmap::Copy(HIMAGELIST hImageList)
{
	DeleteObject();

	int nWidth = 0, nHeight = 0;
	int nCount = ImageList_GetImageCount(hImageList);

	ImageList_GetIconSize(hImageList, &nWidth, &nHeight);

	HDC hdc = GetDC(NULL);
	HBITMAP hbmDest = ::CreateCompatibleBitmap(hdc, nWidth * nCount, nHeight);
	HDC hdcMem = CreateCompatibleDC(hdc);

	HBITMAP hBMOld = (HBITMAP)::SelectObject(hdcMem, hbmDest);

	for (int nIcon = 0; nIcon < nCount; nIcon++)
	{
		VERIFY(ImageList_Draw(hImageList, nIcon, hdcMem, nIcon * nWidth, 0, ILD_NORMAL));
	}

	// cleanup
	::SelectObject(hdcMem, hBMOld);
	DeleteDC(hdcMem);

	return Attach(hbmDest);
}
