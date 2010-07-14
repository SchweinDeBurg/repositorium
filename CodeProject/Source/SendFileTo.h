#ifndef __SENDFILETO_H__
#define __SENDFILETO_H__

#include <mapi.h>

#ifndef ULONG_PTR
#define ULONG_PTR ULONG 
#endif

class CSendFileTo
{
public:
	static bool SendMail(HWND hWndParent, CString const &strSubject=_T(""), CString const &strBody=_T(""), CString const &strAttachmentFileName=_T(""))
	{
		if (!hWndParent || !::IsWindow(hWndParent))
			return false;

		HINSTANCE hMAPI = ::LoadLibraryA(_T("MAPI32.DLL"));
		if (!hMAPI)
			return false;

		typedef ULONG (PASCAL *PFNSENDMAIL)(ULONG, ULONG_PTR, MapiMessage*, FLAGS, ULONG);
		PFNSENDMAIL fnSendMail = (PFNSENDMAIL)GetProcAddress(hMAPI, "MAPISendMail");

		if (!fnSendMail)
			return false;

		MapiFileDesc fileDesc = { 0 };
		fileDesc.nPosition = (ULONG)-1;
		fileDesc.lpszPathName = const_cast<LPTSTR>(static_cast<LPCTSTR>(strAttachmentFileName));
		fileDesc.lpszFileName = const_cast<LPTSTR>(static_cast<LPCTSTR>(strAttachmentFileName));

		MapiMessage message = { 0 };
		message.lpszSubject = const_cast<LPTSTR>(static_cast<LPCTSTR>(strSubject));
		message.lpszNoteText = const_cast<LPTSTR>(static_cast<LPCTSTR>(strBody));
		message.nFileCount = strAttachmentFileName.IsEmpty() ? 0 : 1;
		message.lpFiles = strAttachmentFileName.IsEmpty() ? NULL : &fileDesc;

		int nError = fnSendMail(0, (ULONG_PTR)hWndParent, &message, MAPI_LOGON_UI|MAPI_DIALOG, 0);

		if (nError != SUCCESS_SUCCESS && nError != MAPI_USER_ABORT && nError != MAPI_E_LOGIN_FAILURE)
			return false;

		return true;
	}
};

#endif
