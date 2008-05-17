// CJShellTree.cpp : implementation file
// Copyright © 1998-1999 CodeJock.com, All Rights Reserved.
// See ReadMe.txt for TERMS OF USE.
//
// Based upon the MFCENUM example. MFCENUM is a companion sample application 
// for Chapter 14 in "Programming the Windows 95 User Interface" by Nancy Cluts,
// available in the Microsoft(R) Development Library.
//
/////////////////////////////////////////////////////////////////////////////
/****************************************************************************
 *
 * $Date: 10/30/99 1:59p $
 * $Revision: 11 $
 * $Archive: /CodeJock/CJLibrary/CJShellTree.cpp $
 *
 * $History: CJShellTree.cpp $
 * 
 * *****************  Version 11  *****************
 * User: Kirk Stowell Date: 10/30/99   Time: 1:59p
 * Updated in $/CodeJock/CJLibrary
 * Changed base class from CTreeCtrl to CCJTreeCtrl for flicker free
 * drawing.
 * 
 * *****************  Version 10  *****************
 * User: Kirk Stowell Date: 9/13/99    Time: 5:52a
 * Updated in $/CodeJockey/CJLibrary
 * Fixed build errors which occured with UNICODE builds.
 * 
 * *****************  Version 9  *****************
 * User: Kirk Stowell Date: 8/31/99    Time: 4:39a
 * Updated in $/CodeJockey/CJLibrary
 * Updated copyright and contact information.
 * 
 * *****************  Version 8  *****************
 * User: Kirk Stowell Date: 7/25/99    Time: 12:30a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 7  *****************
 * User: Kirk Stowell Date: 6/23/99    Time: 12:33a
 * Updated in $/CodeJockey/CJLibrary
 * 
 * *****************  Version 6  *****************
 * User: Kirk Stowell Date: 6/11/99    Time: 10:14p
 * Updated in $/CodeJockey/CJ60Lib
 * Cleaned up system image list initialization.
 * 
 * *****************  Version 5  *****************
 * User: Kirk Stowell Date: 5/12/99    Time: 5:32a
 * Updated in $/CodeJockey/CJ60Lib
 * Added support for vc5 compatibility
 * 
 * *****************  Version 4  *****************
 * User: Kirk Stowell Date: 5/12/99    Time: 1:41a
 * Updated in $/CodeJockey/CJ60Lib
 * Added wait cursor for lengthly processing.
 * 
 * *****************  Version 3  *****************
 * User: Kirk Stowell Date: 5/02/99    Time: 3:38p
 * Updated in $/CodeJockey/CJ60Lib
 * Added version macro  _WIN32_IE, you will need to have at IE4 or greater
 * installed to use this class. This will ensure that the version of
 * SHELL32.DLL is compatible.
 * 
 * *****************  Version 2  *****************
 * User: Kirk Stowell Date: 3/19/99    Time: 11:05p
 * Updated in $/CodeJockey/CJ60Lib
 * Initial re-write and release.
 * 
 * *****************  Version 1  *****************
 * User: Kirk Stowell Date: 2/24/99    Time: 11:05p
 * Created in $/CodeJockey/CJ60Lib
 * Overloaded PopulateTree(LPCTSTR lpszPath) in the class CCJShellTree to
 * fill the tree based upon path. Takehiko Mizoguti
 * [mizoguti@m2.sys.to.casio.co.jp]
 *
 ***************************************************************************/
/////////////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CJShellTree.h"
#include "CJShellList.h"
#include "CJShell.h"
#include "CJFlatComboBox.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#if _WIN32_IE >= 0x0400 // Need to have at IE4 or greater installed.
#include <Shlwapi.h>

/////////////////////////////////////////////////////////////////////////////
// CCJShellTree

CCJShellTree::CCJShellTree()
{
	m_pListCtrl		= NULL;
	m_pComboBox		= NULL;
	m_bRefresh		= false;
    m_bOutaHere		= false;
	m_htiCurrent	= NULL;
}

CCJShellTree::~CCJShellTree()
{
}

IMPLEMENT_DYNAMIC(CCJShellTree, CCJTreeCtrl)

BEGIN_MESSAGE_MAP(CCJShellTree, CCJTreeCtrl)
	//{{AFX_MSG_MAP(CCJShellTree)
	ON_NOTIFY_REFLECT(NM_RCLICK, OnRclick)
	ON_NOTIFY_REFLECT(TVN_DELETEITEM, OnDeleteitem)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	ON_NOTIFY_REFLECT(TVN_ITEMEXPANDING, OnItemexpanding)
	ON_WM_DESTROY()
	ON_WM_CREATE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCJShellTree message handlers

void CCJShellTree::OnFill(bool bRefresh/*=false*/)
{
    LPSHELLFOLDER lpsf=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;

	m_bRefresh = bRefresh;

    // Get a pointer to the desktop folder.
    hr=SHGetDesktopFolder(&lpsf);
	
    if (SUCCEEDED(hr))
    {
		// Initialize the tree view to be empty.
		DeleteAllItems();
		
		// Fill in the tree view from the root.
		FillTreeView(lpsf,
			NULL,
			TVI_ROOT);
		// Release the folder pointer.
		lpsf->Release();
    }
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;
	
    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
	
	Expand(GetRootItem(), TVE_EXPAND);
}

void CCJShellTree::FillTreeView(LPSHELLFOLDER lpsf, LPITEMIDLIST  lpifq, HTREEITEM hParent)
{
    TV_ITEM         tvi;                          // TreeView Item.
    TV_INSERTSTRUCT tvins;                        // TreeView Insert Struct.
    HTREEITEM       hPrev = NULL;                 // Previous Item Added.
    LPENUMIDLIST    lpe=0;
    LPITEMIDLIST    lpi=0, lpifqThisItem=0;
    LPTVITEMDATA    lptvid=0;
    LPMALLOC        lpMalloc=0;
    ULONG           ulFetched;
    HRESULT         hr;
    TCHAR           szBuff[256];
    HWND            hwnd=GetParent()->GetSafeHwnd();

    hr=SHGetMalloc(&lpMalloc);
    if (FAILED(hr))
       return;

    // Hourglass on!

    SetCapture();
    CWaitCursor wait;   // display wait cursor

    if (SUCCEEDED(hr))
    {
		SetRedraw(FALSE);

        hr=lpsf->EnumObjects(hwnd, 
			SHCONTF_FOLDERS | SHCONTF_NONFOLDERS, &lpe);

        if (SUCCEEDED(hr))
        {
            while (S_OK==lpe->Next(1, &lpi, &ulFetched))
            {
                //Create a fully qualified path to the current item
                //The SH* shell api's take a fully qualified path pidl,
                //(see GetIcon above where I call SHGetFileInfo) whereas the
                //interface methods take a relative path pidl.

                ULONG ulAttrs = SFGAO_HASSUBFOLDER | SFGAO_FOLDER;

                lpsf->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lpi, &ulAttrs);

                if (ulAttrs & (SFGAO_HASSUBFOLDER | SFGAO_FOLDER))
                {
                   //We need this next if statement so that we don't add things like
                   //the MSN to our tree.  MSN is not a folder, but according to the
                   //shell is has subfolders....

                   if (ulAttrs & SFGAO_FOLDER)
                   {
                      tvi.mask            = TVIF_TEXT | TVIF_IMAGE | TVIF_SELECTEDIMAGE |
                                            TVIF_PARAM;

                      if (ulAttrs & SFGAO_HASSUBFOLDER)
                      {
                         //This item has sub-folders, so let's put the + in the TreeView.
                         //The first time the user clicks on the item, we'll populate the
                         //sub-folders then.

                         tvi.cChildren=1;
                         tvi.mask |= TVIF_CHILDREN;
                      }
                        
                      //OK, let's get some memory for our ITEMDATA struct

                      lptvid = (LPTVITEMDATA)lpMalloc->Alloc(sizeof(TVITEMDATA));
                      if (!lptvid)
                         goto Done;
   
                      //Now get the friendly name that we'll put in the treeview...

                      if( m_shell.GetName(lpsf, lpi, SHGDN_NORMAL, szBuff) == FALSE ) {
                         goto Done;
					  }

                      tvi.pszText    = szBuff;
                      tvi.cchTextMax = MAX_PATH;
    
                      lpifqThisItem=m_shell.ConcatPidls(lpifq, lpi);
      
                      //Now, make a copy of the ITEMIDLIST
      
                      lptvid->lpi=m_shell.CopyITEMID(lpMalloc, lpi);
   
                      m_shell.GetNormalAndSelectedIcons(lpifqThisItem, &tvi);
   
                      lptvid->lpsfParent=lpsf;    //Store the parent folders SF
                      lpsf->AddRef();

                      //So, you ask, what's the story here?  Why do we have 2
                      //functions that apparently do the same thing?  Well, 
                      //ParseDisplayName() (used in GetFullyQualPidl) does not 
                      //work well for non-file system objects such as the My 
                      //Computer and Net Hood.  So, since we know that these 
                      //guys will only show up when we are enumerating the root 
                      //of the namespace (as identified by an hParent of TVI_ROOT), 
                      //we special case this here.  You *could* use ConcatPidls() 
                      //in ALL cases, but the GetFullyQualPidl() is shown for 
                      //demonstration purposes, since it's the more intuative way.

//Commented out for now, since we need ConcatPidls for *any* non-file system object.
//This includes things like Control Panel, et al.

//                      if (hParent==TVI_ROOT)
                         lptvid->lpifq=m_shell.ConcatPidls(lpifq, lpi);
//                      else
//                         lptvid->lpifq=GetFullyQualPidl(lpsf, lpi);
   
                      tvi.lParam = (LPARAM)lptvid;
   
                      // Populate the TreeVeiw Insert Struct
                      // The item is the one filled above.
                      // Insert it after the last item inserted at this level.
                      // And indicate this is a root entry.
   				   
                      tvins.item         = tvi;
                      tvins.hInsertAfter = hPrev;
                      tvins.hParent      = hParent;
   
                      // Add the item to the tree
   
                      hPrev = InsertItem(&tvins);
                   }

                   lpMalloc->Free(lpifqThisItem);  
                   lpifqThisItem=0;
                }

                lpMalloc->Free(lpi);  //Finally, free the pidl that the shell gave us...
                lpi=0;
            }
        }

    }
    else {
		SetRedraw();
		return;
	}

Done:
 
    // Hourglass off!

    ReleaseCapture();

    if (lpe)  lpe->Release();

    //The following 2 if statements will only be TRUE if we got here on an
    //error condition from the "goto" statement.  Otherwise, we free this memory
    //at the end of the while loop above.
    if (lpi && lpMalloc)           lpMalloc->Free(lpi);
    if (lpifqThisItem && lpMalloc) lpMalloc->Free(lpifqThisItem);  

    if (lpMalloc) lpMalloc->Release();
	SetRedraw();
}

void CCJShellTree::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
	*pResult = 0;

	POINT			pt;
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	static char		szBuff[MAX_PATH];
	TV_HITTESTINFO	tvhti;
	TV_ITEM			tvi;
	
	::GetCursorPos((LPPOINT)&pt);
	::ScreenToClient(m_hWnd, &pt);
	tvhti.pt=pt;
	HitTest(&tvhti);
	SelectItem(tvhti.hItem);
	if (tvhti.flags & TVHT_ONITEMLABEL)
	{
		::ClientToScreen(m_hWnd, &pt);
		tvi.mask=TVIF_PARAM;
		tvi.hItem=tvhti.hItem;
		
		if (!GetItem(&tvi))
			return;
		
		lptvid=(LPTVITEMDATA)tvi.lParam;
		
		m_shell.DoTheMenuThing(::GetParent(m_hWnd),
			lptvid->lpsfParent, lptvid->lpi, &pt);
	}
}

void CCJShellTree::OnDeleteitem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	HRESULT        hr;
	LPMALLOC       lpMalloc;
	LPTVITEMDATA   lptvid;  //Long pointer to TreeView item data

	//Let's free the memory for the TreeView item data...
	hr=SHGetMalloc(&lpMalloc);
	if (FAILED(hr))
		return;
	
	lptvid=(LPTVITEMDATA)pNMTreeView->itemOld.lParam;
	lptvid->lpsfParent->Release();
	lpMalloc->Free(lptvid->lpi);  
	lpMalloc->Free(lptvid->lpifq);  
	lpMalloc->Free(lptvid);  
	lpMalloc->Release();
}

void CCJShellTree::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	HRESULT			hr;

	SetRedraw(FALSE);
	//Do this only if we are not exiting the application...
	if (!m_bOutaHere)
	{
		lptvid=(LPTVITEMDATA)pNMTreeView->itemNew.lParam;
		if (lptvid)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
				0,IID_IShellFolder,(LPVOID *)&lpsf2);
			
			if (SUCCEEDED(hr) && (m_bRefresh == false))
			{
				// get the current directory.
				CString strFolderPath;
				GetSelectedFolderPath(strFolderPath);
				
				// get a handle to the currently selected item and set the
				// working directory.
				m_htiCurrent = pNMTreeView->itemNew.hItem;
				SetCurrentDirectory( strFolderPath );
				
				if (m_pListCtrl->GetSafeHwnd())
				{
					m_pListCtrl->PopulateListView(lptvid,lpsf2);
					lpsf2->Release();
				}
				if( m_pComboBox->GetSafeHwnd( ))
				{
					int nFound = m_pComboBox->FindString(-1, strFolderPath);
					if (nFound == CB_ERR) {
						m_pComboBox->SetCurSel( m_pComboBox->AddString( strFolderPath ));
					}
					else {
						m_pComboBox->SetCurSel(nFound);
					}
				}
			}
		}
	}

	SetRedraw();
	*pResult = 0;
}

void CCJShellTree::OnItemexpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	*pResult = 0;

	LPTVITEMDATA   lptvid;  //Long pointer to TreeView item data
	HRESULT        hr;
	LPSHELLFOLDER  lpsf2=NULL;
	static char    szBuff[MAX_PATH];
	TV_SORTCB      tvscb;

	if ((pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE))
		return;
	
	SetRedraw(FALSE);
	lptvid=(LPTVITEMDATA)pNMTreeView->itemNew.lParam;
	if (lptvid)
	{
		hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
			0, IID_IShellFolder,(LPVOID *)&lpsf2);
		
		if (SUCCEEDED(hr))
		{
			FillTreeView(lpsf2,
				lptvid->lpifq,
				pNMTreeView->itemNew.hItem);
		}
		
		tvscb.hParent     = pNMTreeView->itemNew.hItem;
		tvscb.lParam      = 0;
		tvscb.lpfnCompare = TreeViewCompareProc;
		
		SortChildrenCB(&tvscb/*, FALSE*/);
	}
	SetRedraw();
}

int CALLBACK CCJShellTree::TreeViewCompareProc(LPARAM lparam1, LPARAM lparam2, LPARAM /*lparamSort*/)
{
    LPTVITEMDATA lptvid1=(LPTVITEMDATA)lparam1;
    LPTVITEMDATA lptvid2=(LPTVITEMDATA)lparam2;
    HRESULT   hr;

    hr = lptvid1->lpsfParent->CompareIDs(0, lptvid1->lpi, lptvid2->lpi);

    if (FAILED(hr))
       return 0;

    return (short)SCODE_CODE(GetScode(hr));
}

BOOL CCJShellTree::InitSystemImageLists()
{
    SHFILEINFO sfi;

    HIMAGELIST hImageList = (HIMAGELIST)::SHGetFileInfo( _T("C:\\"), 0, &sfi, 
		sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON );

    if( hImageList ) {
        SetImageList( CImageList::FromHandle( hImageList ), TVSIL_NORMAL  );
		return TRUE;
    }

    return FALSE;
}

void CCJShellTree::OnDestroy() 
{
	m_bOutaHere	= false;
	CCJTreeCtrl::OnDestroy();
}

void CCJShellTree::TunnelTree(CString strFindPath)
{
	HTREEITEM subNode = GetRootItem();
	CString szPathHop;
	char drive[_MAX_DRIVE];
	char dir[_MAX_DIR];
	char fname[_MAX_FNAME];
	char ext[_MAX_EXT];
	char delimiter[]="\\";

	m_bRefresh = false;
	
	if(!m_shell.Exist(strFindPath))
	{
		if (strFindPath.GetLength() == 3)
		{
		}

		else
		{
			MessageBox(strFindPath,_T("Folder not found"),MB_ICONERROR);
			return;
		}
	}
	
	if(strFindPath.ReverseFind(_T('\\')) != strFindPath.GetLength()-1)
	{
		strFindPath += _T("\\");
	}
	
	m_shell.SplitPath(strFindPath,drive,dir,fname,ext);
	
	//search the drive first
	szPathHop=drive;
	subNode=GetChildItem(subNode);

	if(subNode)
	{
		if(SearchTree(subNode,szPathHop, CCJShellTree::type_drive))
		{
			SetRedraw(FALSE);
			//break down subfolders and search
			char *p=strtok(dir,delimiter);
			while(p)
			{
				subNode = GetSelectedItem();
				Expand(subNode, TVE_EXPAND);
				subNode = GetChildItem(subNode);

				if(SearchTree(subNode,p,CCJShellTree::type_folder))
					p=strtok(NULL,delimiter);
				else
					p=NULL;
			}
			SetRedraw();
		}
	}
}

BOOL CCJShellTree::SearchTree(HTREEITEM treeNode, CString szSearchName, FindAttribs attr)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	char	drive[_MAX_DRIVE];
	char	dir[_MAX_DIR];
	char	fname[_MAX_FNAME];
	char	ext[_MAX_EXT];
	BOOL	bRet=FALSE;
	HRESULT	hr;
	CString	strCompare;
	
	szSearchName.MakeUpper();
	while(treeNode && bRet==FALSE)
	{
		lptvid=(LPTVITEMDATA)GetItemData(treeNode);
		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
				0,IID_IShellFolder,(LPVOID *)&lpsf2);
			if (SUCCEEDED(hr))
			{
				ULONG ulAttrs = SFGAO_FILESYSTEM;
				lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);
				if (ulAttrs & (SFGAO_FILESYSTEM))
				{
					if(SHGetPathFromIDList(lptvid->lpifq,strCompare.GetBuffer(MAX_PATH)))
					{
						switch(attr)
						{
						case type_drive:
							m_shell.SplitPath(strCompare,drive,dir,fname,ext);
							strCompare=drive;
							break;
						case type_folder:
							strCompare = GetItemText(treeNode);
							break;
						}
						strCompare.MakeUpper();
						if(strCompare == szSearchName)
						{
							EnsureVisible(treeNode);
							SelectItem(treeNode);
							bRet=TRUE;
						}
					}
				}
				lpsf2->Release();
			}
		}
		treeNode = GetNextSiblingItem(treeNode);
	}
	return bRet;
}

BOOL CCJShellTree::GetSelectedFolderPath(CString &strFolderPath)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static TCHAR	szBuff[MAX_PATH];
	HTREEITEM		hItem = GetSelectedItem();
	HRESULT			hr;
	BOOL			bRet=FALSE;
	
	if( hItem != NULL )
	{
		lptvid=(LPTVITEMDATA)GetItemData(hItem);
		
		if (lptvid && lptvid->lpsfParent && lptvid->lpi)
		{
			hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
				0,IID_IShellFolder,(LPVOID *)&lpsf2);
			
			if (SUCCEEDED(hr))
			{
				ULONG ulAttrs = SFGAO_FOLDER;
				
				// Determine what type of object we have.
				lptvid->lpsfParent->GetAttributesOf(1,
					(const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);
				
				if (ulAttrs & (SFGAO_FOLDER))
				{
					if(SHGetPathFromIDList(lptvid->lpifq,szBuff)){
						strFolderPath = szBuff;
						bRet = TRUE;
					}
				}
			}
			
		}
		if(lpsf2)
			lpsf2->Release();
	}
	
	return bRet;
}

BOOL CCJShellTree::GetFolderItemPath(HTREEITEM hItem, CString &strFolderPath)
{
	LPTVITEMDATA	lptvid;  //Long pointer to TreeView item data
	LPSHELLFOLDER	lpsf2=NULL;
	static TCHAR	szBuff[MAX_PATH];
	HRESULT			hr;
	BOOL			bRet=FALSE;
	
	lptvid=(LPTVITEMDATA)GetItemData(hItem);
	
	if (lptvid && lptvid->lpsfParent && lptvid->lpi)
	{
		hr=lptvid->lpsfParent->BindToObject(lptvid->lpi,
			0,IID_IShellFolder,(LPVOID *)&lpsf2);
		
		if (SUCCEEDED(hr))
		{
			ULONG ulAttrs = SFGAO_FILESYSTEM;
			
			// Determine what type of object we have.
			lptvid->lpsfParent->GetAttributesOf(1, (const struct _ITEMIDLIST **)&lptvid->lpi, &ulAttrs);
			
			if (ulAttrs & (SFGAO_FILESYSTEM))
			{
				if(SHGetPathFromIDList(lptvid->lpifq,szBuff)){
					strFolderPath = szBuff;
					bRet = TRUE;
				}
			}
		}

		if(lpsf2)
			lpsf2->Release();
	}
	
	return bRet;
}

void CCJShellTree::PopulateTree(LPCTSTR lpszPath)
{
	LPSHELLFOLDER lpsf=NULL,lpsf2=NULL;
    LPITEMIDLIST  lpi=NULL;
    HRESULT hr;
    TV_SORTCB      tvscb;
	LPTSTR			lpFolder = (LPTSTR)lpszPath;
	LPTSTR			lpNextFolder;
	TCHAR			strPath[_MAX_PATH];
	
	LPMALLOC pMalloc;
	if (::SHGetMalloc(&pMalloc) == NOERROR)
	{
		
		// Get a pointer to the desktop folder.
		hr=SHGetDesktopFolder(&lpsf);
		
		if (SUCCEEDED(hr))
		{
			USES_CONVERSION;
			
			// Initialize the tree view to be empty.
			DeleteAllItems();
			
			do{
				
				// Get the Next Component
				lpNextFolder = PathFindNextComponent( lpFolder );
				if( lpNextFolder && *lpNextFolder ){
					memcpy( strPath, lpFolder, ( lpNextFolder - lpFolder ) );
					strPath[lpNextFolder - lpFolder] = _T('\0');
				}
				else{
					_tcscpy( strPath, lpFolder );
					lpNextFolder = NULL;
				}
				
				// Get ShellFolder Pidl
				ULONG eaten;
				hr = lpsf->ParseDisplayName( NULL, NULL, T2OLE(strPath), &eaten, &lpi, NULL );
				if( FAILED( hr ) ){
					break;
				}
				hr=lpsf->BindToObject(lpi, 0, IID_IShellFolder,(LPVOID *)&lpsf2);
				if( FAILED( hr ) ){
					break;
				}
				
				pMalloc->Free( lpi );
				
				// Release the Parent Folder pointer.
				lpsf->Release();
				
				// Chenge Folder Info
				lpsf = lpsf2;
				lpFolder = lpNextFolder;
			}
			while( lpNextFolder );
			
			FillTreeView(lpsf,NULL,TVI_ROOT);
			
		}
	}
    tvscb.hParent     = TVI_ROOT;
    tvscb.lParam      = 0;
    tvscb.lpfnCompare = TreeViewCompareProc;
	
    // Sort the items in the tree view
	SortChildrenCB(&tvscb/*, FALSE*/);
    
	HTREEITEM hItem;
	hItem = GetRootItem();
	Expand(hItem,TVE_EXPAND);
	Select(GetRootItem(),TVGN_CARET);
}

int CCJShellTree::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CCJTreeCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;
	
	// Attach the system image list, and build the tree.
	InitSystemImageLists();
	OnFill();
	
	return 0;
}

BOOL CCJShellTree::PreCreateWindow(CREATESTRUCT& cs) 
{
	if( !CCJTreeCtrl::PreCreateWindow(cs))
		return FALSE;

	cs.style |= TVS_HASLINES|TVS_LINESATROOT|TVS_HASBUTTONS|TVS_SHOWSELALWAYS;

	return TRUE;
}

#endif // (_WIN32_IE >= 0x0400)
