/* noforcem.h -- force private CRT/MFC assemblies to be used
 * Version 2.0 by Martin Richter [WWJD]
 * Supports VC-2005 and VC-2008
 * 
 * see http://www.codeproject.com/KB/cpp/PrivateAssemblyProjects.aspx and
 * http://blog.m-ri.de/index.php/2008/05/06/hotfix-fuer-usemsprivateassembliesh-und-vc-2008/
 * by by Martin Richter for more information
 */

#ifndef NOFORCEM_H
#define NOFORCEM_H

#if defined(_MSC_VER) && (_MSC_VER >= 1400) && !defined(UNDER_CE)
/* VC++ 8.0 or greater */

#pragma once

#define _STL_NOFORCE_MANIFEST
#define _CRT_NOFORCE_MANIFEST
#define _AFX_NOFORCE_MANIFEST
#define _ATL_NOFORCE_MANIFEST

#ifdef __cplusplus
extern "C"
{
#endif

__declspec(selectany) int _forceCRTManifest;
__declspec(selectany) int _forceMFCManifest;
__declspec(selectany) int _forceAtlDllManifest;

/* the next symbols are used by the several versions of VC++ 9.0 */
__declspec(selectany) int _forceCRTManifestRTM;
__declspec(selectany) int _forceMFCManifestRTM;
__declspec(selectany) int _forceMFCManifestCUR;

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* _MSC_VER && UNDER_CE */

#endif /* NOFORCEM_H */
