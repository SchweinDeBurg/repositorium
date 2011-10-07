/* ==========================================================================
    CTextFile

    Author :        Johan Rosengren, Abstrakt Mekanik AB

    Date :          2004-03-22

    Purpose :       The class is a helper-package for text files and
                    windows. It allows loading and saving text files in a
                    single operation, as well as getting text to and
                    from edit- and listboxes. If an empty filename is given
                    as a parameter to a call, the standard file dialog will
                    be displayed, to let the user select a file.
                    Error handling is managed internally, and the different
                    API-functions return a BOOL to signal success or
                    failure. In case of failure, FALSE returned, the member
                    function GetErrorMessage can be called to retrieve a
                    CString with the error message.
                    If this string is empty, the file selection was aborted
                    in the case of an empty input name.
   ========================================================================
                    14/4 2005   Added Dave Pritchards class CStdioFileEx
                                for MBCS/UNICODE-support.
   ========================================================================*/

#ifndef _TEXTFILE_H_
#define _TEXTFILE_H_

class CTextFile
{

public:

	// ctor( s )
	CTextFile(const CString& ext = _T(""), const CString& eol = _T("\r\n"));

	// dtor
	virtual ~CTextFile();

	// File operations
	BOOL    ReadTextFile(CString& filename, CStringArray& contents);
	BOOL    ReadTextFile(CString& filename, CString& contents);

	BOOL    WriteTextFile(CString& filename, const CStringArray& contents);
	BOOL    WriteTextFile(CString& filename, const CString& contents);

	BOOL    AppendFile(CString& filename, const CString& contents);
	BOOL    AppendFile(CString& filename, const CStringArray& contents);

	// Window operations
	BOOL    Load(CString& filename, CEdit* edit);
	BOOL    Load(CString& filename, CListBox* list);
	BOOL    Save(CString& filename, CEdit* edit);
	BOOL    Save(CString& filename, CListBox* list);

	// Error handling
	CString GetErrorMessage();

protected:

	virtual BOOL GetFilename(BOOL save, CString& filename);
	CString GetExtension();

private:

	CString m_error;
	CString m_extension;
	CString m_eol;

	void    ClearError();
	BOOL    ValidParam(CWnd* wnd);

};

#endif // _TEXTFILE_H_
