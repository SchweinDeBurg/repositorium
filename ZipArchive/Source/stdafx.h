///////////////////////////////////////////////////////////////////////////////
// $RCSfile: stdafx.h,v $
// $Revision: 1.3 $ $Name:  $
// $Date: 2005/02/14 08:09:54 $ $Author: Tadeusz Dracz $
////////////////////////////////////////////////////////////////////////////////
// This source file is part of the ZipArchive library source distribution and
// is Copyrighted 2000-2005 by Tadeusz Dracz (http://www.artpol-software.com/)
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// For the licensing details see the file License.txt
////////////////////////////////////////////////////////////////////////////////


#if !defined(AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_)
#define AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if (_MSC_VER < 1300) && !defined(__BORLANDC__) && !defined(__INTEL_COMPILER)
#define ZIPINLINE inline
#else
#define ZIPINLINE
#endif

#if _MSC_VER >= 1300
#define ZIP_ULONGLONG ULONGLONG
#define ZIP_LONGLONG LONGLONG
#else
#define ZIP_ULONGLONG DWORD
#define ZIP_LONGLONG LONG
#endif

#define ZIP_ARCHIVE_MFC

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#include <afx.h>
#include <afxwin.h>

// force ISO/IEC 14882 conformance in for loop scope
#if _MSC_VER < 1300
#define for if(false); else for
#else
#pragma conform(forScope, on)
#endif	// _MSC_VER

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__926F70F4_1B34_49AA_9532_498E8D2F3495__INCLUDED_)
 
