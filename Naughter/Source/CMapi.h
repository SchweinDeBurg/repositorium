/*
Module : CMAPI.H
Purpose: Defines the interface for an MFC wrapper class for sending an email using simple MAPI
Created: PJN / 11-05-1999

Copyright (c) 1999 - 2011 by PJ Naughter (Web: www.naughter.com, Email: pjna@naughter.com)

All rights reserved.

Copyright / Usage Details:

You are allowed to include the source code in any product (commercial, shareware, freeware or otherwise) 
when your product is released in binary form. You are allowed to modify the source code in any way you want 
except you cannot modify the copyright details at the top of each module. If you want to distribute source 
code with your application, then you are only allowed to distribute versions released by the author. This is 
to maintain a single distribution point for the source code. 

*/


////////////////////////// Macros / Defines ////////////////////////////////////////////

#pragma once

#ifndef __CMAPI_H__
#define __CMAPI_H__


#ifndef MAPI_H
#include <MAPI.h>
#pragma message("To avoid this message, please put mapi.h in your pre compiled header (normally stdafx.h)")
#endif

#ifndef __AFXTEMPL_H__
#pragma message("To avoid this message, please put afxtempl.h in your pre compiled header (normally stdafx.h)")
#include <afxtempl.h>
#endif

#ifndef CMAPI_EXT_CLASS
#define CMAPI_EXT_CLASS
#endif


///////////////////////// Classes /////////////////////////////////////////////

//Class which encapsulates a MAPI recipient
class CMAPI_EXT_CLASS CMapiRecipient
{
public:
//Constructors / Destructors
  CMapiRecipient();
  CMapiRecipient(const CString& sAddress);
  CMapiRecipient(const CString& sFriendlyName, const CString& sAddress);

//Methods
  void Initialise();
  CMapiRecipient& operator=(const CMapiRecipient& recipient); 

//Member variables
  CString m_sAddress;      //The actual provider specific email address. For example if using the SMTP 
                           //transport provider such as in Outlook express, then this value would be
                           //"SMTP:pjna@naughter.com"
  CString m_sFriendlyName; //The friendy name of the recipient e.g. "PJ Naughter"
  
protected:
//Member variables
  CStringA m_sAsciiAddress;      //Value used to store ASCII value used by MAPI functions internally
  CStringA m_sAsciiFriendlyName; //Value used to store ASCII value used by MAPI functions internally
  
  friend class CMapiSession;  
};

//Class which encapsulates a MAPI mail message
class CMAPI_EXT_CLASS CMapiMessage
{
public:
//Constructors / Destructors
  CMapiMessage();

//Enums
	enum RECIPIENT_TYPE 
	{ 
	  TO, 
	  CC, 
	  BCC 
	};

//Methods
  void Initialise();
  static INT_PTR ParseMultipleRecipients(const CString& sRecipients, CArray<CMapiRecipient, CMapiRecipient&>& recipients);
  int AddMultipleAttachments(const CString& sAttachments);
  CMapiMessage& operator=(const CMapiMessage& message);

//Member variables
  CArray<CMapiRecipient, CMapiRecipient&> m_To;               //The To: Recipients
  CArray<CMapiRecipient, CMapiRecipient&> m_CC;               //The CC: Recipients
  CArray<CMapiRecipient, CMapiRecipient&> m_BCC;              //The BCC Recipients
  CString                                 m_sSubject;         //The Subject of the message
  CString                                 m_sBody;            //The Body of the message
  CStringArray                            m_Attachments;      //Files to attach to the email
  CStringArray                            m_AttachmentTitles; //Titles to use for the email file attachments
  CMapiRecipient									        m_From;					    //The sender's
  CString											            m_sDateReceived;		//Date recieved of the message
  CString                                 m_sMessageType;     //The MAPI message type
  FLAGS                                   m_Flags;            //The flags for this message (Corresponds to MapiMessage::flFlags)
 
protected:
//Member variables
  CArray<CStringA, CStringA&> m_AsciiAttachments;      //Value used to store ASCII value used by MAPI functions internally
  CArray<CStringA, CStringA&> m_AsciiAttachmentTitles; //Value used to store ASCII value used by MAPI functions internally

  friend class CMapiSession;  
};

//The class which encapsulates the MAPI connection
class CMAPI_EXT_CLASS CMapiSession
{
public:
//Constructors / Destructors
  CMapiSession();
  virtual ~CMapiSession();

//Logon / Logoff Methods
  BOOL Logon(const CString& sProfileName, const CString& sPassword = _T(""), CWnd* pParentWnd = NULL, BOOL bForceDownload = FALSE);
  BOOL SharedLogon(BOOL bForceDownload = FALSE);
  BOOL LoggedOn() const;
  BOOL Logoff();

//Send a message
  BOOL Send(CMapiMessage& message, BOOL bResolve = FALSE, BOOL bInteractive = FALSE, CWnd* pParentWnd = NULL, BOOL bSendAsHTML = FALSE);

//Read support
  BOOL Find(CString& sMessageID, const CString& sSeedMessageID = _T(""), FLAGS flFlags = MAPI_LONG_MSGID, const CString& sMessageType = _T(""), BOOL bInteractive = FALSE, CWnd* pParentWnd = NULL);
  BOOL Read(const CString& sMessageID, CMapiMessage& message, FLAGS flFlags = MAPI_SUPPRESS_ATTACH, BOOL bInteractive = FALSE, CWnd* pParentWnd = NULL);

//Delete support
	BOOL Delete(const CString& sMessageID, BOOL bInteractive = FALSE, CWnd* pParentWnd = NULL);

//General MAPI support
  BOOL MapiInstalled() const;

//Error Handling
  ULONG GetLastError() const;

protected:
//Methods
  void Initialise();
  void Deinitialise(); 
  BOOL Resolve(const CString& sName, lpMapiRecipDesc* lppRecip);
  virtual BOOL CreateTempPathForHTML(CString& sUniquePath);
  virtual void SetupRecipients(MapiMessage& mapiMessage, CMapiMessage& message, BOOL bResolve);
  virtual void ReleaseRecipients(CMapiMessage& message);
  virtual void SetupAttachment(MapiMessage& mapiMessage, CMapiMessage& message, INT_PTR nIndex);
  virtual void SetupAttachments(MapiMessage& mapiMessage, CMapiMessage& message, BOOL bSendAsHTML, const CString& sTempHTMLFile);
  virtual void ReleaseAttachments(CMapiMessage& message);

//Data
  LHANDLE           m_hSession;            //Mapi Session handle
  ULONG             m_nLastError;          //Last Mapi error value
  HINSTANCE         m_hMapi;               //Instance handle of the MAPI dll
  LPMAPILOGON       m_lpfnMAPILogon;       //MAPILogon function pointer
  LPMAPILOGOFF      m_lpfnMAPILogoff;      //MAPILogoff function pointer
  LPMAPISENDMAIL    m_lpfnMAPISendMail;    //MAPISendMail function pointer
  LPMAPIRESOLVENAME m_lpfnMAPIResolveName; //MAPIResolveName function pointer
  LPMAPIFREEBUFFER  m_lpfnMAPIFreeBuffer;  //MAPIFreeBuffer function pointer
	LPMAPIREADMAIL    m_lpfnMAPIReadMail;    //MAPIReadMail function pointer
	LPMAPIFINDNEXT    m_lpfnMAPIFindNext;    //MAPIFindNext function pointer
  LPMAPIDELETEMAIL  m_lpfnMAPIDeleteMail;  //MAPIDeleteMail function pointer
};

#endif //__CMAPI_H__
