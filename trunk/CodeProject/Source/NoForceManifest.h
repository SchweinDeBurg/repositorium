// NoForceManifest.h
// Version 2.0 by Martin Richter [WWJD]
// Supports VC-2005 and VC-2008

// see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
// http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
// by by Martin Richter for more information

#if !defined(__NoForceManifest_h)
#define __NoForceManifest_h

#if defined(_MSC_VER) && (_MSC_VER > 1000)
#pragma once
#endif   // _MSC_VER

// avoid problems with the resource compiler if included
#if !defined(RC_INVOKED)

// these defines block the creation in the header files
#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

// The next statements block the linker from including object files in the
// CRT and the MFC, that would create manifest pragmas too.
#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

// the next symbols are used by the several versions of VC++ 9.0
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
}   // extern "C"
#endif

#endif   // RC_INVOKED

#endif   // __NoForceManifest_h

// end of file
