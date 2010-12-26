/*
Module : FileTreeCtrl.h
Purpose: Interface for an MFC class which provides a tree control similiar 
         to the left hand side of explorer

Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////////////// Macros / Defines  //////////////////////////

#pragma once

#ifndef __FILETREECTRL_H__
#define __FILETREECTRL_H__

#ifndef __AFXMT_H__
#pragma message("To avoid this message, put afxmt.h in your pre compiled header (normally stdafx.h)")
#include <afxmt.h>
#endif

#ifndef _SHLOBJ_H_
#pragma message("To avoid this message, put shlobj.h in your pre compiled header (normally stdafx.h)")
#include <shlobj.h>
#endif

#ifndef _LM_
#pragma message("To avoid this message, put lm.h in your pre compiled header (normally stdafx.h)")
#include <lm.h>
#endif

#ifndef __AFXMT_H__
#pragma message("To avoid this message, put afxmt.h in your pre compiled header (normally stdafx.h)")
#include <afxmt.h>
#endif

#ifndef FILETREECTRL_EXT_CLASS
#define FILETREECTRL_EXT_CLASS
#endif

#ifndef FILETREECTRL_EXT_API
#define FILETREECTRL_EXT_API
#endif

#ifndef FILETREECTRL_BASE_CLASS
#define FILETREECTRL_BASE_CLASS CTreeCtrl
#endif


/////////////////////////// Classes ///////////////////////////////////////////

//Class which gets stored int the item data on the tree control
class FILETREECTRL_EXT_CLASS CFileTreeCtrlItemInfo
{
public:
//Enums
  enum NodeType
  {
    DesktopNode,
    NetworkNode,
    MyDocumentsNode,
    MyComputerNode,
    CurrentUserFolderNode,
    FileNode,
    FolderNode,
    DriveNode,
    UnknownNode
  };

//Constructors / Destructors
  CFileTreeCtrlItemInfo();
  CFileTreeCtrlItemInfo(const CFileTreeCtrlItemInfo& ItemInfo);
  virtual ~CFileTreeCtrlItemInfo();

//Member variables
  CString       m_sFQPath;          //Fully qualified path for this item
  CString       m_sRelativePath;    //The "Display name" for this item, Normally will be a relative bit of the path from "m_sFQPath"
  NETRESOURCE*  m_pNetResource;     //Used if this item is under Network Neighborhood
  BYTE          m_NodeType;         //The type of node this is (A BYTE and not a NodeType instance to optimize memory footprint)
  bool          m_bExtensionHidden; //Is the extension being hidden for this item
};


//Class which encapsulates access to the System image list which contains
//all the icons used by the shell to represent the file system
class FILETREECTRL_EXT_CLASS CSystemImageList
{
public:
//Constructors / Destructors
  CSystemImageList();
  virtual ~CSystemImageList();

protected:
//Data
  CImageList m_ImageList;          //The MFC image list wrapper
  static int sm_nRefCount;         //Reference count for the imagelist

  friend class CFileTreeCtrl;      //Allow the FileTreeCtrl access to our internals
};


//Struct taken from svrapi.h as we cannot mix Win9x and Win NT net headers in one program
#pragma pack(push, 1)
struct FILETREECTRL_EXT_CLASS CFileTreeCtrl_share_info_50 
{
	char		        shi50_netname[LM20_NNLEN+1];  //share name
	unsigned char 	shi50_type;                   //see below
  unsigned short	shi50_flags;                  //see below
	char FAR *	    shi50_remark;                 //ANSI comment string
	char FAR *	    shi50_path;                   //shared resource
	char		        shi50_rw_password[SHPWLEN+1]; //read-write password (share-level security)
	char		        shi50_ro_password[SHPWLEN+1]; //read-only password (share-level security)
};	/* share_info_50 */
#pragma pack(pop)


//class which manages enumeration of shares. This is used for determining 
//if an item is shared or not
class FILETREECTRL_EXT_CLASS CShareEnumerator
{
public:
//Constructors / Destructors
  CShareEnumerator();
  virtual ~CShareEnumerator();

//Methods
  void Refresh(); //Updates the internal enumeration list
  BOOL IsShared(const CString& sPath);

protected:
//Defines
  typedef NET_API_STATUS (WINAPI NT_NETSHAREENUM)(LPWSTR, DWORD, LPBYTE*, DWORD, LPDWORD, LPDWORD, LPDWORD);
  typedef NET_API_STATUS (WINAPI NT_NETAPIBUFFERFREE)(LPVOID);
  typedef NET_API_STATUS (WINAPI WIN9X_NETSHAREENUM)(const char FAR *, short, char FAR *, unsigned short, unsigned short FAR *, unsigned short FAR *);

//Data
  BOOL                         m_bWinNT;          //Are we running on NT
  HMODULE                      m_hNetApi;         //Handle to the net api dll
  NT_NETSHAREENUM*             m_pNTShareEnum;    //NT function pointer for NetShareEnum
  NT_NETAPIBUFFERFREE*         m_pNTBufferFree;   //NT function pointer for NetAPIBufferFree
  SHARE_INFO_502*              m_pNTShareInfo;    //NT share info
  WIN9X_NETSHAREENUM*          m_pWin9xShareEnum; //Win9x function pointer for NetShareEnum
  CFileTreeCtrl_share_info_50* m_pWin9xShareInfo; //Win9x share info
  DWORD                        m_dwShares;        //The number of shares enumerated
};


//Class which implements the tree control representation of the file system
class FILETREECTRL_EXT_CLASS CFileTreeCtrl : public FILETREECTRL_BASE_CLASS
{
public:
//Enums
	enum HideFileExtension
	{
		HideExtension,
		DoNoHideExtension,
		UseTheShellSetting
	};

	enum DriveType
	{
		DRIVE_ATTRIBUTE_REMOVABLE = 0x00000001,
		DRIVE_ATTRIBUTE_FIXED     = 0x00000002,
		DRIVE_ATTRIBUTE_REMOTE    = 0x00000004,
		DRIVE_ATTRIBUTE_CDROM     = 0x00000010,
		DRIVE_ATTRIBUTE_RAMDISK   = 0x00000020
	};

//Constructors / Destructors
	CFileTreeCtrl();

//Public methods
  void              SetRootFolder(const CString& sPath);
  CString           GetRootFolder() const { return m_sRootFolder; };
  CString           GetSelectedPath();
  HTREEITEM         SetSelectedPath(const CString& sPath, BOOL bExpanded=FALSE);
  void              SetShowFiles(BOOL bFiles);
  BOOL              GetShowFiles() const { return m_bShowFiles; };
  void              SetShowNetwork(BOOL bShowNetwork);
  BOOL              GetShowNetwork() const { return m_bShowNetwork; };
  void              SetUsingDifferentIconForSharedFolders(BOOL bShowSharedUsingDifferentIcon);
  BOOL              GetUsingDifferentIconForSharedFolders() const { return m_bShowSharedUsingDifferentIcon; };
  void              SetAllowDragDrop(BOOL bAllowDragDrop) { m_bAllowDragDrop = bAllowDragDrop; };
  BOOL              GetAllowDragDrop() const { return m_bAllowDragDrop; };
  void              SetAllowRename(BOOL bAllowRename) { m_bAllowRename = bAllowRename; };
  BOOL              GetAllowRename() const { return m_bAllowRename; };
  void              SetAllowOpen(BOOL bAllowOpen) { m_bAllowOpen = bAllowOpen; };
  BOOL              GetAllowOpen() const { return m_bAllowOpen; };
  void              SetAllowProperties(BOOL bAllowProperties) { m_bAllowProperties = bAllowProperties; };
  BOOL              GetAllowProperties() const { return m_bAllowProperties; };
  void              SetAllowDelete(BOOL bAllowDelete) { m_bAllowDelete = bAllowDelete; };
  BOOL              GetAllowDelete() const { return m_bAllowDelete; };
  void              SetAllowCreateFolder(BOOL bAllowCreateFolder) { m_bAllowCreateFolder = bAllowCreateFolder; };
  BOOL              GetAllowCreateFolder() const { return m_bAllowCreateFolder; };
  void              SetDriveHideFlags(DWORD dwDriveHideFlags);
  DWORD             GetDriveHideFlags() const { return m_dwDriveHideFlags; };
  void              SetFileHideFlags(DWORD dwFileHideFlags);
  DWORD             GetFileHideFlags() const { return m_dwFileHideFlags; };
  void              SetFolderHideFlags(DWORD dwFolderHideFlags);
  DWORD             GetFolderHideFlags() const { return m_dwFolderHideFlags; };
  INT_PTR           AddMultipleFileNameMasks(const CString& sFileNameMasks);
  void              SetFileNameMask(INT_PTR nIndex, const CString& sFileNameMask);
  void              AddFileNameMask(const CString& sFileNameMask);
  void              RemoveFileNameMask(int nIndex) { m_sFileNameMasks.RemoveAt(nIndex); };
  CString           GetFileNameMask(int nIndex) const { return m_sFileNameMasks.GetAt(nIndex); };
  CString           GetFileNameMasks() const;
  void              SetNetworkItemTypes(DWORD dwTypes); 
  DWORD             GetNetworkItemTypes() const { return m_dwNetworkItemTypes; };
  void              SetShowDriveLabels(BOOL bShowDriveLabels); 
  BOOL              GetShowDriveLabels() const { return m_bShowDriveLabels; };
  COLORREF          GetCompressedColor() const { return m_rgbCompressed; };
  void              SetCompressedColor(COLORREF rgbCompressed);
  BOOL              GetUsingDifferentColorForCompressed() const { return m_bShowCompressedUsingDifferentColor; };
  void              SetUsingDifferentColorForCompressed(BOOL bShowCompressedUsingDifferentColor);
  COLORREF          GetEncryptedColor() const { return m_rgbEncrypted; };
  void              SetEncryptedColor(COLORREF rgbEncrypted);
  BOOL              GetUsingDifferentColorForEncrypted() const { return m_bShowEncryptedUsingDifferentColor; };
  void              SetUsingDifferentColorForEncrypted(BOOL bShowEncryptedUsingDifferentColor);
  HideFileExtension GetShowFileExtensions() const { return m_FileExtensions; };
  void              SetShowFileExtensions(HideFileExtension FileExtensions);
  BOOL              GetShowMyComputer() const { return m_bShowMyComputer; };
  void              SetShowMyComputer(BOOL bShowMyComputer);
  BOOL              GetShowDesktop() const { return m_bShowDesktop; };
  void              SetShowDesktop(BOOL bShowDesktop);
  BOOL              GetShowMyDocuments() const { return m_bShowMyDocuments; };
  void              SetShowMyDocuments(BOOL bShowMyDocuments);
  BOOL              GetShowCurrentUserFolder() const { return m_bShowCurrentUserFolder; };
  void              SetShowCurrentUserFolder(BOOL bShowCurrentUserFolder);
  BOOL              GetShowRootedFolder() const { return m_bShowRootedFolder; };
  void              SetShowRootedFolder(BOOL bShowRootedFolder);
  void              SetAutoRefresh(BOOL bAutoRefresh);
  BOOL              GetAutoRefresh() const { return m_nAutoRefresh; };
  virtual CString   ItemToPath(HTREEITEM hItem) const;
  virtual HTREEITEM PathToItem(const CString& sPath) const;
  virtual BOOL      IsFile(HTREEITEM hItem);
  virtual BOOL      IsFolder(HTREEITEM hItem);
  virtual BOOL      IsDrive(HTREEITEM hItem);
  virtual BOOL      IsCompressed(HTREEITEM hItem);
  virtual BOOL      IsEncrypted(HTREEITEM hItem);
  virtual BOOL      IsShared(const CString& sPath);
  virtual BOOL      IsFile(const CString& sPath);
  virtual BOOL      IsFolder(const CString& sPath);
  virtual BOOL      IsDrive(const CString& sPath);
  virtual BOOL      IsCompressed(const CString& sPath);
  virtual BOOL      IsEncrypted(const CString& sPath);
  virtual BOOL      Rename(HTREEITEM hItem);
  virtual BOOL      ShowProperties(HTREEITEM hItem);
  virtual BOOL      Delete(HTREEITEM hItem);
  virtual BOOL      Open(HTREEITEM hItem);
  virtual HTREEITEM CreateFolder(HTREEITEM hItem, const CString& sNewFolderName = _T(""));
  virtual void      PopulateTree(); 
  virtual void      UpOneLevel();
  virtual void      Refresh();
  virtual BOOL      GoBack();
  virtual BOOL      CanGoBack() const { return m_PrevItems.GetSize() != 0; };
  virtual BOOL      GoForward();
  virtual BOOL      CanGoForward() const { return m_NextItems.GetSize() != 0; };
  virtual INT_PTR   GetMaxHistory() const { return m_nMaxHistory; };
  virtual void      SetMaxHistory(INT_PTR nMaxHistory);
  INT_PTR           GetBackSize() const;
  CString           GetBackItemText(INT_PTR nBack) const;
  INT_PTR           GetForwardSize() const;
  CString           GetForwardItemText(INT_PTR nForward) const;
  void              CollapseExpandBranch(HTREEITEM hItem, int nAction);
  void              CollapseAll();
  void              ExpandAll();
  void              Clear();

//Debug / Assert help
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
#ifndef FILETREECTRL_NO_RESOURCES	
	afx_msg void OnContextMenu(CWnd* pWnd, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	afx_msg void OnTimer(UINT_PTR nIDEvent);
#endif  
	afx_msg void OnProperties();
	afx_msg void OnUpdateProperties(CCmdUI* pCmdUI);
	afx_msg void OnRename();
	afx_msg void OnUpdateRename(CCmdUI* pCmdUI);
	afx_msg void OnOpen();
	afx_msg void OnUpdateOpen(CCmdUI* pCmdUI);
	afx_msg void OnDelete();
	afx_msg void OnUpdateDelete(CCmdUI* pCmdUI);
  afx_msg void OnUpdateUpOneLevel(CCmdUI* pCmdUI);
	afx_msg void OnBack();
	afx_msg void OnUpdateBack(CCmdUI* pCmdUI);
	afx_msg void OnForward();
	afx_msg void OnUpdateForward(CCmdUI* pCmdUI);
  afx_msg void OnUpOneLevel();
	afx_msg void OnRefresh();
	afx_msg void OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg void OnDestroy();
	afx_msg void OnCreateFolder();
	afx_msg void OnUpdateCreateFolder(CCmdUI* pCmdUI);
	afx_msg BOOL OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult);
	afx_msg BOOL OnRclick(NMHDR* pNMHDR, LRESULT* pResult);
#ifndef FILETREECTRL_NO_RESOURCES	
	afx_msg BOOL OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult);
#endif	
  afx_msg BOOL OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult);
 	afx_msg BOOL OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg BOOL OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult);
  afx_msg LRESULT OnChange(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()

  DECLARE_DYNCREATE(CFileTreeCtrl)

//Methods
	virtual BOOL        PreTranslateMessage(MSG* pMsg);
	virtual void        PreSubclassWindow();
  virtual void        DisplayPath(const CString& sPath, HTREEITEM hParent, BOOL bUseSetRedraw = TRUE, BOOL bDeleteExistingSiblings = TRUE);
  virtual void        DisplayDrives(HTREEITEM hParent, BOOL bUseSetRedraw=TRUE);
  virtual int         GetIconIndex(const CString& sFilename);
  virtual int         GetIconIndex(HTREEITEM hItem);
  virtual int         GetSelIconIndex(const CString& sFilename);
  virtual int         GetSelIconIndex(HTREEITEM hItem);
  virtual int         GetIconIndex(LPITEMIDLIST lpPIDL);
  virtual int         GetSelIconIndex(LPITEMIDLIST lpPIDL);
  virtual BOOL        HasGotSubEntries(const CString& sDirectory);
  virtual BOOL        HasChildWithText(HTREEITEM hParent, const CString& sText);
  virtual HTREEITEM   InsertFileItem(HTREEITEM hParent, CFileTreeCtrlItemInfo* pItem, BOOL bShared, int nIcon, int nSelIcon, BOOL bCheckForChildren);
  virtual HTREEITEM   FindSibling(HTREEITEM hParent, const CString& sItem) const;
  virtual BOOL        DriveHasRemovableMedia(const CString& sPath);
  BOOL                IsExpanded(HTREEITEM hItem);
#ifndef FILETREECTRL_NO_RESOURCES  
  virtual BOOL        IsDropSource(HTREEITEM hItem);
  BOOL                IsDragging();
  void                EndDragging(BOOL bCancel);
  virtual HTREEITEM   GetDropTarget(HTREEITEM hItem);
#endif
  virtual HTREEITEM   CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter = TVI_LAST);
  virtual HTREEITEM   CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter = TVI_LAST);
  virtual BOOL        CanDisplayFile(const CFileFind& find);
  virtual BOOL        CanDisplayFolder(const CFileFind& find);
  virtual BOOL        CanDisplayDrive(const CString& sDrive);
  virtual BOOL        CanDisplayNetworkItem(CFileTreeCtrlItemInfo* pItem);
  virtual BOOL        CanHandleChangeNotifications(const CString& sPath);
  static int CALLBACK CompareByFilenameNoCase(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
  virtual void        CreateMonitoringThread(const CString& sPath);
  static UINT         _MonitoringThread(LPVOID pParam);
  virtual UINT        MonitoringThread();
  virtual void        KillMonitoringThread();
  virtual BOOL        GetSerialNumber(const CString& sDrive, DWORD& dwSerialNumber);
  virtual BOOL        IsMediaValid(const CString& sDrive);
  virtual void        OnSelectionChanged(NM_TREEVIEW*, const CString&);
  virtual BOOL        EnumNetwork(HTREEITEM hParent);
  virtual CString     GetDriveLabel(const CString& sDrive);
  virtual CString     CreateNewChildFolderName(const CString& sParentFolder);
  CString             GetCorrectedLabel(CFileTreeCtrlItemInfo* pItem);
  HTREEITEM           FindServersNode(HTREEITEM hFindFrom) const;
  BOOL                HasPlusButton(HTREEITEM hItem);
  void                SetHasPlusButton(HTREEITEM hItem, BOOL bHavePlus);
  void                DoExpand(HTREEITEM hItem);
  virtual void        DisplayRootItems();

//Member variables
  CString                                                     m_sRootFolder;
  CString                                                     m_sMyDocumentsPath;
  CString                                                     m_sCurrentUserFolderPath;
  CString                                                     m_sDesktopPath;
  volatile long                                               m_bShowFiles;
  HTREEITEM                                                   m_hRoot;
#ifndef FILETREECTRL_NO_RESOURCES  
  HTREEITEM                                                   m_hItemDrag;
  HTREEITEM                                                   m_hItemDrop;
#endif  
  HTREEITEM                                                   m_hNetworkRoot;
  HTREEITEM                                                   m_hDesktop;
  HTREEITEM                                                   m_hMyDocumentsRoot;
  HTREEITEM                                                   m_hMyComputerRoot;
  HTREEITEM                                                   m_hCurrentUserFolderRoot;
  HTREEITEM                                                   m_hRootedFolder;
  BOOL                                                        m_bShowMyComputer;
  BOOL                                                        m_bShowDesktop;
  BOOL                                                        m_bShowMyDocuments;
  BOOL                                                        m_bShowCurrentUserFolder;
#ifndef FILETREECTRL_NO_RESOURCES  
  CImageList*                                                 m_pilDrag;
#endif  
  UINT_PTR                                                    m_nTimerID;
#ifndef FILETREECTRL_NO_RESOURCES	
  HCURSOR                                                     m_DropCopyCursor;
  HCURSOR                                                     m_NoDropCopyCursor;
  HCURSOR                                                     m_DropMoveCursor;
  HCURSOR                                                     m_NoDropMoveCursor;
#endif  
  UINT                                                        m_TimerTicks;
  BOOL                                                        m_bAllowDragDrop;
  BOOL                                                        m_bAllowRename;
  BOOL                                                        m_bAllowOpen;
  BOOL                                                        m_bAllowProperties;
  BOOL                                                        m_bAllowDelete;
  BOOL                                                        m_bAllowCreateFolder;
  DWORD                                                       m_dwDriveHideFlags;
  DWORD                                                       m_dwFileHideFlags;
  DWORD                                                       m_dwFolderHideFlags;
  CStringArray                                                m_sFileNameMasks;
  COLORREF                                                    m_rgbCompressed;
  BOOL                                                        m_bShowCompressedUsingDifferentColor;  
  COLORREF                                                    m_rgbEncrypted;
  BOOL                                                        m_bShowEncryptedUsingDifferentColor;  
  CArray<HTREEITEM, HTREEITEM>                                m_PrevItems;
  CArray<HTREEITEM, HTREEITEM>                                m_NextItems;
  INT_PTR                                                     m_nMaxHistory;            
  BOOL                                                        m_bUpdatingHistorySelection;
  BOOL                                                        m_bShowNetwork;
  BOOL                                                        m_bShowSharedUsingDifferentIcon;
  HideFileExtension                                           m_FileExtensions;
  DWORD                                                       m_dwMediaID[26];
  IMalloc*                                                    m_pMalloc; 
  IShellFolder*                                               m_pShellFolder;
  DWORD                                                       m_dwNetworkItemTypes;
  BOOL                                                        m_bShowDriveLabels;
  BOOL                                                        m_bShowRootedFolder;
#ifndef FILETREECTRL_NO_CACHE_COMPUTER_ICON  
  int                                                         m_nComputerIconIndex;
  int                                                         m_nSelectedComputerIconIndex;
#endif
  LONG volatile                                               m_nAutoRefresh;
  CString                                                     m_sMonitoringPath;          //The path we are monitoring
  CWinThread*                                                 m_pMonitoringThread;        //The pointer for the monitor thread
  CEvent                                                      m_MonitoringTerminateEvent; //Event using to terminate the thread
};

//MFC Data exchange routines
void FILETREECTRL_EXT_API DDX_FileTreeValue(CDataExchange* pDX, CFileTreeCtrl& ctrlFileTree, CString& sItem);

#endif //__FILETREECTRL_H__
