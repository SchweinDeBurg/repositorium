/*
Module : FileTreeCtrl.cpp
Purpose: Implementation for an MFC class which provides a tree control similiar 
         to the left hand side of explorer
Created: PJN / 25-12-1999
History: PJN / 11-01-2000 1. Added some asserts to HasGotSubEntries
                          2. Fixed a problem with calling OnDblclk when no tree item is selected
                          3. Removed an unused variable from SetSelectedPath
         PJN / 25-01-2000 1. Minor update to the code in CFileTreeCtrl::OnDblclk to only allow 
                          selected items to be opened.
         PJN / 31-01-2000 1. Fixed a problem when you right mouse click over a non - selected item.
                          The control now implements the same behavior as Explorer for this.
                          2. Removed check for keyboard invocation of the context menu in OnContextMenu
                          3. Now displays the context menu over the selected item when invoked via
                          the keyboard. Again this is the same bahavior as Explorer has.
                          4. Optimized the code in PreTranslateMessage
                          5. Fixed a bug in CFileTreeCtrl::OnEndlabeledit
         PJN / 02-04-2000 1. Fixed a small bug in CFileTreeCtrl::SetRootFolder
                          2. Fixed the problem with initialisation errors in the code. Client code must not 
                          explicitly call PopulateTree when the window is created. When used in a dialog
                          resource this is not necessary as it is called for you in the DDX functions.
         PJN / 13-05-2000 1. Fixed a problem where items on the context menu were being invoked for the 
                          wrong item when you right mouse click over an item that is not the selected item.
                          Behaviour now is that the item is selected prior to showing the context menu. 
                          Now, this is same behaviour as Explorer.
         PJN / 12-07-2000 1. Now uses ON_NOTIFY_REFLECT_EX instead of ON_NOTIFY_REFLECT for handling reflected
                          messages. This allows derived classes to handle these messages also. Thanks to 
                          Christian Dahl for this.
                          2. Sample app now allows drag drop behaviour to be toggled
                          3. Fixed a problem whereby two items were left selected after you did a drag /
                          drop operation. Thanks to Jonathon Ralston for this.
                          4. Removed function declaration for unused function "InsertDriveItem".
                          5. Removed an unreferenced variable in InsertFileItem.
                          6. Tidied up the UpOneLevel functions and made it public.
                          7. Removed all the message handlers in the sample code which reflect straight
                          down to the tree control. Instead the OnCmdMsg first routes the message to this
                          class.
                          8. Renamed all menu items which CFileTreeCtrl uses to include the prefix TREEFILECTRL
                          9. Renamed all the methods to more generic names
                          10. PreTranslateMessage now uses PostMessage instead of calling functions directly. 
                          This allows up to function correctly for derived classes in addition to correctly
                          disabling items through the OnUpdate mechanism
                          11. Removed an unreferrenced variable in OnRclick
                          12. Removed the unreferrenced variable m_hSelItem
                          13. Optimized a number of expressions by putting the boolean comparisons first
                          14. m_bAllowRename parameter is now observed for in place editing of an item
                          15. Now supports hiding of Drive types via the SetDriveHideFlags function. See the 
                          menu options on the Tools menu in the sample program for its usage.
                          16. Filename masks can now be specified via the SetFileNameMask method. See the 
                          menu options on the Tools menu in the sample program for its usage.
                          17. File types can now be specified via the GetFileHideFlags function. See the 
                          menu options on the Tools menu in the sample program for its usage.
                          18. Fixed a small issue in one of my calls to ::GetKeyState
                          19. Fixed a bug where programs was crashing if an icon index for it could not
                          be found.
                          20. Made many of the methods of CFileTreeCtrl virtual, thus allowable better
                          use in end user derived classes.
                          21. Fixed problem where SetSelectedPath(_T("C:\\"), FALSE) was resulting 
                          in the drive being expanded even through FALSE was being sent in to specify
                          that the item should not be expanded.
                          22. A virtual "CanDisplayFile" has been added to allow you to decide at runtime
                          whether or not a certain file is to be displayed.
                          23. A virtual "CanDisplayFolder" has been added to allow you to decide at
                          runtime whether or not a certain folder is to be displayed
                          24. Now optionally displays compressed files in a different color, similiar to
                          explorer. The color is customizable through the class API.
                          25. Code has been made smarter so that it does not have to spin up the floppy
                          disk to determine if there are files on it. It now initially displays a "+"
                          and only when you try to expand it will it do the actual scan.
         PJN / 23-07-2000 1. Fixed a bug where the expansion state of the selected item was not being
                          preserved across refreshes.
                          2. Now includes full support for Next / Prev support similiar to Windows 
                          Explorer with the Desktop Update.
                          3. Updated sample app to have some useful toolbars.
                          4. Changing any tree settings which can affect its appearance now force
                          a refresh of its contents.
                          5. ItemToPath method has been made const.
                          6. Addition of PathToItem method
                          7. Auto refresh of items is now provided for by means of change notification
                          threads. This is configurable via the SetAutoRefresh method.
                          8. The root folder of the tree control can now be changed from the sample app
                          9. Fixed a bug in SetRootFolder when passed an empty folder
                          10. Fixed a bug where the system image list was not being initialized correctly
                          if the user did not have a "C:\\" drive. This could occur on NT/Windows2000
                          11. Fixed a bug in IsFile and IsFolder which was causing invalid files or folders
                          to appear valid.
                          12. Deleted items are now removed upon expansion. Also if the item being expanded was
                          deleted and it was the only child, then its parent has the "-" expansion button removed
                          13. Removable drive nodes are collapsed back to their root nodes if their media is 
                          changed in the intervening time when a node expansion occurs.
                          14. Wait cursor is now displayed while a refresh is taking place.
                          15. A "OnSelectionChanged" virtual function has now been provided
                          16. Sample app's icon has been made the same as Explorers.
                          17. Sample app now displays the path name of the currently selected item in the tree control.
                          18. Fixed a bug in IsCompressed
                          19. items are now deleted when selected if they do not exist.
         PJN / 05-09-2000 1. Fixed a bug in CFileTreeCtrl::IsFile and CFileTreeCtrl::IsFolder
         PJN / 20-09-2000 1. Control now includes DECLARE_DYNCREATE thereby allowing it to be used
                          in code which requires this such as Stingray's SEC3DTabWnd. Thanks to Petter Nilsen for
                          pointing out this omission
         PJN / 02-10-2000 1. Fixed a stack overwrite problem in CSystemImageList::CSystemImageList
                          2. Removed an unreferrenced variable in CFileTreeCtrl::OnSelChanged
                          3. Removed an unreferrenced variable in CFileTreeCtrl::OnItemExpanding
                          4. Changed the SendMessage in CFileTreeCtrl::OnDblClk to prevent a crash 
                          which was occurring when the open call sometimes caused a refresh call 
                          which changed the tree items at times. When the double click message handler
                          continued it then caused item expand notifications for items already deleted
                          and of course crashes.
                          5. Removed an unreferrenced variable in CFileTreeCtrl::EndDragging
                          6. Removed an unreferrenced variable in CFileTreeCtrl::KillNotificationThread
                          7. Sample app now remembers the selected path and its expansion state across
                          invocations.
         PJN / 05-05-2001 1. Updated copyright message.
                          2. Fixed a resource leak where icon resources were not being released. Thanks to Jay Kohler for
                          spotting this problem
         PJN / 05-08-2001 1. You can now optionally display Network Neighborhood
                          2. You can now turn on / off display of file extensions.
                          3. You can now display shared items with a different icon
                          4. Friendly names can now be displayed for drives.
         PJN / 11-08-2001 1. Improved checking to see if action is allowed in Rename and Delete
                          2. Fixed a bug in OnBeginLabelEdit
                          3. Fixed a problem in OnEndLabelEdit which was causing renames to fail when filename extensions 
                          were not being shown.
         PJN / 11-08-2001 1. Fixed a bug in OnSelChanged which was causing a crash when you right click on an empty area of the control.
                          Thanks to Eli Fenton for spotting this one.
                          2. The control now by default shows drives as children of "My Computer" just like in Explorer.
                          3. When you display a rooted directory in the control, you now have the option of displaying the root
                          folder in the control as the root item.  Thanks to Eli Fenton for suggesting this.
         PJN / 26-10-2001 1. Fixed some stability problems with the code. This was due to adding items to the system image list.
                          This is normally a very bad thing. Instead now the code uses TreeView custom draw (just like the blue color
                          for compressed items) to draw the icons for the Network Neighborhood items. Thanks to Darken Screamer and
                          Timo Haberkern for spotting this problem.
         PJN / 24-12-2001 1. Fixed a copy and paste bug in GoForward. Thanks to Michael T. Luongo for this fix.
                          2. Now allows encrypted files to be displayed in a different color
                          3. Fixed memory leak which was occurring when control was being used in a dialog
                          4. Fixed a problem with the calculation of indents when the style "TVS_LINESATROOT" is used.
         PJN / 16-02-2002 1. Updated copyright message
                          2. Fixed a drag/drop problem which caused the tree state to be inconsistent after the file was dropped.
                          3. Fixed a bug in the refresh code which was causing it to not reselect the selected node
                          after the refresh occurs. Thanks to John Noël for this fix.
                          4. Fixed a problem where the custom draw icons for network nodes were not being drawn in the correct
                          positions when scrollbars were present in the control. Again thanks to John Noël for this fix.
                          5. Fixed a bug in SetSelectedPath which would not display the correct selection if the node we 
                          want to select has been deleted due to the node becoming deleted when it was previously collapsed.
                          Thanks to Franz Fackelmann and John Noël for spotting this problem.
         PJN / 05-06-2002 1. Implemented function "SetUsingDifferentColorForEncrypted" which was declared but had no
                          implementation.
                          2. Fixed report of uninitialized member variable "m_nTimerID". Thanks to Emil Isberg for spotting this.
         PJN / 07-08-2002 1. Fixed a bug in the sample app which ships with the class which was occuring when you compiled
                          the code in Visual Studio.Net. This was due to MS changing the location oleimpl2.h header file. Thanks
                          to Darren Schroeder for spotting this problem.
         PJN / 22-09-2002 1. Removed a number of unreferrenced variables from the code, as highlighted by Visual Studio.Net. Thanks
                          to Bill Johnson for spotting this.
         PJN / 31-12-2002 1. Items are now sorted using the TVM_SORTCHILDRENCB Tree Control message. This means that the class
                          is no longer dependent on the CSortedArray class. That class has been left in this download for 
                          convenience only. Thanks to "Vladi" for this suggestion. 
         PJN / 26-02-2003 1. Fixed a bug in CFileTreeCtrl::PathToItem which was causing refreshes to fail when a rooted folder
                          was being displayed. Thanks to Michael J. Ryan for spotting this.
         PJN / 26-02-2003 1. Fixed more problems with refreshing and rooted paths. Thanks to Michael J. Ryan for spotting this.
         PJN / 31-05-2003 1. Fixed a bug where the filename mask was being ignored.
         PJN / 10-06-2003 1. Custom draw is now not used for items underneath Network Neighborhood. This means that some items there will
                          just display the standard network neighborhood icon. To resolve this issue you would need to implement
                          a completely new solution using the shell API's. Thanks to John Noël for reporting a custom draw bug in this
                          area which has forced this change.
                          2. m_sFQPath is now not filled in for virtual items such as My Computer and Network Neighborhood. Thanks
                          to Hans Dietrich for this suggestion.
         PJN / 05-07-2003 1. Updated the WM_DEVICECHANGE message handler in the sample app to use the correct function signature for
                          Visual Studio 6 and later. Thanks to Ed Eichman for reporting this issue.
                          2. Fixed a bug in the documentation of CFileTreeCtrl::SetDriveHideFlags. Thanks to Ed Eichman for 
                          reporting this issue.
                          3. Now documents the functions SetAutoRefresh and GetAutoRefresh. In addition by default auto refresh is now
                          turned off. Thanks to Ed Eichman for pointing this out.
         PJN / 13-01-2004 1. Used a new form of pragma pack to avoid resetting the packing size to the default when it is currently
                          set to a non standard value.
         PJN / 09-07-2004 1. Fixed a memory leak in the NETRESOURCE structure if client code returned FALSE from 
                          CFileTreeCtrl::CanDisplayNetworkItem. Thanks to Michael Oerder reporting this problem.
         PJN / 17-04-2005 1. Addition of TVIF_HANDLE to the mask fields in the functions GetIconIndex(HTREEITEM hItem), 
                          GetSelIconIndex(HTREEITEM hItem) and SetHasPlusButton(HTREEITEM hItem, BOOL bHavePlus). This avoids ASSERTs
                          when the class is derived from the SECTreeCtrl class in the Stingray Objective Toolkit. Thanks to Greg Marr
                          for reporting this problem.
                          2. Addition of a "FILETREECTRL_EXT_CLASS" define to the class defintions. This makes the code easier to use
                          in an extension DLL.
                          3. Code now uses the TVITEM structure name rather than TV_ITEM which is in line with what is recommended by MS.
                          4. Code now uses the TVINSERTSTRUCT structure name rather than TV_INSERTSTRUCT which is in line with what is 
                          recommended by MS.
                          5. Class now uses a preprocessor define of FILETREECTRL_BASE_CLASS to define the class which CFileTreeCtrl
                          is derived from. This allows the class to be used with the Stingray Objective Toolkit without needing to 
                          modify the CFileTreeCtrl source code.
                          6. Optimized the code which deletes child nodes by now using TVE_COLLAPSERESET flag.
                          7. Removed the unused function NumberOfChildItems.
                          8. Code now uses the NMTREEVIEW structure name rather than NM_TREEVIEW which is in line with what is 
                          recommended by MS.
                          9. Removed the now unused DeleteChildren method.
         PJN / 21-06-2005 1. Fixed a bug where drag and drag mechanism had problems when trying to drag an item into a folder immediately
                          after the root item. Thanks to Larry Paul for reporting this bug and provinding the fix.
                          2. Fixed a number of compiler warnings when the code is compiled using Visual Studio .NET 2003
         PJN / 25-01-2006 1. Updated copyright details.
                          2. Removed the GetChecked / SetChecked methods as CTreeCtrl as of VC++ 6 includes equivalent functionality. 
                          3. The CanDisplayFolder method has been updated to use logic similar to the CanDisplayFile virtual function.
                          This new functionality is controlled by a new m_dwFolderHideFlags variable which can be modified via the 
                          functions SetFolderHideFlags & GetFolderHideFlags. Thanks to Bob O'Neil for this update.
                          4. Reviewed all TRACE statements for correctness.
                          5. Fixed an issue where you get an ASSERT in the code when you are using XP manifest support and you are using
                          the TVS_CHECKBOXES style in your CFileTreeCtrl. The issue seems to be a bug in Commctrl v6 custom draw 
                          notifications as this problem does not occur if you are not using XP themes. Thanks to Bob O'Neill for this update.
         PJN / 18-06-2006 1. Replaced all calls to ZeroMemory with memset
                          2. Replaced all calls to CopyMemory with memcpy
                          3. Optimized CFileTreeCtrlItemInfo constructor code
                          4. Now includes code to optionally show "Desktop" as the root node. Thanks to Hans Dietrich for this very nice 
                          addition
                          5. Removed some now deleted files from the project.
                          6. Fixed a minor typo in the Allow Properties menu item in the sample app.
                          7. Now includes code to optionally show "My Documents". Thanks to Hans Dietrich for this very nice addition.
                          8. Fixed bug where m_bExtensionHidden setting was not being copied in CFileTreeCtrlItemInfo constructor
                          9. Renamed SetDisplayNetwork to SetShowNetwork and GetDisplayNetwork to GetShowNetwork for consistency with
                          all the other functions of the same type.
         PJN / 22-06-2006 1. Fixed a bug where expansion of the Desktop node fails on subsequent expansions. Thanks to Jan Moorman for
                          reporting this issue.
         PJN / 01-07-2006 1. Fixed a bug where expansion of UNC folders failed. Thanks to Jan Moorman for reporting this bug.
         PJN / 01-07-2006 1. Updated the code to clean compile on VC 2005     
                          2. Code now uses new C++ style casts rather than old style C casts where necessary.   
         PJN / 17-03-2007 1. Updated copyright details.  
                          2. Updated the sample apps to clean compile on VC 2005
                          3. Added a preprocessor define "FILETREECTRL_NO_RESOURCES" which allows you to use the CFileTreeCtrl code without
                          the need to pull in any resources. Please note that this will lead to reduced functionality of the control, for
                          example drag and drag will not be supported and all the menu functionality will also be excluded. For exact
                          details on what is out when you define this value, just look for all occurences of FILETREECTRL_NO_RESOURCES
                          in the FileTreeCtrl.cpp module. Thanks to saukwan chou for reporting this issue.
                          4. Optimized CFileTreeCtrl constructor code.
                          5. Optimized CFileTreeCtrlThreadInfo constructor code.
                          6. Optimized the code which gets the icon and "selected" icon for network servers which are displayed by the class.
                          What the code does is cache the local machine name in the constructor and uses it to get the icon for a server
                          when the code is enumerating servers. This does mean that if MS ever brings in a mechanism to allow customization
                          of the icon on a server by server level, then CFileTreeCtrl will ignore this. Thanks to Fotis Xomeritakis for
                          providing this optimization which really helps speed up the performance of CFileTreeCtrl when browsing domains or 
                          workgroups with a large number of workstations and servers.
                          7. Optimized CShareEnumerator constructor code.
         PJN / 31-03-2007 1. The icons used for computer nodes in Network neighborhood is now cached in the CFileTreeCtrl constructor. This is a 
                          further improvement over the caching of the local computer name approach which was used in the last version of the
                          code. If you do not want to use this approach, then you can define a FILETREECTRL_CACHE_COMPUTER_ICON preprocessor
                          value which will result in the computer icon being found when actual network enumeration is performed. Thanks to
                          Fotis Xomeritakis for this nice optimization.
         PJN / 09-04-2007 1. Updated IsDropSource method to now allow drag / drop of folders. Thanks to Kirk A. Baker for reporting this 
                          useful update.
                          2. The class now supports multiple filename masks. To support this new functionality, an updated / new set of 
                          functions have been provided, namely: AddMultipleFileNameMasks, SetFileNameMask, AddFileNameMask, RemoveFileNameMask, 
                          GetFileNameMask & GetFileNameMasks. Thanks to Fedor Gavrilov for this nice update.
         PJN / 22-04-2007 1. Fixed a bug in CFileTreeCtrl::OnItemExpanding when the control is created with the TVS_SINGLEEXPAND style. What
                          was happening was that if for example you selected C:\program files using SetSelectedPath, then you selected "C:\"
                          using the same function and then you selected "D:\", the control will end up showing no plus button on the C node.
                          Thanks to John Emmas for reporting this quite difficult to find bug.
         PJN / 06-06-2007 1. Now supports in place creation of folders. Thanks to Phil Gibson for prompting this update.
         PJN / 09-06-2007 1. Updated comments in documentation about usage of the Platform SDK.
                          2. Internal WM_TREEUPDATE_CHANGE message is now defined as WM_APP instead of WM_USER
                          3. CreateFolder method now takes a second parameter which allows the new child folder name to be set. If you leave 
                          this parameter as the default value of _T("") then the new folder name will be created by calling the virtual 
                          CreateNewChildFolderName function. This function has also been updated in this release to better encapsulate how a new
                          folder name is created.
                          4. Return value from CreateFolder method is now a HTREEITEM instead of a BOOL
                          5. Reworked the internal logic of CreateFolder method to avoid the need to call the Refresh method. As a result the
                          user experience is much improved due to the elimination of the tree updates.
         PJN / 13-06-2007 1. Reviewed class code for use of C++ casts instead of old style C casts.
                          2. Desktop node item now includes a file system path in its CFileTreeCtrlItemInfo pointer
                          3. Made all BOOL members of CFileTreeCtrlItemInfo a "bool" to conserve memory usage
                          4. Replaced CFileTreeCtrlItemInfo member variables "m_bDesktopNode", "m_bNetworkNode" & "m_bMyDocumentNode" with a 
                          single enum member variable
                          5. Fixed a bug where rename would previously be allowed for nodes such as My Documents, My Computers etc
                          6. Fixed a bug where delete would previously be allowed for nodes such as My Documents, My Computers etc
                          7. Desktop node now also includes files and folders as found on the desktop. Also the ordering of items on the 
                          desktop is consistent with how Explorer displays the desktop.
                          8. The shell is notified that a new directory has been created in CreateFolder via the SHChangeNotify API
         PJN / 16-10-2007 1. Fixed a bug where if you attempt to create a new folder within a drive and then immediately rename that folder 
                          to something else, the shell throws an error about renaming a file and changing it's parent drive. Thanks to 
                          Michael Stephenson for reporting this bug.
                          2. Fixed a bug where if you delete a folder and it is the only folder under it's parent folder, the parent retains it's 
                          "minus" icon, even though it no longer has any children. Thanks to Michael Stephenson for reporting this bug.
                          3. Fixed a bug where if you create a folder in a parent folder which did not have any child items, the parent folder
                          would not show the "minus" icon correctly. Thanks to Michael Stephenson for reporting this bug.
         PJN / 02-05-2008 1. Updated copyright details.
                          2. Updated the sample app to show how to implement a Vista style explorer tree view if it is running on Vista. This
                          functionality is implemented at the client app level instead of the control code itself, so you can use this code
                          from the sample app if you want your client app's file tree control to look like Vista's explorer. 
                          3. Removed VC 6 style appwizard comments from the code.
                          4. General review and cleanup of all the code
                          5. The code has now been updated to support VC 2005 or later only. 
                          6. Optimized CFileTreeCtrlItemInfo copy constructor code
                          7. Removed CFileTreeCtrl::SetFlags method as it did not add any functionality to the class.
                          8. DRIVE_ATTRIBUTE_* defines are now enums in the CFileTreeCtrl class.
                          9. Fixed bug in DisplayPath where if the desktop does not contain any files or folders, you would not get a plus
                          button for the desktop. Thanks to Steven Tomer for reporting this bug.
                          10. Prev and Next item stacks now store HTREEITEM's directly in their arrays.
                          11. The control now supports showing the current users folder instead of (or in addition to) the "My Documents" 
                          folder, similar to how the tree control behaves in Windows Explorer on Vista. Note that the control does not 
                          show the "Public" folder which Vista's Windows Explorer shows, because this is implemented as a shell extension 
                          whereas this control is a ground up implementation of a file tree control which does not use the Shell API's
                          for enumerating the Shell Namespace.
                          12. All the classes have been renamed from CTreeFileCtrl... to CFileTreeCtrl...
         PJN / 07-07-2008 1. Code now compiles cleanly using Code Analysis (/analyze)
                          2. Updated code to compile correctly using _ATL_CSTRING_EXPLICIT_CONSTRUCTORS define
                          3. Shares shown underneath the Network node no longer show the share icon overlap. This is consistent with how
                          Windows Explorer shows these items.
                          4. Fixed a bug where CFileTreeCtrl::OnChange was not being called due to the fact that its message map entry
                          was declared incorrectly. Thanks to "Roland" for reporting this bug.
                          5. Fixed a bug in CFileTreeCtrl::SetSelectedPath where the expansion state of a folder was not being correctly
                          set.
                          6. Updated the sample app to allow the expansion state of the selected path to be set.
                          7. Fixed an issue where files and folders were not being displayed when the desktop node was being reexpanded.
                          8. Fixed an issue setting up m_sFQPath for some network node items in EnumNetwork
                          9. During testing of the SetSelectedPath and because the control primarily uses file paths to identify a node,
                          refresh of items which do not have a corresponding path will fail in OnRefresh. Examples of this include 
                          "My Network Places" and "My Computer" amongst others. Until I develop a new version of the control based on 
                          Shell PIDLs, this will be a deficiency of the current approach taken by CFileTreeCtrl
                          10. Following analysis of the auto refresh behaviour of Windows Explorer, the control now only monitors 
                          notifications for changes in the current directory and not all drive letters or recursively from the current
                          directory. This helps to improve the robustness of control when change notifications are received
         PJN / 13-12-2008 1. Optimized the memory usage by removing the m_bFileItem member variable from CFileTreeCtrlItemInfo. Instead
                          now whether an item is a File (as opposed to a folder) is encoded into the m_NodeType value. In addition the
                          m_nodeType value has been made a BYTE value. This helps to save an additional 4 bytes per node. Thanks to 
                          Michael Mötje for prompting this update.
         PJN / 08-01-2009 1. Updated copyright details.
                          2. Fixed a bug in DisplayPath when it calls InsertFileItem to show the root folder. The 
                          CFileTreeCtrlItemInfo pointer passed to InsertFileItem was not being initialized with a valid
                          m_NodeType value. Thanks to Colin Bouckaert for reporting this bug.
                          3. Fixed a bug in SetSelectedPath if you are trying to select a path when SetRootFolder and 
                          SetShowRootedFolder have previously been called. Thanks to Colin Bouckaert for reporting this 
                          bug.

Copyright (c) 1999 - 2009 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


/////////////////////////////////  Includes  //////////////////////////////////

#include "stdafx.h"
#ifndef FILETREECTRL_NO_RESOURCES
#include "FileTreeCtrl.rh"
#endif
#include "FileTreeCtrl.h"


//////////////////////////////// Defines / Locals /////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const UINT WM_TREEUPDATE_CHANGE = ::RegisterWindowMessage(_T("FileTreeCtrlUpdateChange"));
const int FILETREECTRL_TIMERID = 1;
const int FILETREECTRL_TIMERTICK_FOR_EXPANSION = 3;
int CSystemImageList::sm_nRefCount = 0; //Initialise the reference count
CSystemImageList theSystemImageList;    //The one and only system image list instance
CShareEnumerator theSharedEnumerator;   //The one and only share enumerator

//Pull in the WNet Lib automatically
#pragma comment(lib, "mpr.lib")


////////////////////////////// Implementation /////////////////////////////////

CFileTreeCtrlItemInfo::CFileTreeCtrlItemInfo() : m_pNetResource(NULL),
                                                 m_NodeType(UnknownNode),
                                                 m_bExtensionHidden(false)
{
}

CFileTreeCtrlItemInfo::CFileTreeCtrlItemInfo(const CFileTreeCtrlItemInfo& ItemInfo)
{
  m_sFQPath          = ItemInfo.m_sFQPath;
  m_sRelativePath    = ItemInfo.m_sRelativePath;
  m_bExtensionHidden = ItemInfo.m_bExtensionHidden;
  m_NodeType         = ItemInfo.m_NodeType;
  if (ItemInfo.m_pNetResource)
  {
	  m_pNetResource = new NETRESOURCE;

    //Copy the direct member variables of NETRESOURCE
    memcpy(m_pNetResource, ItemInfo.m_pNetResource, sizeof(NETRESOURCE)); 

    //Duplicate the strings which are stored in NETRESOURCE as pointers
    if (ItemInfo.m_pNetResource->lpLocalName)
		  m_pNetResource->lpLocalName	= _tcsdup(ItemInfo.m_pNetResource->lpLocalName);
    if (ItemInfo.m_pNetResource->lpRemoteName)
		  m_pNetResource->lpRemoteName = _tcsdup(ItemInfo.m_pNetResource->lpRemoteName);
    if (ItemInfo.m_pNetResource->lpComment)
		  m_pNetResource->lpComment	= _tcsdup(ItemInfo.m_pNetResource->lpComment);
    if (ItemInfo.m_pNetResource->lpProvider)
		  m_pNetResource->lpProvider	= _tcsdup(ItemInfo.m_pNetResource->lpProvider);
  }
  else
    m_pNetResource = NULL; 
}

CFileTreeCtrlItemInfo::~CFileTreeCtrlItemInfo()
{
  if (m_pNetResource)
  {
    if (m_pNetResource->lpLocalName)
      free(m_pNetResource->lpLocalName);
    if (m_pNetResource->lpRemoteName)
      free(m_pNetResource->lpRemoteName);
    if (m_pNetResource->lpComment)
      free(m_pNetResource->lpComment);
    if (m_pNetResource->lpProvider)
      free(m_pNetResource->lpProvider);
    delete m_pNetResource;
    m_pNetResource = NULL;
  }
}


CSystemImageList::CSystemImageList()
{
  ASSERT(sm_nRefCount == 0); //Should only every be one instance of CSystemImageList declared
  ++sm_nRefCount;

  //Get the temp directory. This is used to then bring back the system image list
  TCHAR pszTempDir[_MAX_PATH];
  VERIFY(GetTempPath(_MAX_PATH, pszTempDir));
  TCHAR pszDrive[_MAX_DRIVE + 1];
  _tsplitpath_s(pszTempDir, pszDrive, sizeof(pszDrive)/sizeof(TCHAR), NULL, 0, NULL, 0, NULL, 0);
  size_t nLen = _tcslen(pszDrive);
  if (pszDrive[nLen-1] != _T('\\'))
    _tcscat_s(pszDrive, sizeof(pszDrive)/sizeof(TCHAR), _T("\\"));

  //Attach to the system image list
  SHFILEINFO sfi;
  HIMAGELIST hSystemImageList = reinterpret_cast<HIMAGELIST>(SHGetFileInfo(pszTempDir, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON));
  VERIFY(m_ImageList.Attach(hSystemImageList));
}

CSystemImageList::~CSystemImageList()
{
  //Decrement the reference count
  --sm_nRefCount;

  //Detach from the image list to prevent problems on Windows 9x where
  //the system image list is shared across processes
  m_ImageList.Detach();
}

CShareEnumerator::CShareEnumerator() : m_pNTShareEnum(NULL),
                                       m_pWin9xShareEnum(NULL),
                                       m_pNTBufferFree(NULL),
                                       m_pNTShareInfo(NULL),
                                       m_pWin9xShareInfo(NULL),
                                       m_hNetApi(NULL),
                                       m_dwShares(0)

{
  //Determine if we are running on the NT kernel or Win9x
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  m_bWinNT = (GetVersionEx(&osvi) && osvi.dwPlatformId == VER_PLATFORM_WIN32_NT);
  if (m_bWinNT)
  {
    //Load up the NETAPI DLL
    m_hNetApi = LoadLibrary(_T("NETAPI32.dll"));
    if (m_hNetApi)
    {
      //Get the required function pointers
      m_pNTShareEnum = reinterpret_cast<NT_NETSHAREENUM*>(GetProcAddress(m_hNetApi, "NetShareEnum"));
      m_pNTBufferFree = reinterpret_cast<NT_NETAPIBUFFERFREE*>(GetProcAddress(m_hNetApi, "NetApiBufferFree"));
    }
  }
  else
  {
    //Load up the SRVAPI DLL
    m_hNetApi = LoadLibrary(_T("SVRAPI.dll"));
    if (m_hNetApi)
    {
      //Get the required function pointer
      m_pWin9xShareEnum = reinterpret_cast<WIN9X_NETSHAREENUM*>(GetProcAddress(m_hNetApi, "NetShareEnum"));
    }
  }

  //Update the array of shares we know about
  Refresh();
}

CShareEnumerator::~CShareEnumerator()
{
  if (m_bWinNT)
  {
    //Free the buffer if valid
    if (m_pNTShareInfo)
      m_pNTBufferFree(m_pNTShareInfo);
  }
  else
    //Free up the heap memory we have used
		delete [] m_pWin9xShareInfo;      

  //Free the dll now that we are finished with it
  if (m_hNetApi)
  {
    FreeLibrary(m_hNetApi);
    m_hNetApi = NULL;
  }
}

void CShareEnumerator::Refresh()
{
  m_dwShares = 0;
  if (m_bWinNT)
  {
    //Free the buffer if valid
    if (m_pNTShareInfo)
      m_pNTBufferFree(m_pNTShareInfo);

    //Call the function to enumerate the shares
    if (m_pNTShareEnum)
    {
      DWORD dwEntriesRead = 0;
      m_pNTShareEnum(NULL, 502, reinterpret_cast<LPBYTE*>(&m_pNTShareInfo), MAX_PREFERRED_LENGTH, &dwEntriesRead, &m_dwShares, NULL);
    }
  }
  else
  {
    //Free the buffer if valid
    if (m_pWin9xShareInfo)
      delete [] m_pWin9xShareInfo;

    //Call the function to enumerate the shares
    if (m_pWin9xShareEnum)
    {
      //Start with a reasonably sized buffer
      unsigned short cbBuffer = 1024;
      BOOL bNeedMoreMemory = TRUE;
      BOOL bSuccess = FALSE;
      while (bNeedMoreMemory && !bSuccess)
      {
        unsigned short nTotalRead = 0;
        m_pWin9xShareInfo = reinterpret_cast<CFileTreeCtrl_share_info_50*>(new BYTE[cbBuffer]);
        memset(m_pWin9xShareInfo, 0, cbBuffer);
        unsigned short nShares = 0;
        NET_API_STATUS nStatus = m_pWin9xShareEnum(NULL, 50, reinterpret_cast<char FAR *>(m_pWin9xShareInfo), cbBuffer, static_cast<unsigned short FAR *>(&nShares), static_cast<unsigned short FAR *>(&nTotalRead));
        if (nStatus == ERROR_MORE_DATA)
		    {            
          //Free up the heap memory we have used
		      delete [] m_pWin9xShareInfo;      

          //And double the size, ready for the next loop around
          cbBuffer *= 2;
		    }
        else if (nStatus == NERR_Success)
        {
          m_dwShares = nShares;
          bSuccess = TRUE;
        }
        else
          bNeedMoreMemory = FALSE;
      }
    }
  }
}

BOOL CShareEnumerator::IsShared(const CString& sPath)
{
  //Assume the item is not shared
  BOOL bShared = FALSE;

  if (m_bWinNT)
  {
    if (m_pNTShareInfo)
    {
      for (DWORD i=0; i<m_dwShares && !bShared; i++)
      {
        CString sShare(m_pNTShareInfo[i].shi502_path);
        bShared = (sPath.CompareNoCase(sShare) == 0) && ((m_pNTShareInfo[i].shi502_type == STYPE_DISKTREE) || ((m_pNTShareInfo[i].shi502_type == STYPE_PRINTQ)));
      }
    }
  }  
  else
  {
    if (m_pWin9xShareInfo)
    {
      for (DWORD i=0; i<m_dwShares && !bShared; i++)
      {
        CString sShare(m_pWin9xShareInfo[i].shi50_path);
        bShared = (sPath.CompareNoCase(sShare) == 0) && ((m_pWin9xShareInfo[i].shi50_type == STYPE_DISKTREE) || ((m_pWin9xShareInfo[i].shi50_type == STYPE_PRINTQ)));
      }
    }
  }

  return bShared;
}


IMPLEMENT_DYNCREATE(CFileTreeCtrl, FILETREECTRL_BASE_CLASS)

BEGIN_MESSAGE_MAP(CFileTreeCtrl, FILETREECTRL_BASE_CLASS)
#ifndef FILETREECTRL_NO_RESOURCES	
	ON_COMMAND(ID_TREEFILECTRL_PROPERTIES, OnProperties)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_PROPERTIES, OnUpdateProperties)
	ON_COMMAND(ID_TREEFILECTRL_RENAME, OnRename)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_RENAME, OnUpdateRename)
	ON_COMMAND(ID_TREEFILECTRL_OPEN, OnOpen)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_OPEN, OnUpdateOpen)
	ON_COMMAND(ID_TREEFILECTRL_DELETE, OnDelete)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_DELETE, OnUpdateDelete)
	ON_COMMAND(ID_TREEFILECTRL_CREATEFOLDER, OnCreateFolder)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_CREATEFOLDER, OnUpdateCreateFolder)
	ON_COMMAND(ID_TREEFILECTRL_REFRESH, OnRefresh)
	ON_COMMAND(ID_TREEFILECTRL_UPONELEVEL, OnUpOneLevel)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_UPONELEVEL, OnUpdateUpOneLevel)
	ON_COMMAND(ID_TREEFILECTRL_BACK, OnBack)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_BACK, OnUpdateBack)
	ON_COMMAND(ID_TREEFILECTRL_FORWARD, OnForward)
	ON_UPDATE_COMMAND_UI(ID_TREEFILECTRL_FORWARD, OnUpdateForward)
	ON_WM_CONTEXTMENU()
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
	ON_WM_TIMER()
#endif
	ON_WM_INITMENUPOPUP()
	ON_WM_DESTROY()
	ON_WM_CONTEXTMENU()
	ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnDblclk)
	ON_NOTIFY_REFLECT_EX(TVN_ITEMEXPANDING, OnItemExpanding)
	ON_NOTIFY_REFLECT_EX(TVN_BEGINLABELEDIT, OnBeginLabelEdit)
	ON_NOTIFY_REFLECT_EX(TVN_ENDLABELEDIT, OnEndLabelEdit)
	ON_NOTIFY_REFLECT_EX(NM_RCLICK, OnRclick)
#ifndef FILETREECTRL_NO_RESOURCES	
	ON_NOTIFY_REFLECT_EX(TVN_BEGINDRAG, OnBeginDrag)
#endif	
  ON_NOTIFY_REFLECT_EX(NM_CUSTOMDRAW, OnCustomDraw)
 	ON_NOTIFY_REFLECT_EX(TVN_SELCHANGED, OnSelChanged)
	ON_NOTIFY_REFLECT_EX(TVN_DELETEITEM, OnDeleteItem)
  ON_REGISTERED_MESSAGE(WM_TREEUPDATE_CHANGE, OnChange)
END_MESSAGE_MAP()

CFileTreeCtrl::CFileTreeCtrl() : FILETREECTRL_BASE_CLASS(),
                                 m_dwFileHideFlags(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_TEMPORARY),
                                 m_dwFolderHideFlags(FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM | FILE_ATTRIBUTE_OFFLINE | FILE_ATTRIBUTE_TEMPORARY),
                                 m_bShowCompressedUsingDifferentColor(TRUE),
                                 m_rgbCompressed(RGB(0, 0, 255)),
                                 m_bShowEncryptedUsingDifferentColor(TRUE),
                                 m_rgbEncrypted(RGB(255, 0, 0)),
                                 m_dwDriveHideFlags(0),
                                 m_bShowFiles(TRUE),
                                 m_hNetworkRoot(NULL),
                                 m_hMyDocumentsRoot(NULL),
                                 m_hCurrentUserFolderRoot(NULL),
                                 m_hDesktop(NULL),
                                 m_hRoot(TVI_ROOT),
                                 m_TimerTicks(0),
                                 m_bAllowDragDrop(FALSE),
                                 m_bAllowRename(FALSE),
                                 m_bAllowOpen(TRUE),
                                 m_bAllowProperties(TRUE),
                                 m_bAllowDelete(FALSE),
                                 m_bAllowCreateFolder(FALSE),
                                 m_nMaxHistory(20),
                                 m_bUpdatingHistorySelection(FALSE),
                                 m_nAutoRefresh(FALSE),
                                 m_bShowSharedUsingDifferentIcon(TRUE),
                                 m_nTimerID(0),
                                 m_pMalloc(NULL),
                                 m_pShellFolder(NULL),
                                 m_bShowNetwork(TRUE),
                                 m_FileExtensions(UseTheShellSetting),
                                 m_dwNetworkItemTypes(RESOURCETYPE_ANY),
                                 m_hMyComputerRoot(NULL),
                                 m_bShowMyComputer(TRUE),
                                 m_bShowDesktop(TRUE),
                                 m_bShowMyDocuments(TRUE),
                                 m_bShowCurrentUserFolder(FALSE),
                                 m_bShowRootedFolder(FALSE),
                                 m_hRootedFolder(NULL),
                               #ifndef FILETREECTRL_NO_RESOURCES	  
                                 m_DropCopyCursor(NULL),
                                 m_NoDropCopyCursor(NULL),
                                 m_DropMoveCursor(NULL),
                                 m_NoDropMoveCursor(NULL),
                                 m_pilDrag(NULL),
                                 m_hItemDrag(NULL),
                                 m_hItemDrop(NULL),
                               #endif
                               #ifndef FILETREECTRL_NO_CACHE_COMPUTER_ICON  
                                 m_nComputerIconIndex(-1),
                                 m_nSelectedComputerIconIndex(-1),
                               #endif
                                 m_bShowDriveLabels(TRUE),
                                 m_pMonitoringThread(NULL)
{
  m_sFileNameMasks.Add(_T("*.*"));

  for (int i=0; i<26; i++)
    m_dwMediaID[i] = 0xFFFFFFFF;
    
  //As a performance optimization we cache the indexes for the computer and selected computer icons, so that we can use it 
  //when displaying machines on the network. This helps to speed up the performance of CFileTreeCtrl when browsing domains or 
  //workgroups with a large number of workstations and servers.
#ifndef FILETREECTRL_NO_CACHE_COMPUTER_ICON    
  TCHAR szComputerName[MAX_COMPUTERNAME_LENGTH + 1];
  DWORD dwCompuerNameSize = sizeof(szComputerName)/sizeof(TCHAR);
  if (GetComputerName(szComputerName, &dwCompuerNameSize))
  {
    CString sLocalComputerName;
    sLocalComputerName.Format(_T("\\\\%s"), szComputerName);
    m_nComputerIconIndex = GetIconIndex(sLocalComputerName);
    m_nSelectedComputerIconIndex = GetSelIconIndex(sLocalComputerName);
  }
#endif    

  //Determine if we are running on the Vista and if so, by default show the Current User folder instead of My Documents
  OSVERSIONINFO osvi;
  osvi.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
  BOOL bVistaOrLater = (GetVersionEx(&osvi) && osvi.dwMajorVersion >= 6);
  if (bVistaOrLater)
  {
		m_bShowMyDocuments = FALSE;
		m_bShowCurrentUserFolder = TRUE;
  }
}

int CALLBACK CFileTreeCtrl::CompareByFilenameNoCase(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
  CFileTreeCtrlItemInfo* pItem1 = reinterpret_cast<CFileTreeCtrlItemInfo*>(lParam1);
  CFileTreeCtrlItemInfo* pItem2 = reinterpret_cast<CFileTreeCtrlItemInfo*>(lParam2);
  
  //Handle the special ordering of some nodes
  if (pItem1->m_NodeType == CFileTreeCtrlItemInfo::CurrentUserFolderNode) //Current User node come before anything else
    return -1;
  if (pItem1->m_NodeType == CFileTreeCtrlItemInfo::MyDocumentsNode && pItem2->m_NodeType != CFileTreeCtrlItemInfo::CurrentUserFolderNode) //Then "My Documents"
    return -1;
  else if (pItem1->m_NodeType == CFileTreeCtrlItemInfo::MyComputerNode && (pItem2->m_NodeType != CFileTreeCtrlItemInfo::CurrentUserFolderNode) && (pItem2->m_NodeType != CFileTreeCtrlItemInfo::MyDocumentsNode)) //Then "My Computer"
    return -1;
  else if (pItem1->m_NodeType == CFileTreeCtrlItemInfo::NetworkNode && (pItem2->m_NodeType != CFileTreeCtrlItemInfo::CurrentUserFolderNode) && (pItem2->m_NodeType != CFileTreeCtrlItemInfo::MyDocumentsNode) && (pItem2->m_NodeType != CFileTreeCtrlItemInfo::MyComputerNode)) //Then "My Network Places" 
    return -1;
  
  //And then fall back to the standard sorting order
  if (pItem1->m_NodeType == CFileTreeCtrlItemInfo::FileNode)
  {
    if (pItem2->m_NodeType == CFileTreeCtrlItemInfo::FileNode)
      return pItem1->m_sRelativePath.CompareNoCase(pItem2->m_sRelativePath);
    else
      return 1;
  }
  else
  {
    if (pItem2->m_NodeType == CFileTreeCtrlItemInfo::FileNode)
      return -1;
    else
      return pItem1->m_sRelativePath.CompareNoCase(pItem2->m_sRelativePath);      
  }
}

#ifdef _DEBUG
void CFileTreeCtrl::AssertValid() const
{
	//Let the base class do its thing
	FILETREECTRL_BASE_CLASS::AssertValid();
}

void CFileTreeCtrl::Dump(CDumpContext& dc) const
{
	//Let the base class do its thing
	FILETREECTRL_BASE_CLASS::Dump(dc);
}
#endif //_DEBUG

void CFileTreeCtrl::SetShowFiles(BOOL bFiles) 
{ 
  //Since it can be touched by more than one thead
  InterlockedExchange(&m_bShowFiles, bFiles); 

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetRootFolder(const CString& sPath)
{
  m_sRootFolder = sPath;

  //Ensure it is not terminated with a "\"
  int nLength = m_sRootFolder.GetLength();
  if (nLength && m_sRootFolder.GetAt(nLength - 1) == _T('\\'))
    m_sRootFolder = m_sRootFolder.Left(nLength - 1);

  //Force a refresh
  Refresh();
}

int CFileTreeCtrl::GetIconIndex(HTREEITEM hItem)
{
  TVITEM tvi;
  tvi.mask = TVIF_HANDLE | TVIF_IMAGE;
  tvi.hItem = hItem;
  if (GetItem(&tvi))
    return tvi.iImage;
  else
    return -1;
}

int CFileTreeCtrl::GetIconIndex(const CString& sFilename)
{
  //Retreive the icon index for a specified file/folder
  SHFILEINFO sfi;
  memset(&sfi, 0, sizeof(SHFILEINFO));
  SHGetFileInfo(sFilename, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_SMALLICON);
  return sfi.iIcon; 
}

int CFileTreeCtrl::GetSelIconIndex(const CString& sFilename)
{
  //Retreive the icon index for a specified file/folder
  SHFILEINFO sfi;
  memset(&sfi, 0, sizeof(SHFILEINFO));
  SHGetFileInfo(sFilename, 0, &sfi, sizeof(SHFILEINFO), SHGFI_SYSICONINDEX | SHGFI_OPENICON | SHGFI_SMALLICON);
  return sfi.iIcon; 
}

int CFileTreeCtrl::GetIconIndex(LPITEMIDLIST lpPIDL)
{
  SHFILEINFO sfi;
  memset(&sfi, 0, sizeof(SHFILEINFO));
  SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpPIDL), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_LINKOVERLAY);
  return sfi.iIcon; 
}

int CFileTreeCtrl::GetSelIconIndex(LPITEMIDLIST lpPIDL)
{
  SHFILEINFO sfi;
  memset(&sfi, 0, sizeof(SHFILEINFO));
  SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpPIDL), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_SYSICONINDEX | SHGFI_SMALLICON | SHGFI_OPENICON);
  return sfi.iIcon; 
}

int CFileTreeCtrl::GetSelIconIndex(HTREEITEM hItem)
{
  TVITEM tvi;
  tvi.mask = TVIF_HANDLE | TVIF_SELECTEDIMAGE;
  tvi.hItem = hItem;
  if (GetItem(&tvi))
    return tvi.iSelectedImage;
  else
    return -1;
}

HTREEITEM CFileTreeCtrl::FindSibling(HTREEITEM hParent, const CString& sItem) const
{
  HTREEITEM hChild = GetChildItem(hParent);
  while (hChild)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hChild));
    AFXASSUME(pItem);
    if (pItem->m_sRelativePath.CompareNoCase(sItem) == 0)
      return hChild;
    hChild = GetNextItem(hChild, TVGN_NEXT);
  }
  return NULL;
}

CString CFileTreeCtrl::GetSelectedPath()
{
  HTREEITEM hItem = GetSelectedItem();
  if (hItem)
    return ItemToPath(hItem);
  else
    return CString();
}

HTREEITEM CFileTreeCtrl::FindServersNode(HTREEITEM hFindFrom) const
{
  if (m_bShowNetwork)
  {
    //Try to find some "servers" in the child items of hFindFrom
    HTREEITEM hChild = GetChildItem(hFindFrom);
    while (hChild)
    {
      CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hChild));
      AFXASSUME(pItem);
      if (pItem->m_pNetResource)
      {
        //Found a share 
        if (pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SERVER)
          return hFindFrom;
      }
 
      //Get the next sibling for the next loop around
      hChild = GetNextSiblingItem(hChild);
    }

    //Ok, since we got here, we did not find any servers in any of the child nodes of this 
    //item. In this case we need to call ourselves recursively to find one
    hChild = GetChildItem(hFindFrom);
    while (hChild)
    {
      HTREEITEM hFound = FindServersNode(hChild);
      if (hFound)
        return hFound;
       
      //Get the next sibling for the next loop around
      hChild = GetNextSiblingItem(hChild);
    }
  }

  //If we got as far as here then no servers were found.
  return NULL;
}

void CFileTreeCtrl::SetHasPlusButton(HTREEITEM hItem, BOOL bHavePlus)
{
  //Remove all the child items from the parent
  TVITEM tvItem;
  tvItem.hItem = hItem;
  tvItem.mask = TVIF_HANDLE | TVIF_CHILDREN;  
  tvItem.cChildren = bHavePlus;
  SetItem(&tvItem);
}

BOOL CFileTreeCtrl::HasPlusButton(HTREEITEM hItem)
{
  TVITEM tvItem;
  tvItem.hItem = hItem;
  tvItem.mask = TVIF_HANDLE | TVIF_CHILDREN;
  return GetItem(&tvItem) && (tvItem.cChildren != 0);
}

HTREEITEM CFileTreeCtrl::SetSelectedPath(const CString& sPath, BOOL bExpanded)
{
  CString sSearch(sPath);
  sSearch.MakeUpper();
  int nSearchLength = sSearch.GetLength();
  if (nSearchLength == 0)
  {
    TRACE(_T("CFileTreeCtrl::SetSelectedPath, Cannot select a empty path\n"));
    return NULL;
  }

  //Remove initial part of path if the root folder is setup
  CString sRootFolder(m_sRootFolder);
  sRootFolder.MakeUpper();
  int nRootLength = sRootFolder.GetLength();
  if (nRootLength)
  {
    if (sSearch.Find(sRootFolder) != 0)
    {
      TRACE(_T("CFileTreeCtrl::SetSelectedPath, Could not select the path %s as the root has been configued as %s\n"), sPath.operator LPCTSTR(), m_sRootFolder.operator LPCTSTR());
      return NULL;
    }
    sSearch = sSearch.Right(sSearch.GetLength() - nRootLength);
    
    //Remove leading "\" from the path
    nSearchLength = sSearch.GetLength();
    if (nSearchLength > 1 && sSearch.GetAt(0) == _T('\\'))
      sSearch = sSearch.Right(nSearchLength-1);
  }

  //Remove trailing "\" from the path
  nSearchLength = sSearch.GetLength();
  if (nSearchLength > 3 && sSearch.GetAt(nSearchLength - 1) == _T('\\'))
    sSearch = sSearch.Left(nSearchLength-1);

  if (sSearch.IsEmpty())
    return NULL;

  SetRedraw(FALSE);

  HTREEITEM hItemFound = m_hRoot;
  if (nRootLength && m_hRootedFolder)
    hItemFound = m_hRootedFolder;
  BOOL bDriveMatch = sRootFolder.IsEmpty();
  BOOL bNetworkMatch = m_bShowNetwork && ((sSearch.GetLength() > 2) && sSearch.Find(_T("\\\\")) == 0);
  if (bNetworkMatch)
  {
    bDriveMatch = FALSE;

    BOOL bHasPlus = HasPlusButton(m_hNetworkRoot);
    BOOL bHasChildren = (GetChildItem(m_hNetworkRoot) != NULL);

    if (bHasPlus && !bHasChildren)
      DoExpand(m_hNetworkRoot);
    else
      Expand(m_hNetworkRoot, TVE_EXPAND);
    hItemFound = FindServersNode(m_hNetworkRoot);
    sSearch = sSearch.Right(sSearch.GetLength() - 2);
  }
  if (bDriveMatch)
  {
    CString sMyDocumentsPath(m_sMyDocumentsPath);
    sMyDocumentsPath.MakeUpper();
    CString sMyDesktopPath(m_sDesktopPath);
    sMyDesktopPath.MakeUpper();
    CString sCurrentUserFolderPath(m_sCurrentUserFolderPath);
    sCurrentUserFolderPath.MakeUpper();

    if (m_hDesktop && sSearch.Find(sMyDesktopPath) == 0)
    {
      BOOL bHasPlus = HasPlusButton(m_hDesktop);
      BOOL bHasChildren = (GetChildItem(m_hDesktop) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(m_hDesktop);
      else
        Expand(m_hDesktop, TVE_EXPAND);
      hItemFound = m_hDesktop;

      //Skip over the part of the search path which contains the desktop path
      sSearch = sSearch.Right(sSearch.GetLength() - sMyDesktopPath.GetLength() - 1);
      
      //Because of the special way that the desktop item is shown, check to see if
      //we should collapse the desktop node
      if (sSearch.GetLength() == 0)
        Expand(m_hDesktop, TVE_COLLAPSE);
      
      bDriveMatch = FALSE;
    }
    else if (m_hCurrentUserFolderRoot && sSearch.Find(sCurrentUserFolderPath) == 0)
    {
      BOOL bHasPlus = HasPlusButton(m_hCurrentUserFolderRoot);
      BOOL bHasChildren = (GetChildItem(m_hCurrentUserFolderRoot) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(m_hCurrentUserFolderRoot);
      else
        Expand(m_hCurrentUserFolderRoot, TVE_EXPAND);
      hItemFound = m_hCurrentUserFolderRoot;

      //Skip over the part of the search path which contains the Current Users path
      sSearch = sSearch.Right(sSearch.GetLength() - sCurrentUserFolderPath.GetLength() - 1);
      bDriveMatch = FALSE;
    }
    else if (m_hMyDocumentsRoot && sSearch.Find(sMyDocumentsPath) == 0)
    {
      BOOL bHasPlus = HasPlusButton(m_hMyDocumentsRoot);
      BOOL bHasChildren = (GetChildItem(m_hMyDocumentsRoot) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(m_hMyDocumentsRoot);
      else
        Expand(m_hMyDocumentsRoot, TVE_EXPAND);
      hItemFound = m_hMyDocumentsRoot;

      //Skip over the part of the search path which contains the My documents path
      sSearch = sSearch.Right(sSearch.GetLength() - sMyDocumentsPath.GetLength() - 1);
      bDriveMatch = FALSE;
    }
    else if (m_hMyComputerRoot)
    {
      BOOL bHasPlus = HasPlusButton(m_hMyComputerRoot);
      BOOL bHasChildren = (GetChildItem(m_hMyComputerRoot) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(m_hMyComputerRoot);
      else
        Expand(m_hMyComputerRoot, TVE_EXPAND);
      hItemFound = m_hMyComputerRoot;
    }
  }

  int nFound = sSearch.Find(_T('\\'));
  while (nFound != -1)
  {
    CString sMatch;
    if (bDriveMatch)
    {
      sMatch = sSearch.Left(nFound + 1);
      bDriveMatch = FALSE;
    }
    else
      sMatch = sSearch.Left(nFound);
    
    hItemFound = FindSibling(hItemFound, sMatch);
    if (hItemFound == NULL)
      break;
    else if (!IsDrive(sPath))
    {
      SelectItem(hItemFound);

      BOOL bHasPlus = HasPlusButton(hItemFound);
      BOOL bHasChildren = (GetChildItem(hItemFound) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(hItemFound);
      else
        Expand(hItemFound, TVE_EXPAND);
    }

    sSearch = sSearch.Right(sSearch.GetLength() - nFound - 1);
    nFound = sSearch.Find(_T('\\'));
  };


  //The last item 
  if (hItemFound)
  {
    if (sSearch.GetLength())
    {
      HTREEITEM hTempItem = FindSibling(hItemFound, sSearch);
      if (hTempItem)
        hItemFound = hTempItem;
      else
      {
        //If we cannot find the final item, then deliberately do not
        //expand the parent folder
        bExpanded = FALSE;
      }
    }
    SelectItem(hItemFound);
    if (bExpanded)
    {
      BOOL bHasPlus = HasPlusButton(hItemFound);
      BOOL bHasChildren = (GetChildItem(hItemFound) != NULL);

      if (bHasPlus && !bHasChildren)
        DoExpand(hItemFound);
      Expand(hItemFound, TVE_EXPAND);
    }
  }

  //Turn back on the redraw flag
  SetRedraw(TRUE);

  return hItemFound;
}

BOOL CFileTreeCtrl::Rename(HTREEITEM hItem)
{
  if (hItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
    AFXASSUME(pItem);
	  if (m_bAllowRename && ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode) || (pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode)))
      return (EditLabel(hItem) != NULL);
    else
      return FALSE;
  }
  else
    return FALSE;
}

BOOL CFileTreeCtrl::ShowProperties(HTREEITEM hItem)
{
  BOOL bSuccess = FALSE;
  if (m_bAllowProperties && hItem)
  {
    //Show the "properties" for the selected file
    CString sFile = ItemToPath(hItem);
    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    sei.nShow = SW_SHOW;
    sei.lpFile = sFile.GetBuffer(sFile.GetLength());
    sei.lpVerb = _T("properties");
    sei.fMask  = SEE_MASK_INVOKEIDLIST;
    bSuccess = ShellExecuteEx(&sei);
    sFile.ReleaseBuffer();
  }
  return bSuccess;
}

CString CFileTreeCtrl::CreateNewChildFolderName(const CString& sParentFolder)
{
  //What will be the return value from this function
  CString sNewFolder;

  //Remove trailing "\" from the parent folder path
  CString sLocalParentFolder(sParentFolder);
  int nFind = sLocalParentFolder.GetLength();
  if ((nFind != -1) && sLocalParentFolder.GetAt(nFind - 1) == _T('\\'))
    sLocalParentFolder = sLocalParentFolder.Left(nFind - 1);

  BOOL bContinue = TRUE;
  int nAttempt = 1;
  while (bContinue)
  {
    //Create the new potential folder name
    if (nAttempt == 1)
      sNewFolder.Format(_T("%s\\New Folder"), sLocalParentFolder.operator LPCTSTR());
    else
      sNewFolder.Format(_T("%s\\New Folder (%d)"), sLocalParentFolder.operator LPCTSTR(), nAttempt);

    //Check to see if it already exists
    DWORD dwAttributes = GetFileAttributes(sNewFolder);
    if (dwAttributes == INVALID_FILE_ATTRIBUTES)
      bContinue = FALSE;
    else
      ++nAttempt;
  }

  return sNewFolder;
}

HTREEITEM CFileTreeCtrl::CreateFolder(HTREEITEM hItem, const CString& sNewFolderName)
{
  //What will be the return value from this function assume the worst
  HTREEITEM hNewFolder = NULL;

  if (hItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
    AFXASSUME(pItem);
    if (m_bAllowCreateFolder && (pItem->m_NodeType != CFileTreeCtrlItemInfo::NetworkNode))
    {
      //Get the path of the parent folder
      CString sParentFolder(ItemToPath(hItem));

      //Form the name of the new folder
      CString sNewFolder = sNewFolderName.IsEmpty() ? CreateNewChildFolderName(sParentFolder) : sNewFolderName;

      //Kill the monitoring thread if necessary
      BOOL bMonitoringThread = (m_pMonitoringThread != NULL);
      CString sMonitoringPath(m_sMonitoringPath);
      if (bMonitoringThread)
        KillMonitoringThread();

      //Try to create the new folder
      if (CreateDirectory(sNewFolder, NULL))
      {
        //Notify the shell that a new folder has been created
        SHChangeNotify(SHCNE_MKDIR, SHCNF_PATH, sNewFolder, NULL);
      
        //Expand the parent folder
        Expand(hItem, TVE_EXPAND);

        //Break apart the new folder name ready for setting up the CFileTreeCtrlItemInfo* item
        TCHAR szPath[_MAX_PATH];
        TCHAR szFname[_MAX_FNAME];
        TCHAR szExt[_MAX_EXT];
        _tsplitpath_s(sNewFolder, NULL, 0, NULL, 0, szFname, sizeof(szFname)/sizeof(TCHAR), szExt, sizeof(szExt)/sizeof(TCHAR));
        _tmakepath_s(szPath, sizeof(szPath)/sizeof(TCHAR), NULL, NULL, szFname, szExt);
      
        //Determine if the parent item already has child items before we add the new item
        BOOL bHasChildren = (GetChildItem(hItem) != NULL);

        //Insert the new item
        CFileTreeCtrlItemInfo* pNewItem = new CFileTreeCtrlItemInfo;
        pNewItem->m_sFQPath = sNewFolder;
        pNewItem->m_sRelativePath = szPath;
        pNewItem->m_NodeType = CFileTreeCtrlItemInfo::FolderNode;
        hNewFolder = InsertFileItem(hItem, pNewItem, FALSE, GetIconIndex(sNewFolder), GetSelIconIndex(sNewFolder), TRUE);
        if (hNewFolder)
        {
          //Update the plus button for the parent if necessary
          if (!bHasChildren)
            SetHasPlusButton(hItem, TRUE);        
        
          //Select the new folder
          SelectItem(hNewFolder);
                
          //Attempt in place editing of the new folder
          Rename(hNewFolder);
        }
      }
      
		  //Recreate the monitoring thread if necessary
		  if (bMonitoringThread)
		    CreateMonitoringThread(m_sMonitoringPath);
    }     
  }

  return hNewFolder;
}

BOOL CFileTreeCtrl::Delete(HTREEITEM hItem)
{
  BOOL bSuccess = FALSE;

  if (hItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
    AFXASSUME(pItem);
    if (m_bAllowDelete && ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode) || ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode))))
    {
      //Create a Multi SZ string with the filename to delete
      CString sFileToDelete = ItemToPath(hItem);
      int nChars = sFileToDelete.GetLength() + 1;
      nChars++;
      SHFILEOPSTRUCT shfo;
      memset(&shfo, 0, sizeof(SHFILEOPSTRUCT));
      shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
      shfo.wFunc = FO_DELETE;

      //Undo is not allowed if the SHIFT key is held down
      if (!(GetKeyState(VK_SHIFT) & 0x8000))
        shfo.fFlags = FOF_ALLOWUNDO;

      TCHAR* pszFrom = new TCHAR[nChars];
      TCHAR* pszCur = pszFrom;
      _tcscpy_s(pszCur, nChars, sFileToDelete);
      pszCur[nChars-1] = _T('\0');
      shfo.pFrom = pszFrom;

      //Kill the monitoring thread if necessary
      BOOL bMonitoringThread = (m_pMonitoringThread != NULL);
      CString sMonitoringPath(m_sMonitoringPath);
      if (bMonitoringThread)
        KillMonitoringThread();

      //Let the shell perform the actual deletion
      if (SHFileOperation(&shfo) == 0 && shfo.fAnyOperationsAborted == FALSE)
      {
        //Gather the parent, next sibling and previous sibling before we do the delete
        HTREEITEM hParent = GetParentItem(hItem);
        HTREEITEM hPrevious = GetNextItem(hItem, TVGN_PREVIOUS);
        HTREEITEM hNext = GetNextItem(hItem, TVGN_NEXT);
        
        //Work out which item we should reselect after the delete
        if (hPrevious == NULL)
          hPrevious = hNext;
        if (hPrevious == NULL)
          hPrevious = hParent;
      
        //Delete the item from the view
        bSuccess = DeleteItem(hItem);
        
        //Update the selected item and plus button state of the parent if required
        if (bSuccess && hPrevious)
        {
          Select(hPrevious, TVGN_CARET);
          if (hPrevious == hParent)
            SetHasPlusButton(hParent, FALSE);
        }
      }

		  //Recreate the monitoring thread if necessary
		  if (bMonitoringThread)
		    CreateMonitoringThread(m_sMonitoringPath);

      //Free up the memory we had allocated
      delete [] pszFrom;
    }
  }
  return bSuccess;
}

BOOL CFileTreeCtrl::Open(HTREEITEM hItem)
{
  BOOL bSuccess = FALSE;
  if (m_bAllowOpen && hItem)
  {
    //Execute the default command for the selected item
    CString sFile = ItemToPath(hItem);
    SHELLEXECUTEINFO sei;
    memset(&sei, 0, sizeof(sei));
    sei.cbSize = sizeof(sei);
    sei.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    sei.nShow = SW_SHOW;
    sei.lpFile = sFile.GetBuffer(sFile.GetLength());
    sei.fMask  = SEE_MASK_INVOKEIDLIST;
    bSuccess = ShellExecuteEx(&sei);
    sFile.ReleaseBuffer();
  }
  return bSuccess;
}

void CFileTreeCtrl::SetDriveHideFlags(DWORD dwDriveHideFlags)
{
  m_dwDriveHideFlags = dwDriveHideFlags;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetFileHideFlags(DWORD dwFileHideFlags)
{
  m_dwFileHideFlags = dwFileHideFlags;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetFolderHideFlags(DWORD dwFolderHideFlags)
{
  m_dwFolderHideFlags = dwFolderHideFlags;

  //Force a refresh
  Refresh();
}

INT_PTR CFileTreeCtrl::AddMultipleFileNameMasks(const CString& sFileNameMasks)
{
  //Empty out the array
  m_sFileNameMasks.SetSize(0);
	
	//Loop through the whole string, adding masks as they are encountered
	int length = sFileNameMasks.GetLength();
	TCHAR* buf = new TCHAR[length + 1];	// Allocate a work area (don't touch parameter itself)
  _tcscpy_s(buf, length+1, sFileNameMasks);

	for (int pos=0, start=0; pos<=length; pos++)
	{
		//Valid separators between addresses are ',' or ';'
		if ((buf[pos] == _T(',')) || (buf[pos] == _T(';')) || (buf[pos] == 0))
		{
			buf[pos] = 0;	//Redundant when at the end of string, but who cares.
      CString sTemp(&buf[start]);
      sTemp.Trim();

			if (sTemp.GetLength())
				m_sFileNameMasks.Add(sTemp);

      //Move on to the next position
			start = pos + 1;
		}
	}
    
  //Tidy up the heap memory we have used
	delete [] buf;

  //Force a refresh
  Refresh();

  //Return the number of recipients parsed
	return m_sFileNameMasks.GetSize();
}

CString CFileTreeCtrl::GetFileNameMasks() const
{
  //What will be the return value from this function
  CString sMasks;

  INT_PTR nSize = m_sFileNameMasks.GetSize();  
  for (INT_PTR i=0; i<nSize; i++)
  {
    sMasks += m_sFileNameMasks.GetAt(i);
    if (i < (nSize-1))
      sMasks += _T(",");
  }

  return sMasks;
}

void CFileTreeCtrl::SetFileNameMask(INT_PTR nIndex, const CString& sFileNameMask)
{
  m_sFileNameMasks.SetAt(nIndex, sFileNameMask);

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::AddFileNameMask(const CString& sFileNameMask)
{
  m_sFileNameMasks.Add(sFileNameMask);

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetCompressedColor(COLORREF rgbCompressed)
{
  m_rgbCompressed = rgbCompressed;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowNetwork(BOOL bShowNetwork)
{
  m_bShowNetwork = bShowNetwork;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetUsingDifferentColorForCompressed(BOOL bShowCompressedUsingDifferentColor)
{
  m_bShowCompressedUsingDifferentColor = bShowCompressedUsingDifferentColor;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetUsingDifferentIconForSharedFolders(BOOL bShowSharedUsingDifferentIcon)
{
  m_bShowSharedUsingDifferentIcon = bShowSharedUsingDifferentIcon;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetUsingDifferentColorForEncrypted(BOOL bShowEncryptedUsingDifferentColor)
{
  m_bShowEncryptedUsingDifferentColor = bShowEncryptedUsingDifferentColor;

  //Force a refresh
  Refresh();
};

void CFileTreeCtrl::SetShowFileExtensions(HideFileExtension FileExtensions)
{
  m_FileExtensions = FileExtensions;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetNetworkItemTypes(DWORD dwTypes)
{
  m_dwNetworkItemTypes = dwTypes;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowDriveLabels(BOOL bShowDriveLabels)
{
  m_bShowDriveLabels = bShowDriveLabels;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowMyComputer(BOOL bShowMyComputer)
{
  m_bShowMyComputer = bShowMyComputer;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowDesktop(BOOL bShowDesktop)
{
  m_bShowDesktop = bShowDesktop;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowMyDocuments(BOOL bShowMyDocuments)
{
  m_bShowMyDocuments = bShowMyDocuments;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowCurrentUserFolder(BOOL bShowCurrentUserFolder)
{
  m_bShowCurrentUserFolder = bShowCurrentUserFolder;

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::SetShowRootedFolder(BOOL bShowRootedFolder)
{
  m_bShowRootedFolder = bShowRootedFolder;

  //Force a refresh
  Refresh();
}

BOOL CFileTreeCtrl::CanDisplayFile(const CFileFind& find)
{
  //Derived classes can decide dynamically whether or not a 
  //certain file is to be displayed. CFileTreeCtrl by default
  //displays all files which do not have attributes as set in m_dwFileHideFlags

  return (m_bShowFiles && !find.IsDirectory() && !find.MatchesMask(m_dwFileHideFlags));
}

BOOL CFileTreeCtrl::CanDisplayFolder(const CFileFind& find)
{
  //Derived classes can decide dynamically whether or not a 
  //certain folder is to be displayed. CFileTreeCtrl by default
  //displays all folders excluding the ".." and "." entries and which do
  //not have attributes as set in m_dwFolderHideFlags

  return (find.IsDirectory() && !find.IsDots() && !find.MatchesMask(m_dwFolderHideFlags));
}

BOOL CFileTreeCtrl::CanDisplayNetworkItem(CFileTreeCtrlItemInfo* /*pItem*/)
{
  //Derived classes can decide dynamically whether or not a 
  //certain network item is to be displayed. CFileTreeCtrl by default
  //displays all items

  return TRUE;
}

BOOL CFileTreeCtrl::CanHandleChangeNotifications(const CString& sPath)
{
  //If no path was given, then fail this function
  if (sPath.IsEmpty())
    return FALSE;
  
  //We are not interested in files
  if (IsFile(sPath))
    return FALSE;
    
  //The path must be a folder of some kind  
  if (!IsFolder(sPath))
    return FALSE;
  
  //check if this drive is one of the types which can issue notification changes
  CString sDrive(sPath);
  if (!IsDrive(sDrive))
    sDrive = sPath.Left(3);

  UINT nDrive = GetDriveType(sDrive);
  return ((nDrive != DRIVE_REMOVABLE) && nDrive != DRIVE_CDROM);
}

BOOL CFileTreeCtrl::CanDisplayDrive(const CString& sDrive)
{
  //Derived classes can decide dynamically whether or not a 
  //certain drive is to be displayed. CFileTreeCtrl by default
  //displays all drives which do not have attributes as set in
  //m_dwDriveHideFlags

  //check if this drive is one of the types to hide
  BOOL bDisplay = TRUE;
  UINT nDrive = GetDriveType(sDrive);
  switch (nDrive)
  {
    case DRIVE_REMOVABLE:
    {
      if (m_dwDriveHideFlags & DRIVE_ATTRIBUTE_REMOVABLE)
        bDisplay = FALSE;
      break;
    }
    case DRIVE_FIXED:
    {
      if (m_dwDriveHideFlags & DRIVE_ATTRIBUTE_FIXED)
        bDisplay = FALSE;
      break;
    }
    case DRIVE_REMOTE:
    {
      if (m_dwDriveHideFlags & DRIVE_ATTRIBUTE_REMOTE)
        bDisplay = FALSE;
      break;
    }
    case DRIVE_CDROM:
    {
      if (m_dwDriveHideFlags & DRIVE_ATTRIBUTE_CDROM)
        bDisplay = FALSE;
      break;
    }
    case DRIVE_RAMDISK:
    {
      if (m_dwDriveHideFlags & DRIVE_ATTRIBUTE_RAMDISK)
        bDisplay = FALSE;
      break;
    }
    default:
    {
      break;
    }
  }

  return bDisplay;
}

void CFileTreeCtrl::OnRename() 
{
  Rename(GetSelectedItem());
}

void CFileTreeCtrl::OnUpdateRename(CCmdUI* pCmdUI) 
{
  HTREEITEM hSelItem = GetSelectedItem();
  if (hSelItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hSelItem));
    AFXASSUME(pItem);
	  pCmdUI->Enable(m_bAllowRename && ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode) || (pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode)));
  }
  else
    pCmdUI->Enable(FALSE);
}

void CFileTreeCtrl::OnProperties() 
{
  ShowProperties(GetSelectedItem());
}

void CFileTreeCtrl::OnUpdateProperties(CCmdUI* pCmdUI) 
{
  HTREEITEM hSelItem = GetSelectedItem();
  if (hSelItem)
  {
    if (m_bAllowProperties)
    {
      CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hSelItem));
      AFXASSUME(pItem);
      if (pItem->m_NodeType == CFileTreeCtrlItemInfo::NetworkNode)
      { 
        if (pItem->m_pNetResource)
          pCmdUI->Enable(pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SERVER ||
                         pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SHARE);
        else
          pCmdUI->Enable(FALSE);
      }
      else
        pCmdUI->Enable(TRUE);
    }
    else
      pCmdUI->Enable(FALSE);
  }
  else
    pCmdUI->Enable(FALSE);
}

void CFileTreeCtrl::OnOpen() 
{
  Open(GetSelectedItem());
}

void CFileTreeCtrl::OnUpdateOpen(CCmdUI* pCmdUI) 
{
  HTREEITEM hSelItem = GetSelectedItem();
  if (hSelItem)
  {
    if (m_bAllowOpen)
    {
      CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hSelItem));
      AFXASSUME(pItem);
      if (pItem->m_NodeType == CFileTreeCtrlItemInfo::NetworkNode)
      { 
        if (pItem->m_pNetResource)
          pCmdUI->Enable(pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SERVER ||
                         pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SHARE);
        else
          pCmdUI->Enable(FALSE);
      }
      else
        pCmdUI->Enable(TRUE);
    }
    else
      pCmdUI->Enable(FALSE);
  }
  else
    pCmdUI->Enable(FALSE);
}

void CFileTreeCtrl::OnDelete() 
{
  Delete(GetSelectedItem());
}

void CFileTreeCtrl::OnUpdateDelete(CCmdUI* pCmdUI) 
{
  HTREEITEM hSelItem = GetSelectedItem();
  if (hSelItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hSelItem));
    AFXASSUME(pItem);
	  pCmdUI->Enable(m_bAllowDelete && ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode) || ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode))));
  }
  else
    pCmdUI->Enable(FALSE);
}


void CFileTreeCtrl::OnCreateFolder() 
{
  CreateFolder(GetSelectedItem());
}

void CFileTreeCtrl::OnUpdateCreateFolder(CCmdUI* pCmdUI) 
{
  HTREEITEM hSelItem = GetSelectedItem();
  if (hSelItem && m_bAllowCreateFolder)
  {
    //Allow creation of a folder if the selected item is itself a directory/folder
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hSelItem));
    ASSERT(pItem);
    DWORD dwAttributes = GetFileAttributes(pItem->m_sFQPath);
    if (dwAttributes != INVALID_FILE_ATTRIBUTES)
	    pCmdUI->Enable((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) != 0);
    else
      pCmdUI->Enable(FALSE);
  }
  else
    pCmdUI->Enable(FALSE);
}


#ifndef FILETREECTRL_NO_RESOURCES
void CFileTreeCtrl::OnContextMenu(CWnd*, CPoint point)
{
	CMenu menu;
  VERIFY(menu.LoadMenu(IDR_TREEFILECTRL_POPUP));
	CMenu* pPopup = menu.GetSubMenu(0);
	AFXASSUME(pPopup);
	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y,	this);
}
#endif

void CFileTreeCtrl::UpOneLevel()
{
  OnUpOneLevel();
}

void CFileTreeCtrl::OnUpdateUpOneLevel(CCmdUI* pCmdUI)
{
  HTREEITEM hItem = GetSelectedItem();
  if (hItem)
    pCmdUI->Enable(GetParentItem(hItem) != NULL);
  else
    pCmdUI->Enable(FALSE);
}

BOOL CFileTreeCtrl::OnDblclk(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  HTREEITEM hItem = GetSelectedItem();
  CPoint pt(GetCurrentMessage()->pt);
  ScreenToClient(&pt);

	if (hItem && (hItem == HitTest(pt)))
	{
		if (!HasPlusButton(hItem))
			OnOpen();
	}
	
	*pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

BOOL CFileTreeCtrl::GoBack()
{
  BOOL bSuccess = FALSE;
  if (m_PrevItems.GetSize())
  {
    OnBack();
    bSuccess = TRUE;
  }
  return bSuccess;
}

BOOL CFileTreeCtrl::GoForward()
{
  BOOL bSuccess = FALSE;
  if (m_NextItems.GetSize())
  {
    OnForward();
    bSuccess = TRUE;
  }
  return bSuccess;
}

void CFileTreeCtrl::Refresh()
{
  if (GetSafeHwnd())
    OnRefresh();
}

void CFileTreeCtrl::OnRefresh() 
{
  //Just in case this will take some time
  CWaitCursor wait;

  SetRedraw(FALSE);

  //Get the item which is currently selected
  HTREEITEM hSelItem = GetSelectedItem();
  CString sItem;
  BOOL bExpanded = FALSE;
  if (hSelItem)
  {
    sItem = ItemToPath(hSelItem);
    bExpanded = IsExpanded(hSelItem); 
  }

  theSharedEnumerator.Refresh();

  KillMonitoringThread();

  //Remove all nodes that currently exist
  Clear();

  //Display the folder items in the tree
  if (m_sRootFolder.IsEmpty())
  {
	  //Should we insert a desktop node
    TCHAR szDesktopPath[_MAX_PATH];
    szDesktopPath[0] = _T('\0');
    if (m_bShowDesktop)
    {
		  CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
		  pItem->m_NodeType = CFileTreeCtrlItemInfo::DesktopNode;
		  int nIcon = 0;
		  int nSelIcon = 0;

		  //Get the localized name, path and correct icons for the Desktop
		  LPITEMIDLIST lpDPidl;
		  if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &lpDPidl)))
		  {
		    SHFILEINFO sfi;
		    if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpDPidl), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
			    pItem->m_sRelativePath = sfi.szDisplayName;

        //While we are at it, also get the real path of the Desktop
        if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_DESKTOP, NULL, SHGFP_TYPE_CURRENT, szDesktopPath)))
        {
          pItem->m_sFQPath = szDesktopPath;
          m_sDesktopPath = pItem->m_sFQPath; //Also maintain a copy ourselves, We use this when we are trying to select items in the control
        }
			    
		    nIcon = GetIconIndex(lpDPidl);
		    nSelIcon = GetSelIconIndex(lpDPidl);

		    //Free up the pidl now that we are finished with it
		    ASSERT(m_pMalloc);
		    m_pMalloc->Free(lpDPidl);
		    m_pMalloc->Release();
		  }

		  //Add it to the tree control
		  m_hRoot = InsertFileItem(TVI_ROOT, pItem, FALSE, nIcon, nSelIcon, FALSE);
		  m_hDesktop = m_hRoot;
    }
    else
      m_hRoot = TVI_ROOT;

    DisplayRootItems();
    
	  //Also show all items underneath the desktop
    if (_tcslen(szDesktopPath))
		  DisplayPath(szDesktopPath, m_hRoot, FALSE, FALSE);
  }
  else
    DisplayPath(m_sRootFolder, m_hRoot, FALSE);
  
  //Reselect the initially selected item
  if (hSelItem)
    SetSelectedPath(sItem, bExpanded);

  //Turn back on the redraw flag
  SetRedraw(TRUE);
}

HTREEITEM CFileTreeCtrl::InsertFileItem(HTREEITEM hParent, CFileTreeCtrlItemInfo* pItem, BOOL bShared, int nIcon, int nSelIcon, BOOL bCheckForChildren)
{
  //Validate our parameters
  ASSERT(pItem->m_NodeType != CFileTreeCtrlItemInfo::UnknownNode);

  //Correct the label if need be
  CString sLabel;
  if (IsDrive(pItem->m_sFQPath) && m_bShowDriveLabels)
    sLabel = GetDriveLabel(pItem->m_sFQPath);
  else
    sLabel = GetCorrectedLabel(pItem);

  //Add the actual item
	TVINSERTSTRUCT tvis;
  memset(&tvis, 0, sizeof(TVINSERTSTRUCT));
	tvis.hParent = hParent;
	tvis.hInsertAfter = TVI_LAST;
	tvis.item.mask = TVIF_CHILDREN | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT | TVIF_PARAM;
  tvis.item.iImage = nIcon;
  tvis.item.iSelectedImage = nSelIcon;

  tvis.item.lParam = reinterpret_cast<LPARAM>(pItem);
  tvis.item.pszText = sLabel.GetBuffer(sLabel.GetLength());
  if (bCheckForChildren)
	  tvis.item.cChildren = HasGotSubEntries(pItem->m_sFQPath);
  else
    tvis.item.cChildren = TRUE;
  if (bShared)
  {
    tvis.item.mask |= TVIF_STATE;
    tvis.item.stateMask |= TVIS_OVERLAYMASK;
    tvis.item.state |= INDEXTOOVERLAYMASK(1); //1 is the index for the shared overlay image
  }

  HTREEITEM hItem = InsertItem(&tvis);
  sLabel.ReleaseBuffer();
  return hItem;
}

BOOL CFileTreeCtrl::IsExpanded(HTREEITEM hItem)
{
  TVITEM tvItem;
  tvItem.hItem = hItem;
  tvItem.mask = TVIF_HANDLE | TVIF_STATE;
  return GetItem(&tvItem) && (tvItem.state & TVIS_EXPANDED);
}

CString CFileTreeCtrl::GetCorrectedLabel(CFileTreeCtrlItemInfo* pItem)
{
  CString sLabel(pItem->m_sRelativePath);

  switch (m_FileExtensions)
  {
    case UseTheShellSetting:
    {
      TCHAR pszLabel[_MAX_PATH];
      if (IsFile(pItem->m_sFQPath) && GetFileTitle(pItem->m_sRelativePath, pszLabel, _MAX_PATH) == 0)
      {
        pItem->m_bExtensionHidden = (sLabel.CompareNoCase(pszLabel) != 0);
        sLabel = pszLabel;
      }
      break;
    }
    case HideExtension:
    {
      //Remove the extension if the item is a file
      if (IsFile(pItem->m_sFQPath))
      {
        TCHAR szPath[_MAX_PATH];
        TCHAR szDrive[_MAX_DRIVE];
        TCHAR szDir[_MAX_DIR];
        TCHAR szFname[_MAX_FNAME];
        _tsplitpath_s(pItem->m_sRelativePath, szDrive, sizeof(szDrive)/sizeof(TCHAR), szDir, sizeof(szDir)/sizeof(TCHAR), szFname, sizeof(szFname)/sizeof(TCHAR), NULL, 0);
        _tmakepath_s(szPath, sizeof(szPath)/sizeof(TCHAR), szDrive, szDir, szFname, NULL);
        sLabel = szPath;
        pItem->m_bExtensionHidden = true;
      }
      break;
    }
    default:
    {
      pItem->m_bExtensionHidden = false;
      break;
    }
  }

  return sLabel;
}


void CFileTreeCtrl::DisplayRootItems()
{
  //add Current User Folder if requested to do so
  if (m_bShowCurrentUserFolder)
  {
    CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
    pItem->m_NodeType = CFileTreeCtrlItemInfo::CurrentUserFolderNode;
    int nIcon = 0;
    int nSelIcon = 0;

    //Get the localized name and correct icons for the current users folder
    LPITEMIDLIST lpDocsPidl;
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_PROFILE, &lpDocsPidl)))
    {
			//Get the display name of the folder
      SHFILEINFO sfi;
      if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpDocsPidl), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
        pItem->m_sRelativePath = sfi.szDisplayName;

      //While we are at it, also get the real path of the current users folder
      TCHAR szFolderPath[_MAX_PATH];
      if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PROFILE, NULL, SHGFP_TYPE_CURRENT, szFolderPath)))
      {
        pItem->m_sFQPath = szFolderPath;
        m_sCurrentUserFolderPath = pItem->m_sFQPath; //Also maintain a copy ourselves, We use this when we are trying to select items in the control
      }

			//Get the icons for the folder
      nIcon = GetIconIndex(lpDocsPidl);
      nSelIcon = GetSelIconIndex(lpDocsPidl);

      //Free up the pidl now that we are finished with it
      ASSERT(m_pMalloc);
      m_pMalloc->Free(lpDocsPidl);
      m_pMalloc->Release();
    }

    //Add it to the tree control
    m_hCurrentUserFolderRoot = InsertFileItem(m_hRoot, pItem, FALSE, nIcon, nSelIcon, FALSE);
  }

  //add My Documents if requested to do so
  if (m_bShowMyDocuments)
  {
    CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
    pItem->m_NodeType = CFileTreeCtrlItemInfo::MyDocumentsNode;
    int nIcon = 0;
    int nSelIcon = 0;

    //Get the localized name and correct icons for "My Documents"
    LPITEMIDLIST lpDocsPidl;
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_PERSONAL, &lpDocsPidl)))
    {
			//Get the display name of the folder
      SHFILEINFO sfi;
      if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpDocsPidl), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
        pItem->m_sRelativePath = sfi.szDisplayName;

      //While we are at it, also get the real path of "My Documents"
      TCHAR szFolderPath[_MAX_PATH];
      if (SUCCEEDED(SHGetFolderPath(NULL, CSIDL_PERSONAL, NULL, SHGFP_TYPE_CURRENT, szFolderPath)))
      {
        pItem->m_sFQPath = szFolderPath;
        m_sMyDocumentsPath = pItem->m_sFQPath; //Also maintain a copy ourselves, We use this when we are trying to select items in the control
      }

			//Get the icons for the folder
      nIcon = GetIconIndex(lpDocsPidl);
      nSelIcon = GetSelIconIndex(lpDocsPidl);

      //Free up the pidl now that we are finished with it
      ASSERT(m_pMalloc);
      m_pMalloc->Free(lpDocsPidl);
      m_pMalloc->Release();
    }

    //Add it to the tree control
    m_hMyDocumentsRoot = InsertFileItem(m_hRoot, pItem, FALSE, nIcon, nSelIcon, FALSE);
  }

  //Should we insert a "My Computer" node
  if (m_bShowMyComputer)
  {
    CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
    pItem->m_NodeType = CFileTreeCtrlItemInfo::MyComputerNode;
    int nIcon = 0;
    int nSelIcon = 0;

    //Get the localized name and correct icons for "My Computer"
    LPITEMIDLIST lpMCPidl;
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_DRIVES, &lpMCPidl)))
    {
			//Get the display name for the folder
      SHFILEINFO sfi;
      if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpMCPidl), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
        pItem->m_sRelativePath = sfi.szDisplayName;
        
      //Get the icons for the folder  
      nIcon = GetIconIndex(lpMCPidl);
      nSelIcon = GetSelIconIndex(lpMCPidl);

      //Free up the pidl now that we are finished with it
      ASSERT(m_pMalloc);
      m_pMalloc->Free(lpMCPidl);
      m_pMalloc->Release();
    }

    //Add it to the tree control
    m_hMyComputerRoot = InsertFileItem(m_hRoot, pItem, FALSE, nIcon, nSelIcon, FALSE);
  }

  //Display all the drives (if we are not using My Computers")
  if (!m_bShowMyComputer)
    DisplayDrives(m_hRoot, FALSE);

  //Also add network neighborhood if requested to do so
  if (m_bShowNetwork)
  {
    CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
    pItem->m_NodeType = CFileTreeCtrlItemInfo::NetworkNode;
    int nIcon = 0;
    int nSelIcon = 0;

    //Get the localized name and correct icons for "Network Neighborhood"
    LPITEMIDLIST lpNNPidl;
    if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_NETWORK, &lpNNPidl)))
    {
			//Get the display name for the folder
      SHFILEINFO sfi;
      if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpNNPidl), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
        pItem->m_sRelativePath = sfi.szDisplayName;
        
      //Get the icons for the folder  
      nIcon = GetIconIndex(lpNNPidl);
      nSelIcon = GetSelIconIndex(lpNNPidl);

      //Free up the pidl now that we are finished with it
      ASSERT(m_pMalloc);
      m_pMalloc->Free(lpNNPidl);
      m_pMalloc->Release();
    }

    //Add it to the tree control
    m_hNetworkRoot = InsertFileItem(m_hRoot, pItem, FALSE, nIcon, nSelIcon, FALSE);
  }
}

BOOL CFileTreeCtrl::OnBeginLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);

  if (pDispInfo->item.hItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(pDispInfo->item.hItem));
    AFXASSUME(pItem);
	  if (m_bAllowRename && ((pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode) || (pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode)))
      *pResult = FALSE;
    else
    	*pResult = TRUE;
  }
  else
    *pResult = TRUE;

  return TRUE; //Allow the message to be reflected again
}

BOOL CFileTreeCtrl::OnCustomDraw(NMHDR* pNMHDR, LRESULT* pResult) 
{
  NMTVCUSTOMDRAW* pCustomDraw = reinterpret_cast<NMTVCUSTOMDRAW*>(pNMHDR);
  switch (pCustomDraw->nmcd.dwDrawStage) 
  {
    case CDDS_PREPAINT:
    {
      *pResult = CDRF_NOTIFYITEMDRAW; //Tell the control that we are interested in item notifications
      break;
		}	
    case CDDS_ITEMPREPAINT:
    {
      //Let it know that we are finished doing custom draw
      *pResult = CDRF_DODEFAULT;

      //To work around what looks like a bug in Commctrl v6 we do not do custom drawing until the window is visible
      if (!IsWindowVisible())
        return TRUE;

      //Check to see if this item is compressed and if it it is, change its
      //color just like explorer does
      if (m_bShowCompressedUsingDifferentColor && ((pCustomDraw->nmcd.uItemState & CDIS_SELECTED) == 0) && 
          IsCompressed((HTREEITEM) pCustomDraw->nmcd.dwItemSpec))
      {
        pCustomDraw->clrText = m_rgbCompressed;
      }
      //also check for encrypted files
      else if (m_bShowEncryptedUsingDifferentColor && ((pCustomDraw->nmcd.uItemState & CDIS_SELECTED) == 0) && 
               IsEncrypted((HTREEITEM) pCustomDraw->nmcd.dwItemSpec))
      {
        pCustomDraw->clrText = m_rgbEncrypted;
      }

      break;
    }
    default:
    {
      break;
    }
  }
  
  return TRUE; //Allow the message to be reflected again
}

BOOL CFileTreeCtrl::OnSelChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pNMHDR);
	AFXASSUME(pNMTreeView);

  //Nothing selected
  if (pNMTreeView->itemNew.hItem == NULL)
    return FALSE;

  //Check to see if the current item is valid, if not then delete it (Exclude special items from this check)
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(pNMTreeView->itemNew.hItem));
  AFXASSUME(pItem);
  CString sPath(pItem->m_sFQPath);
  if ((pNMTreeView->itemNew.hItem != m_hRoot) &&
      (pNMTreeView->itemNew.hItem != m_hMyDocumentsRoot) &&
      (pNMTreeView->itemNew.hItem != m_hCurrentUserFolderRoot) &&
      (pNMTreeView->itemNew.hItem != m_hNetworkRoot) && (pItem->m_pNetResource == NULL) && 
      (pNMTreeView->itemNew.hItem != m_hMyComputerRoot) && !IsDrive(sPath) && (GetFileAttributes(sPath) == 0xFFFFFFFF))
  {
    //Before we delete it see if we are the only child item
    HTREEITEM hParent = GetParentItem(pNMTreeView->itemNew.hItem);

    //Delete the item
    DeleteItem(pNMTreeView->itemNew.hItem);

    //Remove all the child items from the parent
    SetHasPlusButton(hParent, FALSE);

    *pResult = 1;

    return FALSE; //Allow the message to be reflected again
  }

  //Add to the prev array the item we were just at
  if (pNMTreeView->itemOld.hItem && !m_bUpdatingHistorySelection)
  {
    if (m_PrevItems.GetSize() > m_nMaxHistory)
      m_PrevItems.RemoveAt(0);
    m_PrevItems.Add(pNMTreeView->itemOld.hItem);
  }

  //Remeber the serial number for this item (if it is a drive)
  if (IsDrive(sPath))
  {
    int nDrive = sPath.GetAt(0) - _T('A');
    GetSerialNumber(sPath, m_dwMediaID[nDrive]); 
  }

  //Setup notifications if required
  if (m_nAutoRefresh && CanHandleChangeNotifications(sPath))
    CreateMonitoringThread(pItem->m_sFQPath);
  else
    KillMonitoringThread();

  //call the virtual function
  OnSelectionChanged(pNMTreeView, sPath);
	
	*pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

void CFileTreeCtrl::OnSelectionChanged(NMTREEVIEW*, const CString&)
{
  //CFileTreeCtrl does nothing, but a derived class may want to do something
}

BOOL CFileTreeCtrl::OnEndLabelEdit(NMHDR* pNMHDR, LRESULT* pResult) 
{
	TV_DISPINFO* pDispInfo = reinterpret_cast<TV_DISPINFO*>(pNMHDR);
	AFXASSUME(pDispInfo);
  if (pDispInfo->item.pszText)
  {
    SHFILEOPSTRUCT shfo;
    memset(&shfo, 0, sizeof(SHFILEOPSTRUCT));
    shfo.hwnd = AfxGetMainWnd()->GetSafeHwnd();
    shfo.wFunc = FO_RENAME;
    shfo.fFlags = FOF_ALLOWUNDO;

    //Work out the "From" string
    CString sFrom(ItemToPath(pDispInfo->item.hItem));
    int nFromLength = sFrom.GetLength();
    TCHAR* pszFrom = new TCHAR[nFromLength + 2];
    _tcscpy_s(pszFrom, nFromLength+2, sFrom);
    pszFrom[nFromLength+1] = _T('\0');
    shfo.pFrom = pszFrom;
    HTREEITEM hParent = GetParentItem(pDispInfo->item.hItem);
    CString sParent(ItemToPath(hParent));

    //Work out the "To" string
    CString sTo;
    CString sToRelative(pDispInfo->item.pszText);
    if (IsDrive(sParent))
      sTo = sParent + pDispInfo->item.pszText;
    else
      sTo = sParent + _T("\\") + pDispInfo->item.pszText;
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(pDispInfo->item.hItem));
    AFXASSUME(pItem);
    if (pItem->m_bExtensionHidden)
    {
      TCHAR szExt[_MAX_EXT];
      _tsplitpath_s(sFrom, NULL, 0, NULL, 0, NULL, 0, szExt, sizeof(szExt)/sizeof(TCHAR));
      sTo += szExt;
      sToRelative += szExt;
    }
    size_t nToLength = _tcslen(sTo);
    TCHAR* pszTo = new TCHAR[nToLength + 2];
    _tcscpy_s(pszTo, nToLength+2, sTo);
    pszTo[nToLength+1] = _T('\0');
    shfo.pTo = pszTo;

    //Kill the monitoring thread if necessary
    BOOL bMonitoringThread = (m_pMonitoringThread != NULL);
    CString sMonitoringPath(m_sMonitoringPath);
    if (bMonitoringThread)
      KillMonitoringThread();

    //Let the shell perform the actual rename
    if (SHFileOperation(&shfo) == 0 && shfo.fAnyOperationsAborted == FALSE)
    {
      *pResult = TRUE;

      //Update its text  
      SetItemText(pDispInfo->item.hItem, pDispInfo->item.pszText);

      //Update the item data
      pItem->m_sFQPath = sTo;
      pItem->m_sRelativePath = sToRelative;

      //Also update the icons for it (if need be)
      if (!pItem->m_bExtensionHidden)
      {
        CString sPath(ItemToPath(pDispInfo->item.hItem));
        SetItemImage(pDispInfo->item.hItem, GetIconIndex(sPath), GetSelIconIndex(sPath));
      }
    }

		//Recreate the monitoring thread if necessary
		if (bMonitoringThread)
		  CreateMonitoringThread(m_sMonitoringPath);

    //Don't forget to free up the memory we allocated
    delete [] pszFrom;
    delete [] pszTo;
  }
	
	*pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

BOOL CFileTreeCtrl::PreTranslateMessage(MSG* pMsg) 
{
  //When an item is being edited make sure the edit control
  //receives certain important key strokes
  if (GetEditControl())
  {
    ::TranslateMessage(pMsg);
    ::DispatchMessage(pMsg);
    return TRUE; // DO NOT process further
  }

  //Context menu via the keyboard
	if ((((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) &&   //If we hit a key and
    	(pMsg->wParam == VK_F10) && (GetKeyState(VK_SHIFT) & 0x8000)) != 0) || //it's Shift+F10 OR
		  (pMsg->message == WM_CONTEXTMENU))						                   	     //Natural keyboard key
	{
		CRect rect;
		GetItemRect(GetSelectedItem(), rect, TRUE);
		ClientToScreen(rect);
		OnContextMenu(NULL, rect.CenterPoint());
		return TRUE;
	}
  //Hitting the Escape key cancels drag & drop
#ifndef FILETREECTRL_NO_RESOURCES	  
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_ESCAPE && IsDragging())
  {
    EndDragging(TRUE);
    return TRUE;
  }
#endif  
  //Hitting the Alt-Enter key combination shows the properties sheet 
	else if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_RETURN)
  {
    OnProperties();
    return TRUE;
  }
  //Hitting the Enter key opens the item
	else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_RETURN)
  {
    OnOpen();
    return TRUE;
  }
  //Hitting the delete key deletes the item
  else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_DELETE)
  {
    OnDelete();
    return TRUE;
  }
  //hitting the F2 key begins in-place editing of an item
  else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F2)
  {
    OnRename();
    return TRUE;
  }
  //hitting the F5 key forces a refresh of the whole tree
  else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_F5)
  {
    OnRefresh();
    return TRUE;
  }
  //Hitting the Alt-Left Arrow key combination moves to the previous item
	else if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_LEFT)
  {
    OnBack();
    return TRUE;
  }
  //Hitting the Alt-Right Arrow key combination moves to the next item
	else if (pMsg->message == WM_SYSKEYDOWN && pMsg->wParam == VK_RIGHT)
  {
    OnForward();
    return TRUE;
  }
  //hitting the backspace key goes to the parent folder
  else if (pMsg->message == WM_KEYDOWN && pMsg->wParam == VK_BACK)
  {
    UpOneLevel();
    return TRUE;
  }

  //Let the parent class do its thing
	return FILETREECTRL_BASE_CLASS::PreTranslateMessage(pMsg);
}

void CFileTreeCtrl::OnUpOneLevel() 
{
  HTREEITEM hItem = GetSelectedItem();
  if (hItem)
  {
    HTREEITEM hParent = GetParentItem(hItem);
    if (hParent)
      Select(hParent, TVGN_CARET);
  }
}

BOOL CFileTreeCtrl::IsFile(HTREEITEM hItem)
{
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
  AFXASSUME(pItem);
  return pItem->m_NodeType == CFileTreeCtrlItemInfo::FileNode;
}

BOOL CFileTreeCtrl::IsFolder(HTREEITEM hItem)
{
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
  AFXASSUME(pItem);
  return pItem->m_NodeType == CFileTreeCtrlItemInfo::FolderNode;
}

BOOL CFileTreeCtrl::IsDrive(HTREEITEM hItem)
{
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
  AFXASSUME(pItem);
  return pItem->m_NodeType == CFileTreeCtrlItemInfo::DriveNode;
}

BOOL CFileTreeCtrl::DriveHasRemovableMedia(const CString& sPath)
{
  BOOL bRemovableMedia = FALSE;
  if (IsDrive(sPath))
  {
    UINT nDriveType = GetDriveType(sPath);
    bRemovableMedia = ((nDriveType == DRIVE_REMOVABLE) ||
                       (nDriveType == DRIVE_CDROM)); 
  }

  return bRemovableMedia;
}

BOOL CFileTreeCtrl::IsCompressed(HTREEITEM hItem)
{
  return IsCompressed(ItemToPath(hItem));
}

BOOL CFileTreeCtrl::IsEncrypted(HTREEITEM hItem)
{
  return IsEncrypted(ItemToPath(hItem));
}

BOOL CFileTreeCtrl::IsFile(const CString& sPath)
{
  DWORD dwAttributes = GetFileAttributes(sPath);
  return ((dwAttributes != 0xFFFFFFFF) && ((dwAttributes & FILE_ATTRIBUTE_DIRECTORY) == 0));
}

BOOL CFileTreeCtrl::IsFolder(const CString& sPath)
{
  DWORD dwAttributes = GetFileAttributes(sPath);
  return ((dwAttributes != 0xFFFFFFFF) && (dwAttributes & FILE_ATTRIBUTE_DIRECTORY));
}

BOOL CFileTreeCtrl::IsDrive(const CString& sPath)
{
  return (sPath.GetLength() == 3 && sPath.GetAt(1) == _T(':') && sPath.GetAt(2) == _T('\\'));
}

BOOL CFileTreeCtrl::IsCompressed(const CString& sPath)
{
  BOOL bCompressed = FALSE;
  if (!IsDrive(sPath))
  {
    DWORD dwAttributes = GetFileAttributes(sPath);
    bCompressed = ((dwAttributes != 0xFFFFFFFF) && (dwAttributes & FILE_ATTRIBUTE_COMPRESSED));
  }

  return bCompressed;
}

BOOL CFileTreeCtrl::IsEncrypted(const CString& sPath)
{
  BOOL bEncrypted = FALSE;
  if (!IsDrive(sPath))
  {
    DWORD dwAttributes = GetFileAttributes(sPath);
    bEncrypted = ((dwAttributes != 0xFFFFFFFF) && (dwAttributes & FILE_ATTRIBUTE_ENCRYPTED));
  }

  return bEncrypted;
}

BOOL CFileTreeCtrl::HasGotSubEntries(const CString& sDirectory)
{
  ASSERT(sDirectory.GetLength());

  if (DriveHasRemovableMedia(sDirectory))
  {
    return TRUE; //we do not bother searching for files on drives 
                 //which have removable media as this would cause 
                 //the drive to spin up, which for the case of a 
                 //floppy is annoying
  }
  else
  {
    //First check to see if there is any sub directories  
    CFileFind find1;
    CString sFile;
    if (sDirectory.GetAt(sDirectory.GetLength() - 1) == _T('\\'))
      sFile = sDirectory + _T("*.*");
    else
      sFile = sDirectory + _T("\\*.*");
    BOOL bFind = find1.FindFile(sFile);  
    while (bFind)
    {
      bFind = find1.FindNextFile();
      if (CanDisplayFolder(find1))
        return TRUE;
    }

    //Now check to see if there is any files of the specfied file mask  
    INT_PTR nMasks = m_sFileNameMasks.GetSize();
    for (INT_PTR i=0; i<nMasks; i++)
    {
      CFileFind find2;
      if (sDirectory.GetAt(sDirectory.GetLength() - 1) == _T('\\'))
        sFile = sDirectory + m_sFileNameMasks.GetAt(i);
      else
        sFile = sDirectory + _T("\\") + m_sFileNameMasks.GetAt(i);
      bFind = find2.FindFile(sFile);  
      while (bFind)
      {
        bFind = find2.FindNextFile();
        if (CanDisplayFile(find2))
          return TRUE;
      }
    }
  }

  return FALSE;
}

void CFileTreeCtrl::PopulateTree()
{
  ASSERT(GetSafeHwnd()); //Should only call this function after the creation of it on screen

  //Attach the image list to the tree control
  SetImageList(&theSystemImageList.m_ImageList, TVSIL_NORMAL);

  //Force a refresh
  Refresh();
}

void CFileTreeCtrl::CreateMonitoringThread(const CString& sPath)
{
  //Kill any current notification thread
  KillMonitoringThread();

  //Setup the monitoring path
  m_sMonitoringPath = sPath;
  int nLength = m_sMonitoringPath.GetLength();
  ASSERT(nLength);
  if (nLength && m_sMonitoringPath.GetAt(nLength - 1) != _T('\\'))
    m_sMonitoringPath += _T('\\');

  TRACE(_T("CFileTreeCtrl::CreateMonitoringThread, Creating monitoring thread for %s\n"), m_sMonitoringPath.operator LPCTSTR());

  ASSERT(m_pMonitoringThread == NULL);
  m_pMonitoringThread = AfxBeginThread(_MonitoringThread, this, THREAD_PRIORITY_IDLE, 0, CREATE_SUSPENDED);
  if (m_pMonitoringThread)
  {
    //We're in charge of deletion of the thread
    m_pMonitoringThread->m_bAutoDelete = FALSE;

    //Resume the thread now that everything is ready to go
    m_pMonitoringThread->ResumeThread();
  }
}

UINT CFileTreeCtrl::_MonitoringThread(LPVOID pParam)
{
  //Validate our parameters
  ASSERT(pParam);
  CFileTreeCtrl* pThis = static_cast<CFileTreeCtrl*>(pParam);
  AFXASSUME(pThis);

  //Convert from the SDK world to the C++ world
  return pThis->MonitoringThread();
}

UINT CFileTreeCtrl::MonitoringThread()
{
  //Form the notification flag to use
  DWORD dwNotifyFilter = FILE_NOTIFY_CHANGE_DIR_NAME;
  if (m_bShowFiles)
    dwNotifyFilter |= FILE_NOTIFY_CHANGE_FILE_NAME;

  //Get a handle to a file change notification object
  HANDLE hChange = FindFirstChangeNotification(m_sMonitoringPath, FALSE, dwNotifyFilter);
  if (hChange != INVALID_HANDLE_VALUE)
  {
    //Setup the handles to wait on
    HANDLE handles[2];
    handles[0] = hChange;
    handles[1] = m_MonitoringTerminateEvent;

    //Sleep until a file change notification wakes this thread or m_TerminateEvent becomes
    //set indicating it's time for the thread to end
    BOOL bContinue = TRUE;
    while (bContinue)
    {
      DWORD dwWait = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
      if ((dwWait - WAIT_OBJECT_0) == 0)
      {
        //Respond to the change notification by posting a user defined message 
        //back to the GUI thread
        if (!m_nAutoRefresh)
          bContinue = FALSE;
        else
          PostMessage(WM_TREEUPDATE_CHANGE);
        
        //Move onto the next notification
        FindNextChangeNotification(hChange);
      }
      else
      {
        //Kill the thread
        bContinue = FALSE;
      }
    }

    //Close the handle we have open
    FindCloseChangeNotification(hChange);
  }

  return 0;
}

BOOL CFileTreeCtrl::GetSerialNumber(const CString& sDrive, DWORD& dwSerialNumber)
{
  return GetVolumeInformation(sDrive, NULL, 0, &dwSerialNumber, NULL, NULL, NULL, 0);
}

BOOL CFileTreeCtrl::IsMediaValid(const CString& sDrive)
{
  //return TRUE if the drive does not support removable media
  UINT nDriveType = GetDriveType(sDrive);
  if ((nDriveType != DRIVE_REMOVABLE) && (nDriveType != DRIVE_CDROM))
    return TRUE;

  //Return FALSE if the drive is empty (::GetVolumeInformation fails)
  DWORD dwSerialNumber;
  int nDrive = sDrive.GetAt(0) - _T('A');
  if (GetSerialNumber(sDrive, dwSerialNumber))
    m_dwMediaID[nDrive] = dwSerialNumber;
  else
  {
    m_dwMediaID[nDrive] = 0xFFFFFFFF;
    return FALSE;
  }

  //Also return FALSE if the disk's serial number has changed
  if ((m_dwMediaID[nDrive] != dwSerialNumber) &&
      (m_dwMediaID[nDrive] != 0xFFFFFFFF))
  {
    m_dwMediaID[nDrive] = 0xFFFFFFFF;
    return FALSE;
  }

  return TRUE;
}

BOOL CFileTreeCtrl::EnumNetwork(HTREEITEM hParent)
{
  //What will be the return value from this function
	BOOL bGotChildren = FALSE;

	//Check if the item already has a network resource and use it.
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hParent));
  AFXASSUME(pItem);
	NETRESOURCE* pNetResource = pItem->m_pNetResource;

	//Setup for the network enumeration
	HANDLE hEnum;      
	DWORD dwResult = WNetOpenEnum(pNetResource ? RESOURCE_GLOBALNET : RESOURCE_CONTEXT, m_dwNetworkItemTypes,
      								          0, pNetResource ? pNetResource : NULL, &hEnum);

	//Was the read sucessful
	if (dwResult != NO_ERROR)      
	{
		TRACE(_T("CFileTreeCtrl::EnumNetwork, Cannot enumerate network drives, Error:%d\n"), dwResult);
		return FALSE;
	} 
	
  //Do the network enumeration
	DWORD cbBuffer = 16384;

  BOOL bNeedMoreMemory = TRUE;
  BOOL bSuccess = FALSE;
  LPNETRESOURCE lpnrDrv = NULL;
	DWORD cEntries = 0;      
  while (bNeedMoreMemory && !bSuccess)
  {
    //Allocate the memory and enumerate
  	lpnrDrv = reinterpret_cast<LPNETRESOURCE>(new BYTE[cbBuffer]);
    cEntries = 0xFFFFFFFF;
		dwResult = WNetEnumResource(hEnum, &cEntries, lpnrDrv, &cbBuffer);

    if (dwResult == ERROR_MORE_DATA)
		{            
      //Free up the heap memory we have used
		  delete [] lpnrDrv;      

      cbBuffer *= 2;
		}
    else if (dwResult == NO_ERROR)
      bSuccess = TRUE;
    else
      bNeedMoreMemory = FALSE;
	}

  //Enumeration successful?
  if (bSuccess)
  {
		//Scan through the results
		for (DWORD i=0; i<cEntries; i++)            
		{
		  //Get the display name for the item
			CString sDisplayName(lpnrDrv[i].lpRemoteName);
			if (sDisplayName.IsEmpty())
				sDisplayName = lpnrDrv[i].lpComment;

			//Get the UNC path for the item
			CString sUNCPath;
			if (sDisplayName.Find(_T("\\\\")) == 0)
			{
			  sUNCPath = sDisplayName;
			  sDisplayName = sDisplayName.Right(sDisplayName.GetLength() - 2);
			}

      //Setup the item data for the new item
      CFileTreeCtrlItemInfo* pNewItem = new CFileTreeCtrlItemInfo;
			pNewItem->m_pNetResource = new NETRESOURCE;
      memset(pNewItem->m_pNetResource, 0, sizeof(NETRESOURCE));
			*pNewItem->m_pNetResource = lpnrDrv[i];
      if (lpnrDrv[i].lpLocalName)
				pNewItem->m_pNetResource->lpLocalName	= _tcsdup(lpnrDrv[i].lpLocalName);
      if (lpnrDrv[i].lpRemoteName)
				pNewItem->m_pNetResource->lpRemoteName = _tcsdup(lpnrDrv[i].lpRemoteName);
      if (lpnrDrv[i].lpComment)
				pNewItem->m_pNetResource->lpComment	= _tcsdup(lpnrDrv[i].lpComment);
      if (lpnrDrv[i].lpProvider)
				pNewItem->m_pNetResource->lpProvider	= _tcsdup(lpnrDrv[i].lpProvider);
      if (sUNCPath.GetLength())
        pNewItem->m_sFQPath = sUNCPath;
      pNewItem->m_sRelativePath = sDisplayName;
      pNewItem->m_NodeType = CFileTreeCtrlItemInfo::NetworkNode;

			//Display a share and the appropriate icon
			if (lpnrDrv[i].dwDisplayType == RESOURCEDISPLAYTYPE_SHARE)
			{
				//Display only the share name
				int nPos = pNewItem->m_sRelativePath.Find(_T('\\'));
				if (nPos >= 0)
					pNewItem->m_sRelativePath = pNewItem->m_sRelativePath.Mid(nPos+1);

        //Now add the item into the control
        if (CanDisplayNetworkItem(pNewItem))
          InsertFileItem(hParent, pNewItem, FALSE, GetIconIndex(pNewItem->m_sFQPath), GetSelIconIndex(pNewItem->m_sFQPath), TRUE);
        else
          delete pNewItem;
			}
			else if (lpnrDrv[i].dwDisplayType == RESOURCEDISPLAYTYPE_SERVER)
			{
        //Now add the item into the control
        if (CanDisplayNetworkItem(pNewItem))
        {
          //Note as an optimization we use the cached computer icon indexes for network computers. This helps to 
          //speed up speed up the performance of CFileTreeCtrl when browsing domains or workgroups with a large 
          //number of workstations and servers. You can decide to find the icon indexes at runtime if you define
          //the proprocessor value FILETREECTRL_NO_CACHE_COMPUTER_ICON
        #ifndef FILETREECTRL_NO_CACHE_COMPUTER_ICON    
          if ((m_nComputerIconIndex != -1) && (m_nSelectedComputerIconIndex != -1))
            InsertFileItem(hParent, pNewItem, FALSE, m_nComputerIconIndex, m_nSelectedComputerIconIndex, FALSE);
          else
          {
            CString sServer;
            sServer.Format(_T("\\\\%s"), pNewItem->m_sRelativePath.operator LPCTSTR());
            InsertFileItem(hParent, pNewItem, FALSE, GetIconIndex(sServer), GetSelIconIndex(sServer), FALSE);
          }
        #else
          CString sServer;
          sServer.Format(_T("\\\\%s"), pNewItem->m_sRelativePath.operator LPCTSTR());
          InsertFileItem(hParent, pNewItem, FALSE, GetIconIndex(sServer), GetSelIconIndex(sServer), FALSE);
        #endif    
        }
        else
          delete pNewItem;
			}
			else
			{
        //Now add the item into the control
        if (CanDisplayNetworkItem(pNewItem))
        {
          //Just use the generic Network Neighborhood icons for everything else
          LPITEMIDLIST lpNNPidl;
          int nIcon = 0xFFFF;
          int nSelIcon = nIcon;
          if (SUCCEEDED(SHGetSpecialFolderLocation(NULL, CSIDL_NETWORK, &lpNNPidl)))
          {
            nIcon = GetIconIndex(lpNNPidl);
            nSelIcon = GetSelIconIndex(lpNNPidl);

            //Free up the pidl now that we are finished with it
            ASSERT(m_pMalloc);
            m_pMalloc->Free(lpNNPidl);
            m_pMalloc->Release();
          }

          InsertFileItem(hParent, pNewItem, FALSE, nIcon, nSelIcon, FALSE);
        }
        else
          delete pNewItem;
			}
			bGotChildren = TRUE;
		}
  }
  else
	  TRACE(_T("CFileTreeCtrl::EnumNetwork, Cannot complete network drive enumeration, Error:%d\n"), dwResult);

	//Clean up the enumeration handle
	WNetCloseEnum(hEnum);   

  //Free up the heap memory we have used
	delete [] lpnrDrv;      

  //Return whether or not we added any items
	return bGotChildren;
}

void CFileTreeCtrl::DisplayDrives(HTREEITEM hParent, BOOL bUseSetRedraw)
{
	//Display the wait cursor while we are adding the drives to the control
  CWaitCursor c;

  //Speed up the job by turning off redraw
  if (bUseSetRedraw)
    SetRedraw(FALSE);

  //Enumerate the drive letters and add them to the tree control
  DWORD dwDrives = GetLogicalDrives();
  DWORD dwMask = 1;
  for (int i=0; i<32; i++)
  {
    if (dwDrives & dwMask)
    {
      CString sDrive;
      sDrive.Format(_T("%c:\\"), i + _T('A'));

      //check if this drive is one of the types to hide
      if (CanDisplayDrive(sDrive))
      {
        CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
        pItem->m_sFQPath = sDrive;
        pItem->m_sRelativePath = sDrive;
        pItem->m_NodeType = CFileTreeCtrlItemInfo::DriveNode;

        //Insert the item into the view
        InsertFileItem(hParent, pItem, m_bShowSharedUsingDifferentIcon && IsShared(sDrive), GetIconIndex(sDrive), GetSelIconIndex(sDrive), TRUE);
      }
    }
    dwMask <<= 1;
  }

  if (bUseSetRedraw)
    SetRedraw(TRUE);
}

CString CFileTreeCtrl::GetDriveLabel(const CString& sDrive)
{
  //Let's start with the drive letter
  CString sLabel(sDrive);

  //Try to find the item directory using ParseDisplayName
  LPITEMIDLIST lpItem;
  HRESULT hr = m_pShellFolder->ParseDisplayName(NULL, NULL, CT2W(sDrive), NULL, &lpItem, NULL);
  if (SUCCEEDED(hr))
  {
    SHFILEINFO sfi;
    if (SHGetFileInfo(reinterpret_cast<LPCTSTR>(lpItem), 0, &sfi, sizeof(sfi), SHGFI_PIDL | SHGFI_DISPLAYNAME))
      sLabel = sfi.szDisplayName;

    //Free the pidl now that we are finished with it
    m_pMalloc->Free(lpItem);
  }

  return sLabel;
}

BOOL CFileTreeCtrl::IsShared(const CString& sPath)
{
  //Defer all the work to the share enumerator class
  return theSharedEnumerator.IsShared(sPath);
}

void CFileTreeCtrl::DisplayPath(const CString& sPath, HTREEITEM hParent, BOOL bUseSetRedraw, BOOL bDeleteExistingSiblings)
{
  //Display the wait cursor while we are adding the items to the control
  CWaitCursor c;

  //Speed up the job by turning off redraw
  if (bUseSetRedraw)
    SetRedraw(FALSE);

  //Remove all the items currently under hParent if required
  if (bDeleteExistingSiblings)
  {
    HTREEITEM hChild = GetChildItem(hParent);
    while (hChild)
    {
      DeleteItem(hChild);
      hChild = GetChildItem(hParent);
    }
  }

  //Should we display the root folder
  if (m_bShowRootedFolder && (hParent == m_hRoot))
  {
    CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
    pItem->m_NodeType = CFileTreeCtrlItemInfo::FolderNode;
    pItem->m_sFQPath = m_sRootFolder;
    pItem->m_sRelativePath = m_sRootFolder;
    m_hRootedFolder = InsertFileItem(m_hRoot, pItem, FALSE, GetIconIndex(m_sRootFolder), GetSelIconIndex(m_sRootFolder), TRUE);
    Expand(m_hRootedFolder, TVE_EXPAND);
    return;
  }

  //find all the directories underneath sPath
  int nDirectories = 0;
  CFileFind find1;
  CString sFile;
  if (sPath.GetAt(sPath.GetLength() - 1) == _T('\\'))
    sFile = sPath + _T("*.*");
  else
    sFile = sPath + _T("\\*.*");
  BOOL bFind = find1.FindFile(sFile);  
  while (bFind)
  {
    bFind = find1.FindNextFile();
    if (find1.IsDirectory())
    {
      if (CanDisplayFolder(find1))
      {
        ++nDirectories;
        CString sTempPath(find1.GetFilePath());

        TCHAR szPath[_MAX_PATH];
        TCHAR szFname[_MAX_FNAME];
        TCHAR szExt[_MAX_EXT];
        _tsplitpath_s(sTempPath, NULL, 0, NULL, 0, szFname, sizeof(szFname)/sizeof(TCHAR), szExt, sizeof(szExt)/sizeof(TCHAR));
        _tmakepath_s(szPath, sizeof(szPath)/sizeof(TCHAR), NULL, NULL, szFname, szExt);

        CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
        pItem->m_sFQPath = sTempPath;
        pItem->m_sRelativePath = szPath;
        pItem->m_NodeType = CFileTreeCtrlItemInfo::FolderNode;
        InsertFileItem(hParent, pItem, m_bShowSharedUsingDifferentIcon && IsShared(sTempPath), GetIconIndex(sTempPath), GetSelIconIndex(sTempPath), TRUE);
      }
    }
  }

  //find all the files underneath sPath
  int nFiles = 0;
  INT_PTR nMasks = m_sFileNameMasks.GetSize();
  for (INT_PTR i=0; i<nMasks; i++)
  {
    CFileFind find2;
    if (sPath.GetAt(sPath.GetLength() - 1) == _T('\\'))
      sFile = sPath + m_sFileNameMasks.GetAt(i);
    else
      sFile = sPath + _T("\\") + m_sFileNameMasks.GetAt(i);
    bFind = find2.FindFile(sFile);  
    while (bFind)
    {
      bFind = find2.FindNextFile();
      if (!find2.IsDirectory())
      {
        if (CanDisplayFile(find2))
        {
          ++nFiles;
          CString sTempPath(find2.GetFilePath());

          TCHAR szPath[_MAX_PATH];
          TCHAR szFname[_MAX_FNAME];
          TCHAR szExt[_MAX_EXT];
          _tsplitpath_s(sTempPath, NULL, 0, NULL, 0, szFname, sizeof(szFname)/sizeof(TCHAR), szExt, sizeof(szExt)/sizeof(TCHAR));
          _tmakepath_s(szPath, sizeof(szPath)/sizeof(TCHAR), NULL, NULL, szFname, szExt);

          CFileTreeCtrlItemInfo* pItem = new CFileTreeCtrlItemInfo;
          pItem->m_sFQPath = sTempPath;
          pItem->m_sRelativePath = szPath;
          pItem->m_NodeType = CFileTreeCtrlItemInfo::FileNode;
          InsertFileItem(hParent, pItem, FALSE, GetIconIndex(sTempPath), GetSelIconIndex(sTempPath), TRUE);
        }  
      }
    }
  }

  //Now sort the items we just added
  TVSORTCB tvsortcb;
  tvsortcb.hParent = hParent;
  tvsortcb.lpfnCompare = CompareByFilenameNoCase;
  tvsortcb.lParam = 0;
  SortChildrenCB(&tvsortcb);

  //If no items were added then remove the "+" indicator from hParent
  if (nFiles == 0 && nDirectories == 00 && hParent != m_hRoot)
    SetHasPlusButton(hParent, FALSE);

  //Turn back on the redraw flag
  if (bUseSetRedraw)
    SetRedraw(TRUE);
}

void CFileTreeCtrl::DoExpand(HTREEITEM hItem)
{
  CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
  AFXASSUME(pItem);

  if (hItem == m_hRoot && m_sRootFolder.IsEmpty())
  {
    DisplayRootItems();
    
 	  //Also show all items underneath the desktop
    if (m_sDesktopPath.GetLength())
		  DisplayPath(m_sDesktopPath, m_hRoot, FALSE, FALSE);
  }
  else if (hItem == m_hMyComputerRoot)
  {
    //Display the wait cursor as this may take some time
    CWaitCursor wait;

    //Enumerate the local drive letters
    DisplayDrives(m_hMyComputerRoot, FALSE);
  }
  else if ((hItem == m_hNetworkRoot) || ((pItem->m_pNetResource) && !IsFolder(pItem->m_sFQPath)))
  {
    //Display the wait cursor as this may take some time
    CWaitCursor wait;

    //Enumerate the network resources
    EnumNetwork(hItem);
  }
  else if (pItem->m_sFQPath.GetLength())
  {
    if (IsMediaValid(pItem->m_sFQPath))
    {
      //Delete the item if the path is no longer valid
      if (IsFolder(pItem->m_sFQPath))
      {
        //Add the new items to the tree if it does not have any child items
        //already
        if (!GetChildItem(hItem))
          DisplayPath(pItem->m_sFQPath, hItem);
      }
      else
      {
        //Before we delete it see if we are the only child item
        HTREEITEM hParent = GetParentItem(hItem);

        //Delete the item
        DeleteItem(hItem);

        //Remove all the child items from the parent
        SetHasPlusButton(hParent, FALSE);
      }
    }
    else if (hItem != m_hRoot)
    {
      //Display the wait cursor as this may take some time
      CWaitCursor wait;

      //Collapse the drive node and remove all the child items from it
      Expand(hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);
      SetHasPlusButton(hItem, FALSE);
    }
  }
}

BOOL CFileTreeCtrl::OnItemExpanding(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pNMHDR);
	AFXASSUME(pNMTreeView);
	
  if (pNMTreeView->action == TVE_EXPAND)
  {
    BOOL bHasPlus = HasPlusButton(pNMTreeView->itemNew.hItem);
    BOOL bHasChildren = (GetChildItem(pNMTreeView->itemNew.hItem) != NULL);

    if (bHasPlus && !bHasChildren)
      DoExpand(pNMTreeView->itemNew.hItem);
  }
  else if (pNMTreeView->action == TVE_COLLAPSE)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(pNMTreeView->itemNew.hItem));
    AFXASSUME(pItem);

    //Display the wait cursor  as this may take some time
    CWaitCursor wait;

    //Collapse the node and remove all the child items from it
    BOOL bHasChildren = HasPlusButton(pNMTreeView->itemNew.hItem);
    Expand(pNMTreeView->itemNew.hItem, TVE_COLLAPSE | TVE_COLLAPSERESET);

    //Never uppdate the child indicator for a network node which is not a share
    BOOL bUpdateChildIndicator = TRUE;
    if (pItem->m_NodeType == CFileTreeCtrlItemInfo::NetworkNode)
    {
      if (pItem->m_pNetResource)
        bUpdateChildIndicator = (pItem->m_pNetResource->dwDisplayType == RESOURCEDISPLAYTYPE_SHARE);
      else
        bUpdateChildIndicator = FALSE;                              
    }  

    if (bUpdateChildIndicator)
      SetHasPlusButton(pNMTreeView->itemNew.hItem, bHasChildren);
  }

  *pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

HTREEITEM CFileTreeCtrl::PathToItem(const CString& sPath) const 
{
  CString sSearch(sPath);
  sSearch.MakeUpper();
  int nSearchLength = sSearch.GetLength();
  if (nSearchLength == 0)
    return NULL;

  //Remove initial part of path if the root folder is setup
  int nRootLength = m_sRootFolder.GetLength();
  CString sRootFolder(m_sRootFolder);
  sRootFolder.MakeUpper();
  if (nRootLength)
  {
    if (sSearch.Find(sRootFolder) != 0)
    {
      TRACE(_T("CFileTreeCtrl::PathToItem, Could not find the path %s as the root has been configured as %s\n"), sPath.operator LPCTSTR(), m_sRootFolder.operator LPCTSTR());
      return NULL;
    }
    sSearch = sSearch.Right(nSearchLength - 1 - nRootLength);
    nSearchLength = sSearch.GetLength();
  }

  //Remove trailing "\" from the path
  if (nSearchLength > 3 && sSearch.GetAt(nSearchLength - 1) == _T('\\'))
    sSearch = sSearch.Left(nSearchLength-1);

  if (sSearch.IsEmpty())
  {
    if (m_hRootedFolder)
      return m_hRootedFolder;
    else
      return NULL;
  }

  HTREEITEM hItemFound = m_hRoot;
  if (nRootLength && m_hRootedFolder)
    hItemFound = m_hRootedFolder;
  BOOL bDriveMatch = sRootFolder.IsEmpty();
  BOOL bNetworkMatch = m_bShowNetwork && ((sSearch.GetLength() > 2) && sSearch.Find(_T("\\\\")) == 0);
  if (bNetworkMatch)
  {
    bDriveMatch = FALSE;
    hItemFound = FindServersNode(m_hNetworkRoot);
    sSearch = sSearch.Right(sSearch.GetLength() - 2);
  }
  if (bDriveMatch)
  {
    CString sMyDocumentsPath(m_sMyDocumentsPath);
    sMyDocumentsPath.MakeUpper();
    CString sMyDesktopPath(m_sDesktopPath);
    sMyDesktopPath.MakeUpper();
    CString sCurrentUserFolderPath(m_sCurrentUserFolderPath);
    sCurrentUserFolderPath.MakeUpper();

    if (m_hDesktop && sSearch.Find(sMyDesktopPath) == 0)
    {
      hItemFound = m_hDesktop;

      sSearch = sSearch.Right(sSearch.GetLength() - sMyDesktopPath.GetLength() - 1);
      bDriveMatch = FALSE;
    }
    else if (m_hCurrentUserFolderRoot && sSearch.Find(sCurrentUserFolderPath) == 0)
    {
      hItemFound = m_hCurrentUserFolderRoot;

      sSearch = sSearch.Right(sSearch.GetLength() - sCurrentUserFolderPath.GetLength() - 1);
      bDriveMatch = FALSE;
    }
    else if (m_hMyDocumentsRoot && sSearch.Find(sMyDocumentsPath) == 0)
    {
      hItemFound = m_hMyDocumentsRoot;

      sSearch = sSearch.Right(sSearch.GetLength() - sMyDocumentsPath.GetLength() - 1);
      bDriveMatch = FALSE;
    }
    else if (m_hMyComputerRoot)
    { 
      hItemFound = m_hMyComputerRoot;
    }
  }
  
  int nFound = sSearch.Find(_T('\\'));
  while (nFound != -1)
  {
    CString sMatch;
    if (bDriveMatch)
    {
      sMatch = sSearch.Left(nFound + 1);
      bDriveMatch = FALSE;
    }
    else
      sMatch = sSearch.Left(nFound);
    hItemFound = FindSibling(hItemFound, sMatch);
    if (hItemFound == NULL)
      break;

    sSearch = sSearch.Right(sSearch.GetLength() - nFound - 1);
    nFound = sSearch.Find(_T('\\'));
  };

  //The last item
  if (hItemFound)
  {
    if (sSearch.GetLength())
      hItemFound = FindSibling(hItemFound, sSearch);
  }

  return hItemFound;
}

CString CFileTreeCtrl::ItemToPath(HTREEITEM hItem) const
{
  CString sPath;
  if (hItem)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(GetItemData(hItem));
    AFXASSUME(pItem);
    sPath = pItem->m_sFQPath;
  }
  return sPath;
}

BOOL CFileTreeCtrl::OnRclick(NMHDR* /*pNMHDR*/, LRESULT* pResult) 
{
  //Work out the position of where the context menu should be
  CPoint p(GetCurrentMessage()->pt);
  CPoint pt(p);
  ScreenToClient(&pt);
  Select(HitTest(pt), TVGN_CARET);
	OnContextMenu(NULL, p);

	*pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

//Copied from CFrameWnd::OnInitMenuPopup to provide OnUpdateCmdUI functionality
//in the tree control
void CFileTreeCtrl::OnInitMenuPopup(CMenu* pMenu, UINT /*nIndex*/, BOOL bSysMenu) 
{
	//AfxCancelModes(m_hWnd);

	if (bSysMenu)
		return;     // don't support system menu

	AFXASSUME(pMenu != NULL);
	// check the enabled state of various menu items

	CCmdUI state;
	state.m_pMenu = pMenu;
	ASSERT(state.m_pOther == NULL);
	ASSERT(state.m_pParentMenu == NULL);

	// determine if menu is popup in top-level menu and set m_pOther to
	//  it if so (m_pParentMenu == NULL indicates that it is secondary popup)
	HMENU hParentMenu;
	if (AfxGetThreadState()->m_hTrackingMenu == pMenu->m_hMenu)
		state.m_pParentMenu = pMenu;    // parent == child for tracking popup
	else if ((hParentMenu = ::GetMenu(m_hWnd)) != NULL)
	{
		CWnd* pParent = GetTopLevelParent();
			// child windows don't have menus -- need to go to the top!
		if (pParent != NULL &&
			(hParentMenu = ::GetMenu(pParent->m_hWnd)) != NULL)
		{
			int nIndexMax = ::GetMenuItemCount(hParentMenu);
			for (int nIndex = 0; nIndex < nIndexMax; nIndex++)
			{
				if (::GetSubMenu(hParentMenu, nIndex) == pMenu->m_hMenu)
				{
					// when popup is found, m_pParentMenu is containing menu
					state.m_pParentMenu = CMenu::FromHandle(hParentMenu);
					break;
				}
			}
		}
	}

	state.m_nIndexMax = pMenu->GetMenuItemCount();
	for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
	  state.m_nIndex++)
	{
		state.m_nID = pMenu->GetMenuItemID(state.m_nIndex);
		if (state.m_nID == 0)
			continue; // menu separator or invalid cmd - ignore it

		ASSERT(state.m_pOther == NULL);
		ASSERT(state.m_pMenu != NULL);
		if (state.m_nID == (UINT)-1)
		{
			// possibly a popup menu, route to first item of that popup
			state.m_pSubMenu = pMenu->GetSubMenu(state.m_nIndex);
			if (state.m_pSubMenu == NULL ||
				(state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
				state.m_nID == (UINT)-1)
			{
				continue;       // first item of popup can't be routed to
			}
			state.DoUpdate(this, FALSE);    // popups are never auto disabled
		}
		else
		{
			// normal menu item
			// Auto enable/disable if frame window has 'm_bAutoMenuEnable'
			//    set and command is _not_ a system command.
			state.m_pSubMenu = NULL;
			//state.DoUpdate(this, m_bAutoMenuEnable && state.m_nID < 0xF000);
      state.DoUpdate(this, TRUE && state.m_nID < 0xF000);
		}

		// adjust for menu deletions and additions
		UINT nCount = pMenu->GetMenuItemCount();
		if (nCount < state.m_nIndexMax)
		{
			state.m_nIndex -= (state.m_nIndexMax - nCount);
			while (state.m_nIndex < nCount &&
				pMenu->GetMenuItemID(state.m_nIndex) == state.m_nID)
			{
				state.m_nIndex++;
			}
		}
		state.m_nIndexMax = nCount;
	}
}

#ifndef FILETREECTRL_NO_RESOURCES
BOOL CFileTreeCtrl::OnBeginDrag(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pNMHDR);
	AFXASSUME(pNMTreeView);
	
	*pResult = 0;

  if (!m_bAllowDragDrop || !IsDropSource(pNMTreeView->itemNew.hItem))
    return FALSE; //Allow the message to be reflected again

  m_pilDrag = CreateDragImage(pNMTreeView->itemNew.hItem);
  if (!m_pilDrag)
    return FALSE; //Allow the message to be reflected again

  m_hItemDrag = pNMTreeView->itemNew.hItem;
  m_hItemDrop = NULL;

  //Calculate the offset to the hotspot
  CPoint offsetPt(8,8); //Initialize a default offset

  CPoint dragPt(pNMTreeView->ptDrag); //Get the Drag point
  UINT nHitFlags = 0;
  HTREEITEM htiHit = HitTest(dragPt, &nHitFlags);
  if (htiHit != NULL)
  {
    //The drag point has Hit an item in the tree
    CRect itemRect;
    if (GetItemRect(htiHit, &itemRect, FALSE))
    {
      //Count indent levels
      HTREEITEM htiParent = htiHit;
      int nIndentCnt = 0;
      while (htiParent != NULL)
      {
        htiParent = GetParentItem(htiParent);
        nIndentCnt++;
      }

      if (!(GetStyle() & TVS_LINESATROOT)) 
        nIndentCnt--;

      //Calculate the new offset
      offsetPt.y = dragPt.y - itemRect.top;
      offsetPt.x = dragPt.x - (nIndentCnt * GetIndent()) + GetScrollPos(SB_HORZ);
    }
  }

  //Begin the dragging  
  m_pilDrag->BeginDrag(0, offsetPt);
  POINT pt = pNMTreeView->ptDrag;
  ClientToScreen(&pt);
  m_pilDrag->DragEnter(NULL, pt);
  SetCapture();

  //Create the timer which is used for auto expansion
  m_nTimerID = SetTimer(FILETREECTRL_TIMERID, 300, NULL);

  return FALSE; //Allow the message to be reflected again
}
#endif

#ifndef FILETREECTRL_NO_RESOURCES
void CFileTreeCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	if (IsDragging())
  {
    CRect clientRect;
    GetClientRect(&clientRect);

    //Draw the drag
    POINT pt = point;
    ClientToScreen(&pt);
    CImageList::DragMove(pt);

    //Only select the drop item if we are in the client area
    HTREEITEM hItem = NULL;
    if (clientRect.PtInRect(point))
    {
      UINT flags = 0;
      hItem = HitTest(point, &flags);
      if (m_hItemDrop != hItem)
      {
        CImageList::DragShowNolock(FALSE);
        SelectDropTarget(hItem);
        m_hItemDrop = hItem;
        CImageList::DragShowNolock(TRUE);
      }
    }
    
    if (hItem)
      hItem = GetDropTarget(hItem);

    //Change the cursor to give feedback
    if (hItem)
    {
      if ((GetKeyState(VK_CONTROL) & 0x8000))
        SetCursor(m_DropCopyCursor);
      else
        SetCursor(m_DropMoveCursor);
    }
    else
    {
      if ((GetKeyState(VK_CONTROL) & 0x8000))
        SetCursor(m_NoDropCopyCursor);
      else
        SetCursor(m_NoDropMoveCursor);
    }
  }

  //Let the parent class do its thing	
	FILETREECTRL_BASE_CLASS::OnMouseMove(nFlags, point);
}
#endif

#ifndef FILETREECTRL_NO_RESOURCES
BOOL CFileTreeCtrl::IsDropSource(HTREEITEM hItem)
{
  return IsFile(hItem) || IsFolder(hItem);
}

HTREEITEM CFileTreeCtrl::GetDropTarget(HTREEITEM hItem)
{
  if (!IsFile(hItem) && (hItem != m_hItemDrag) && (hItem != GetParentItem(m_hItemDrag)) && IsFolder(hItem))
  {
    HTREEITEM htiParent = hItem;
    while ((htiParent = GetParentItem(htiParent)) != NULL)
    {
      if (htiParent == m_hItemDrag)
        return NULL;
    }
    return hItem;
  }
  return NULL;
}

BOOL CFileTreeCtrl::IsDragging()
{
  return (m_pilDrag != NULL);
}

void CFileTreeCtrl::OnLButtonUp(UINT nFlags, CPoint point) 
{
  CRect clientRect;
  GetClientRect(&clientRect);

  if (clientRect.PtInRect(point))
    EndDragging(FALSE);
  else
    EndDragging(TRUE);
	 
  //Let the parent class do its thing	
	FILETREECTRL_BASE_CLASS::OnLButtonUp(nFlags, point);
}

void CFileTreeCtrl::EndDragging(BOOL bCancel)
{
  if (IsDragging())
  {
    //Kill the timer that is being used
    KillTimer(m_nTimerID);
    m_nTimerID = 0;

    CImageList::DragLeave(this);
    CImageList::EndDrag();
    ReleaseCapture();

    //Delete the drag image list
    delete m_pilDrag;
    m_pilDrag = NULL;

    //Remove drop target highlighting
    SelectDropTarget(NULL);

    //Find out where we are dropping
    m_hItemDrop = GetDropTarget(m_hItemDrop);
    if (m_hItemDrop == NULL)
      return;

    if (!bCancel)
    {
      //Also need to make the change on disk
      CString sFromPath(ItemToPath(m_hItemDrag));
      CString sToPath(ItemToPath(m_hItemDrop));

      int nFromLength = sFromPath.GetLength();
      int nToLength = sToPath.GetLength();
      SHFILEOPSTRUCT shfo;
      memset(&shfo, 0, sizeof(SHFILEOPSTRUCT));
      shfo.hwnd = GetSafeHwnd();

      if ((GetKeyState(VK_CONTROL) & 0x8000))
        shfo.wFunc = FO_COPY;
      else
        shfo.wFunc = FO_MOVE;

      shfo.fFlags = FOF_SILENT | FOF_NOCONFIRMMKDIR;
      //Undo is not allowed if the SHIFT key is held down
      if (!(GetKeyState(VK_SHIFT) & 0x8000))
        shfo.fFlags |= FOF_ALLOWUNDO;

      TCHAR* pszFrom = new TCHAR[nFromLength + 2];
      _tcscpy_s(pszFrom, nFromLength+2, sFromPath);
      pszFrom[nFromLength+1] = _T('\0');
      shfo.pFrom = pszFrom;

      TCHAR* pszTo = new TCHAR[nToLength + 2];
      _tcscpy_s(pszTo, nToLength+2, sToPath);
      pszTo[nToLength+1] = _T('\0');
      shfo.pTo = pszTo;

      //Kill the monitoring thread if necessary
      BOOL bMonitoringThread = (m_pMonitoringThread != NULL);
      CString sMonitoringPath(m_sMonitoringPath);
      if (bMonitoringThread)
        KillMonitoringThread();

      //Let the shell perform the actual deletion
      BOOL bSuccess = ((SHFileOperation(&shfo) == 0) && (shfo.fAnyOperationsAborted == FALSE));

		  //Recreate the monitoring thread if necessary
		  if (bMonitoringThread)
		    CreateMonitoringThread(m_sMonitoringPath);

      //Free up the memory we had allocated
      delete [] pszFrom;
      delete [] pszTo;

      if (bSuccess)
      {
        //Only copy the item in the tree if there is not an item with the same
        //text under m_hItemDrop
        CString sText(GetItemText(m_hItemDrag));
        if (!HasChildWithText(m_hItemDrop, sText))
        {
          Expand(m_hItemDrop, TVE_COLLAPSE | TVE_COLLAPSERESET);
          SelectItem(m_hItemDrop);

          //Update the children indicator for the folder we just dropped into
          SetHasPlusButton(m_hItemDrop, TRUE);
        }

        if (shfo.wFunc == FO_MOVE)
        {
          //Get the parent of the item we moved prior to deleting it
          HTREEITEM hParent = GetParentItem(m_hItemDrag);

          //Delete the item we just moved
          DeleteItem(m_hItemDrag);

          //Update the children indicator for the item we just dragged from
          BOOL bHasChildren = (GetChildItem(hParent) != NULL);
          if (hParent && !bHasChildren)
            SetHasPlusButton(hParent, FALSE);
        }
      }
    }
  }
}

void CFileTreeCtrl::OnTimer(UINT_PTR nIDEvent) 
{
	if (nIDEvent != m_nTimerID)
  {
	  FILETREECTRL_BASE_CLASS::OnTimer(nIDEvent);
    return;
  }

  //Show the dragging effect
  POINT pt;
  GetCursorPos(&pt);
  RECT rect;
  GetClientRect(&rect);
  ClientToScreen(&rect);
  CImageList::DragMove(pt);

  HTREEITEM hFirstItem = GetFirstVisibleItem();
  CRect ItemRect;
  GetItemRect(hFirstItem, &ItemRect, FALSE);
  if (hFirstItem != GetRootItem() && pt.y < (rect.top + (ItemRect.Height()*2)) && pt.y > rect.top)
  {
    //we need to scroll up
    CImageList::DragShowNolock(FALSE);
    SendMessage(WM_VSCROLL, SB_LINEUP);
    EnsureVisible(hFirstItem);
    SelectDropTarget(hFirstItem);
    m_hItemDrop = hFirstItem;
    CImageList::DragShowNolock(TRUE);
  }
  else if (pt.y > (rect.bottom - (ItemRect.Height()*2)) && pt.y < rect.bottom)
  {
    //we need to scroll down
    CImageList::DragShowNolock(FALSE);
    SendMessage(WM_VSCROLL, SB_LINEDOWN);
    HTREEITEM hLastItem = hFirstItem;
    int nCount = GetVisibleCount();
    for (int i=0; i<(nCount-1); i++)
      hLastItem = GetNextVisibleItem(hLastItem);
    SelectDropTarget(hLastItem);
    EnsureVisible(hLastItem);
    m_hItemDrop = hLastItem;
    CImageList::DragShowNolock(TRUE);
  }

  //Expand the item if the timer ticks has expired
  if (m_TimerTicks == FILETREECTRL_TIMERTICK_FOR_EXPANSION)
  {
    m_TimerTicks = 0;
    Expand(m_hItemDrop, TVE_EXPAND);
  }

  //Expand the selected item if it is collapsed and
  //the timeout has occurred
  TVITEM tvItem;
  tvItem.hItem = m_hItemDrop;
  tvItem.mask = TVIF_HANDLE | TVIF_CHILDREN | TVIF_STATE;
  tvItem.stateMask = TVIS_EXPANDED;
  GetItem(&tvItem);
  if (tvItem.cChildren && ((tvItem.state & TVIS_EXPANDED) == 0))
    m_TimerTicks++;
}
#endif

BOOL CFileTreeCtrl::HasChildWithText(HTREEITEM hParent, const CString& sText)
{
  HTREEITEM hChild = GetChildItem(hParent);
  while (hChild)
  {
    CString sItemText(GetItemText(hChild));
    if (sItemText.CompareNoCase(sText) == 0)
      return TRUE;
    hChild = GetNextSiblingItem(hChild);
  }
  return FALSE;
}

HTREEITEM CFileTreeCtrl::CopyItem(HTREEITEM hItem, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
  //Get the details of the item to copy
  TVINSERTSTRUCT tvstruct;
  tvstruct.item.hItem = hItem;
  tvstruct.item.mask = TVIF_CHILDREN | TVIF_HANDLE | TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_PARAM;
  GetItem(&tvstruct.item);
  CString sText(GetItemText(hItem));
  tvstruct.item.cchTextMax = sText.GetLength();
  tvstruct.item.pszText = sText.GetBuffer(tvstruct.item.cchTextMax);

  //Make a copy of the item data we are carying around
  CFileTreeCtrlItemInfo* pOldInfo = reinterpret_cast<CFileTreeCtrlItemInfo*>(tvstruct.item.lParam);
  tvstruct.item.lParam = reinterpret_cast<LPARAM>(new CFileTreeCtrlItemInfo(*pOldInfo));

  //Insert the item at the proper location
  tvstruct.hParent = htiNewParent;
  tvstruct.hInsertAfter = htiAfter;
  tvstruct.item.mask |= TVIF_TEXT;
  HTREEITEM hNewItem = InsertItem(&tvstruct);

  //Don't forget to release the CString buffer  
  sText.ReleaseBuffer();

  return hNewItem;
}

HTREEITEM CFileTreeCtrl::CopyBranch(HTREEITEM htiBranch, HTREEITEM htiNewParent, HTREEITEM htiAfter)
{
  HTREEITEM hNewItem = CopyItem(htiBranch, htiNewParent, htiAfter);
  HTREEITEM hChild = GetChildItem(htiBranch);
  while (hChild != NULL)
  {
    //recursively transfer all the items
    CopyBranch(hChild, hNewItem);
    hChild = GetNextSiblingItem(hChild);
  }
  return hNewItem;
}

void CFileTreeCtrl::OnBack() 
{
  INT_PTR nSize = m_PrevItems.GetSize();
  if (nSize)
  {
    HTREEITEM hOldItem = GetSelectedItem();
    HTREEITEM hNewItem = m_PrevItems.GetAt(nSize - 1);

    //Select the previous item
    m_bUpdatingHistorySelection = TRUE;
    m_PrevItems.RemoveAt(nSize - 1);
    SelectItem(hNewItem);
    EnsureVisible(hNewItem);
    m_bUpdatingHistorySelection = FALSE;

    //Add the old item to the next stack
    m_NextItems.Add(hOldItem);
  }
}

void CFileTreeCtrl::OnUpdateBack(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanGoBack());
}

void CFileTreeCtrl::OnForward() 
{
  INT_PTR nSize = m_NextItems.GetSize();
  if (nSize)
  {
    HTREEITEM hOldItem = GetSelectedItem();
    HTREEITEM hNewItem = m_NextItems.GetAt(nSize - 1);

    //Select the previous item
    m_bUpdatingHistorySelection = TRUE;
    m_NextItems.RemoveAt(nSize - 1);
    SelectItem(hNewItem);
    EnsureVisible(hNewItem);
    m_bUpdatingHistorySelection = FALSE;

    //Add the old item to the prev stack
    m_PrevItems.Add(hOldItem);
  }
}

void CFileTreeCtrl::OnUpdateForward(CCmdUI* pCmdUI) 
{
	pCmdUI->Enable(CanGoForward());	
}

void CFileTreeCtrl::SetMaxHistory(INT_PTR nMaxHistory)
{
  m_nMaxHistory = nMaxHistory;

  //Shrink the prev array if necessary
  INT_PTR nCurItems = m_PrevItems.GetSize();
  if (nCurItems > m_nMaxHistory)
  {
    INT_PTR nItemsToDelete = nCurItems - m_nMaxHistory;
    for (INT_PTR i=0; i<nItemsToDelete; i++)
      m_PrevItems.RemoveAt(nCurItems - i - 1);
  }

  //Shrink the next array if necessary
  nCurItems = m_NextItems.GetSize();
  if (nCurItems > m_nMaxHistory)
  {
    INT_PTR nItemsToDelete = nCurItems - m_nMaxHistory;
    for (INT_PTR i=0; i<nItemsToDelete; i++)
      m_NextItems.RemoveAt(nCurItems - i - 1);
  }
}

INT_PTR CFileTreeCtrl::GetBackSize() const
{
  return m_PrevItems.GetSize();
}

CString CFileTreeCtrl::GetBackItemText(INT_PTR nBack) const
{
  ASSERT(nBack < GetBackSize());
  HTREEITEM hItem = m_PrevItems.GetAt(nBack);
  return ItemToPath(hItem);
}

INT_PTR CFileTreeCtrl::GetForwardSize() const
{
  return m_NextItems.GetSize();
}

CString CFileTreeCtrl::GetForwardItemText(INT_PTR nForward) const
{
  ASSERT(nForward < GetForwardSize());
  HTREEITEM hItem = m_NextItems.GetAt(nForward);
  return ItemToPath(hItem);
}

void CFileTreeCtrl::KillMonitoringThread()
{
  if (m_pMonitoringThread)
  {
	  //Kill all the running file change notification thread
    TRACE(_T("CFileTreeCtrl::KillNotificationThread, Killing monitoring thread for %s\n"), m_sMonitoringPath.operator LPCTSTR());

    //Signal the worker thread to exit and wait for it to return
    m_MonitoringTerminateEvent.SetEvent();
    WaitForSingleObject(m_pMonitoringThread->m_hThread, INFINITE);
    delete m_pMonitoringThread;
    m_pMonitoringThread = NULL;
  }
}

void CFileTreeCtrl::OnDestroy() 
{
  //Kill the monitoring thread
  KillMonitoringThread();

  //Remove all the items from the tree control 
  //This ensures that all the heap memory we
  //have used in the item datas is freed
  Clear();

  //Let the parent class do its thing
	FILETREECTRL_BASE_CLASS::OnDestroy();
}

LRESULT CFileTreeCtrl::OnChange(WPARAM /*wParam*/, LPARAM /*lParam*/)
{
  if (m_nAutoRefresh)
  {
    //Trace message which is helpful for diagnosing autorefresh
    TRACE(_T("CFileTreeCtrl::OnChange, Refreshing %s due to change\n"), m_sMonitoringPath.operator LPCTSTR()), 

    Refresh();
  }
  return 0L;
}

void CFileTreeCtrl::SetAutoRefresh(BOOL bAutoRefresh) 
{
  //Since it can be touched by more than one thead
  InterlockedExchange(&m_nAutoRefresh, bAutoRefresh); 

  Refresh(); //Force the monitoring threads to be recreated
}

void CFileTreeCtrl::CollapseExpandBranch(HTREEITEM hItem, int nAction)
{
  if (ItemHasChildren(hItem))
  {
    Expand(hItem, nAction);
    hItem = GetChildItem(hItem);
    while (hItem)
    {
      CollapseExpandBranch(hItem, nAction);
      hItem = GetNextSiblingItem(hItem);
    }
  }
}

void CFileTreeCtrl::CollapseAll() 
{
  HTREEITEM hItem = GetRootItem();        
  while (hItem)
  {
    CollapseExpandBranch(hItem, TVE_COLLAPSE);
    hItem = GetNextSiblingItem(hItem);
  }   
}

void CFileTreeCtrl::ExpandAll() 
{
  HTREEITEM hItem = GetRootItem();        
  while (hItem)
  {
    CollapseExpandBranch(hItem, TVE_EXPAND);
    hItem = GetNextSiblingItem(hItem);
  }
}

void CFileTreeCtrl::Clear()
{
  //Delete all the items
  DeleteAllItems();

  //Reset the member variables we have
  m_hMyComputerRoot = NULL;
  m_hNetworkRoot = NULL;
  m_hRootedFolder = NULL;
  m_hMyDocumentsRoot = NULL;
  m_hCurrentUserFolderRoot = NULL;
  m_hDesktop = NULL;
  m_hRoot = TVI_ROOT;
}

void CFileTreeCtrl::PreSubclassWindow() 
{
  //Let the base class do its thing
	FILETREECTRL_BASE_CLASS::PreSubclassWindow();

  //Get a pointer to IShellFolder and IMalloc
  ASSERT(m_pShellFolder == NULL);
  VERIFY(SUCCEEDED(SHGetDesktopFolder(&m_pShellFolder)));
  VERIFY(SUCCEEDED(SHGetMalloc(&m_pMalloc)));
  
  //Load up the cursors we need
#ifndef FILETREECTRL_NO_RESOURCES  
  CWinApp* pApp = AfxGetApp();
  AFXASSUME(pApp);
  m_NoDropCopyCursor = pApp->LoadCursor(IDR_TREEFILECTRL_NO_DROPCOPY);
  VERIFY(m_NoDropCopyCursor);
  m_DropCopyCursor = pApp->LoadCursor(IDR_TREEFILECTRL_DROPCOPY);
  VERIFY(m_DropCopyCursor);
  m_NoDropMoveCursor = pApp->LoadCursor(IDR_TREEFILECTRL_NO_DROPMOVE);
  VERIFY(m_NoDropMoveCursor);
  m_DropMoveCursor = pApp->LoadStandardCursor(IDC_ARROW);
  VERIFY(m_DropMoveCursor);
#endif  
}

BOOL CFileTreeCtrl::OnDeleteItem(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NMTREEVIEW* pNMTreeView = reinterpret_cast<NMTREEVIEW*>(pNMHDR);
	AFXASSUME(pNMTreeView);

  if (pNMTreeView->itemOld.hItem != TVI_ROOT)
  {
    CFileTreeCtrlItemInfo* pItem = reinterpret_cast<CFileTreeCtrlItemInfo*>(pNMTreeView->itemOld.lParam);
    if (pItem->m_pNetResource)
    {
      if (pItem->m_pNetResource->lpLocalName)
      {
        free(pItem->m_pNetResource->lpLocalName);
        pItem->m_pNetResource->lpLocalName = NULL;
      }
      if (pItem->m_pNetResource->lpRemoteName)
      {
        free(pItem->m_pNetResource->lpRemoteName);
        pItem->m_pNetResource->lpRemoteName = NULL;
      }
      if (pItem->m_pNetResource->lpComment)
      {
        free(pItem->m_pNetResource->lpComment);
        pItem->m_pNetResource->lpComment = NULL;
      }
      if (pItem->m_pNetResource->lpProvider)
      {
        free(pItem->m_pNetResource->lpProvider);
        pItem->m_pNetResource->lpProvider = NULL;
      }
      delete pItem->m_pNetResource;
      pItem->m_pNetResource = NULL;
    }
    delete pItem;
  }

	*pResult = 0;

  return FALSE; //Allow the message to be reflected again
}

void DDX_FileTreeValue(CDataExchange* pDX, CFileTreeCtrl& ctrlFileTree, CString& sItem)
{
  if (pDX->m_bSaveAndValidate)
    sItem = ctrlFileTree.GetSelectedPath();
  else
    ctrlFileTree.SetSelectedPath(sItem);
}
