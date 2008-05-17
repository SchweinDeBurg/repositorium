// CJLibrary.h : header file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 13/12/02 23:08 $
 * $Revision: 6 $
 * $Archive: /Projects/Libraries/CJLibrary/Include/CJLibrary.h $
 *
 * $History: CJLibrary.h $
 * 
 * *****************  Version 6  *****************
 * User: Anna         Date: 13/12/02   Time: 23:08
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * Raised version to 6.10 and added CCJThemeHelper
 * 
 *
 * *****************  Version 5  *****************
 * User: Andy         Date: 24/05/02   Time: 22:43
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 4  *****************
 * User: Andy         Date: 24/05/02   Time: 13:51
 * Updated in $/Projects/AddIns/ResOrg/CJLibrary/Include
 * Modifications for compatibility with Visual Studio.NET
 * 
 * *****************  Version 3  *****************
 * User: Andy         Date: 10/22/01   Time: 10:11p
 * Updated in $/Projects/Libraries/CJLibrary/Include
 * Added CJLIB_VER #define
 * 
 * *****************  Version 15  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 9:39p
 * Updated in $/CodeJock/Include
 * Added includes for CJHexEdit and CJListBox.
 * 
 * *****************  Version 14  *****************
 * User: Kirk Stowell Date: 10/26/99   Time: 10:57p
 * Updated in $/CodeJock/Include
 * Added includes for CCJMemFile, CCJStatusBar and CCJLogoPane.
 * 
 * *****************  Version 13  *****************
 * User: Kirk Stowell Date: 10/14/99   Time: 7:22p
 * Updated in $/CodeJock/Include
 * Updated for 6.09 release.
 * 
 * *****************  Version 12  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 1:11a
 * Updated in $/CodeJockey/Include
 * Updated copyright and contact information.
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 8/29/99    Time: 9:14p
 * Updated in $/CodeJockey/Include
 * Added Unicode compliance, thanks to Barry Burton for his help with
 * this.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:04a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:42a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:15a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/Include
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 7/18/99    Time: 10:19p
 * Updated in $/CodeJockey/Include
 * Cleaned up import/export macro so that class will be imported when
 * linked to, and exported at compile time.
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 7/14/99    Time: 10:18p
 * Updated in $/CodeJockey/Include
 * Added additional class header files.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 6/26/99    Time: 8:32p
 * Updated in $/CodeJockey/Include
 * Added include for class CCJMDIChildWnd.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 6/25/99   Time: 12:04p
 * Updated in $/CodeJockey/Include
 * Newly added file which supports CJLibrary includes for both Visual C++
 * 5 and 6
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#ifndef __CJLIBRARY_H__
#define __CJLIBRARY_H__

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#if _MSC_VER < 1200
#define _VC_VERSION_5
#endif

#define CJLIB_VER	0x610

#ifndef _CJX_EXT_CLASS
#ifndef _CJX_DYNAMIC_LINK
#define _CJX_EXT_CLASS
#else
#define _CJX_EXT_CLASS AFX_EXT_CLASS
#endif // _LIB
#endif // _CJX_EXT_CLASS

#ifndef _CJXLIB_INLINE
#define _CJXLIB_INLINE inline
#endif // _CJXLIB_INLINE

#ifndef __AFXTEMPL_H__
#include <afxtempl.h>
#endif

#ifndef __AFXPRIV_H__
#include <afxpriv.h>
#endif


#if _MSC_VER >= 1300
	#include <..\src\mfc\afximpl.h>		// VC.NET
#else
	#include <..\src\afximpl.h>			// VC 5/6
#endif


// Call to initalise the library if using it from a regular DLL
AFX_API_EXPORT	void WINAPI InitCJLibrary(void);



#ifndef __CJThemeHelper_H__
#include "CJThemeHelper.h"
#endif

#ifndef __CJBROWSEEDIT_H__
#include "CJBrowseEdit.h"
#endif

#ifndef __CJCAPTION_H__
#include "CJCaption.h"
#endif

#ifndef __CJCAPTIONPOPUPWND_H__
#include "CJCaptionPopupWnd.h"
#endif

#ifndef __CJCOLORPICKER_H__
#include "CJColorPicker.h"
#endif

#ifndef __CJCOLORPOPUP_H__
#include "CJColorPopup.h"
#endif

#ifndef __CJCOMBOBOXEX_H__
#include "CJComboBoxEx.h"
#endif

#ifndef __CJCONTROLBAR_H__
#include "CJControlBar.h"
#endif

#ifndef __CJDATETIMECTRL_H__
#include "CJDateTimeCtrl.h"
#endif

#ifndef __CJDOCKBAR_H__
#include "CJDockBar.h"
#endif

#ifndef __CJDOCKCONTEXT_H__
#include "CJDockContext.h"
#endif

#ifndef __CJEXPLORERBAR_H__
#include "CJExplorerBar.h"
#endif

#ifndef __CJFLATBUTTON_H__
#include "CJFlatButton.h"
#endif

#ifndef __CJFLATCOMBOBOX_H__
#include "CJFlatComboBox.h"
#endif

#ifndef __CJFLATHEADERCTRL_H__
#include "CJFlatHeaderCtrl.h"
#endif

#ifndef __CJFLATSPLITTERWND_H__
#include "CJFlatSplitterWnd.h"
#endif

#ifndef __CCJFLATTABCTRL_H__
#include "CJFlatTabCtrl.h"
#endif

#ifndef __CJFONTCOMBO_H__
#include "CJFontCombo.h"
#endif

#ifndef __CJFRAMEWND_H__
#include "CJFrameWnd.h"
#endif

#ifndef __CJHEXEDIT_H__
#include "CJHexEdit.h"
#endif

#ifndef __CJHTMLVIEW_H__
#include "CJHtmlView.h"
#endif

#ifndef __CJHYPERLINK_H__
#include "CJHyperLink.h"
#endif

#ifndef __CJLISTBOX_H__
#include "CJListBox.h"
#endif

#ifndef __CJLISTCTRL_H__
#include "CJListCtrl.h"
#endif

#ifndef __CJLISTVIEW_H__
#include "CJListView.h"
#endif

#ifndef __CJLOGOPANE_H__
#include "CJLogoPane.h"
#endif

#ifndef __CJMASKEDIT_H__
#include "CJMaskEdit.h"
#endif

#ifndef __CJMDICHILDWND_H__
#include "CJMDIChildWnd.h"
#endif

#ifndef __CJMDIFRAMEWND_H__
#include "CJMDIFrameWnd.h"
#endif

#ifndef __CJMEMFILE_H__
#include "CJMemFile.h"
#endif

#ifndef __CJMENU_H__
#include "CJMenu.h"        // CCJMenu class declaration
#endif

#ifndef __CJMENUBAR_H__
#include "CJMenuBar.h"
#endif

#ifndef __CJMETAFILEBUTTON_H__
#include "CJMetaFileButton.h"
#endif

#ifndef __CJMINIDOCKFRAMEWND_H__
#include "CJMiniDockFrameWnd.h"
#endif

#ifndef __CJOUTLOOKBAR_H__
#include "CJOutlookBar.h"
#endif

#ifndef __CJPAGERCTRL_H__
#include "CJPagerCtrl.h"
#endif

#ifndef __CJREBAR_H__
#include "CJReBar.h"
#endif

#ifndef __CJREBARCTRL_H__
#include "CJReBarCtrl.h"
#endif

#ifndef __CJSHELL_H__
#include "CJShell.h"
#endif

#ifndef __CJSHELLLIST_H__
#include "CJShellList.h"
#endif

#ifndef __CJSHELLTREE_H__
#include "CJShellTree.h"
#endif

#ifndef __CJSIZEDOCKBAR_H__
#include "CJSizeDockBar.h"
#endif

#ifndef __CJSORTCLASS_H__
#include "CJSortClass.h"
#endif

#ifndef __CJSTATUSBAR_H__
#include "CJStatusBar.h"
#endif

#ifndef __CJTABCTRL_H__
#include "CJTabCtrl.h"
#endif

#ifndef __CJTABCTRLBAR_H__
#include "CJTabCtrlBar.h"
#endif

#ifndef __CJTABVIEW_H__
#include "CJTabView.h"
#endif

#ifndef __CJTOOLBAR_H__
#include "CJToolBar.h"
#endif

#ifndef __CCJTOOLBARBASE_H__
#include "CJToolBarBase.h"
#endif

#ifndef __CJTOOLBARCTRL_H__
#include "CJToolBarCtrl.h"
#endif

#ifndef __CJTREECTRL_H__
#include "CJTreeCtrl.h"
#endif

#ifndef __CJWINDOWPLACEMENT_H__
#include "CJWindowPlacement.h"
#endif

#ifndef __GFXGROUPEDIT_H__
#include "GfxGroupEdit.h"
#endif

#ifndef __GFXOUTBARCTRL_H__
#include "GfxOutBarCtrl.h"
#endif

#ifndef __GFXSPLITTERWND_H__
#include "GfxSplitterWnd.h"
#endif

#define _delete(p){if(p){delete p;p=NULL;}}

#endif // __CJLIBRARY_H__ 
