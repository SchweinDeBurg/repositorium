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

// Themed.h: interface for the CThemed class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_)
#define AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CThemed  
{
public:
	CThemed(CWnd* pWnd = NULL, LPCTSTR szClassList = NULL);
	virtual ~CThemed();
	
	static BOOL AreControlsThemed();
	static BOOL IsNonClientThemed();
	static BOOL IsWebContentThemed();
	static BOOL IsThemeActive();
	static BOOL SupportsTheming(int nFlag);
	
	static BOOL DrawFrameControl(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, LPCRECT prClip = NULL);
	static BOOL DrawEdge(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nType, UINT nState, UINT nEdge, UINT nFlags);
	static BOOL DrawCaption(const CWnd* pWnd, CDC* pDC, LPRECT pRect, UINT nFlags);
	
	BOOL Open(const CWnd* pWnd, LPCTSTR szClassList);
	BOOL IsValid() { return (s_hUxTheme && m_hTheme); }
	BOOL Close();
	
	BOOL DrawParentBackground(const CWnd* pWnd, CDC* pDC, LPRECT pRect = NULL);
	BOOL DrawBackground(CDC* pDC, int nPart, int nState, const CRect& rect, LPCRECT prClip = NULL);
	BOOL DrawText(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, DWORD dwTextFlags2, const CRect& pRect);
	BOOL DrawEdge(CDC* pDC, int nPart, int nState, const CRect& rDest, UINT nEdge, UINT nFlags, LPRECT prContent = NULL);
	BOOL DrawIcon(CDC* pDC, int nPart, int nState, const CRect& rect, const CImageList* pIL, int nImage);
	BOOL DrawBorder(CDC* pDC, int nState, const CRect& rect);
	
	BOOL GetSize(int nPart, int nState, CSize& size, BOOL bMin = FALSE);
	BOOL GetTextExtent(CDC* pDC, int nPart, int nState, const CString& sText, DWORD dwTextFlags, CRect& rExtent, LPCRECT prBounding = NULL);
	BOOL BuildImageList(CImageList& il, int nPart, const int nStates[], int nNumStates, COLORREF crMask = 255);

	COLORREF GetThemeColor(int nPart, int nState, int nProp);
	BOOL GetThemeBackgroundContentRect(CDC* pDC, int nPart,	int nState, const CRect& rBounding, CRect& rContent);
	
protected:
	static HMODULE s_hUxTheme;
	HTHEME m_hTheme;
	HWND m_hWnd;
	
protected:
	BOOL InitUxTheme();
	
	// uxtheme functions
	static DWORD GetAppThemeProperties();
	HTHEME OpenThemeData(HWND hwnd, LPCWSTR pszClassList);
	BOOL CloseThemeData(HTHEME hTheme);
	BOOL DrawThemeBackground(HDC hdc, int iPartId, int iStateId, const RECT *pRect, 
		const RECT *pClipRect);
	BOOL DrawThemeParentBackground(HWND hWnd, HDC hdc, RECT *pRect);
	BOOL DrawThemeText(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
		DWORD dwTextFlags, DWORD dwTextFlags2, const RECT *pRect);
	BOOL DrawThemeEdge(HDC hdc, int iPartId, int iStateId, const RECT *pDestRect, UINT uEdge, 
		UINT uFlags, RECT *pContentRect);
	BOOL DrawThemeIcon(HDC hdc, int iPartId, int iStateId, const RECT *pRect, HIMAGELIST himl, 
		int iImageIndex);
	BOOL DrawThemeBorder(HDC hdc, int iStateId, const RECT *pRect);
	BOOL GetThemePartSize(int iPartId, int iStateId, THEMESIZE eSize, SIZE *psz);
	BOOL GetThemeTextExtent(HDC hdc, int iPartId, int iStateId, LPCWSTR pszText, int iCharCount, 
		DWORD dwTextFlags, const RECT *pBoundingRect, RECT *pExtentRect);
	BOOL GetThemeColor(int iPartId, int iStateId, int iPropId, COLORREF *pColor);
	BOOL GetThemeBackgroundContentRect(HDC hdc, int iPartId, int iStateId, 
										LPCRECT pBoundingRect, LPRECT pContentRect);

	// helpers
	static BOOL GetThemeClassPartState(int nType, int nState, CString& sThClass, int& nThPart, int& nThState); 
};

#endif // !defined(AFX_THEMED_H__2FA586FE_B790_4315_93B9_01000725B68A__INCLUDED_)
