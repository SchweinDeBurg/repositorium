// atlmfc71 library.
// Copyright (c) 2005 by Elijah Zarezky,
// All rights reserved.
// General copyright (c) 1992-2002 Microsoft Corporation.

// FileDialogEx.inl - inline members of the CFileDialogEx class

#if defined(_FileDialogEx_INLINE)

#if defined(__INTEL_COMPILER)
// remark #171: invalid type conversion
#pragma warning(disable: 171)
#endif	// __INTEL_COMPILER

// extracted from <afxdlgs.inl> v7.1

_FileDialogEx_INLINE
BOOL CFileDialogEx::GetReadOnlyPref(void) const
{
	return m_ofn.Flags & OFN_READONLY ? TRUE : FALSE;
}

_FileDialogEx_INLINE
void CFileDialogEx::SetTemplate(UINT nWin3ID, UINT nWin4ID)
{
	SetTemplate(MAKEINTRESOURCE(nWin3ID), MAKEINTRESOURCE(nWin4ID));
}

_FileDialogEx_INLINE
POSITION CFileDialogEx::GetStartPosition(void) const
{
	return (POSITION)m_ofn.lpstrFile;	
}

#if defined(__INTEL_COMPILER)
#pragma warning(default: 171)
#endif	// __INTEL_COMPILER

#endif	// _FileDialogEx_INLINE

// end of file
