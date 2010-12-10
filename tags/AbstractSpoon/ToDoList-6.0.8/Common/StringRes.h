// Copyright (C) 2003-2005 AbstractSpoon Software.
//
// This license applies to everything in the ToDoList package, except where
// otherwise noted.
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the
// use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it
// freely, subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented; you must not
// claim that you wrote the original software. If you use this software in a
// product, an acknowledgment in the product documentation would be appreciated
// but is not required.
//
// 2. Altered source versions must be plainly marked as such, and must not be
// misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.

//*****************************************************************************
// Modified by Elijah Zarezky aka SchweinDeBurg (elijah.zarezky@gmail.com):
// - improved compatibility with the Unicode-based builds
// - added AbstractSpoon Software copyright notice and licenese information
// - adjusted #include's paths
//*****************************************************************************

#if !defined(AFX_STRING_RESOURCES_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_)
#define AFX_STRING_RESOURCES_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// StringRes.h : header file
//

const LPCTSTR BTN_OK = _T("OK");
const LPCTSTR BTN_CANCEL = _T("Cancel");
const LPCTSTR BTN_CLOSE = _T("Close");

const LPCTSTR ABOUT_TITLE = _T("About");

const LPCTSTR MISC_ELLIPSIS = _T("...");

const LPCTSTR FILTER_ALLFILES = _T("All Files (*.*)|*.*||");

const LPCTSTR SPELLCHECK_ACTIVEDICT = _T("Ac&tive Dictionary:");
const LPCTSTR SPELLCHECK_BROWSE = _T("Bro&wse");
const LPCTSTR SPELLCHECK_BROWSE_TITLE = _T("Select Dictionary");
const LPCTSTR SPELLCHECK_DOWNLOADMORE = _T("Download More Dictionaries");
const LPCTSTR SPELLCHECK_CHECKING = _T("C&hecking Text:");
const LPCTSTR SPELLCHECK_RESTART = _T("R&estart");
const LPCTSTR SPELLCHECK_BTN_REPLACE = _T("&Replace");
const LPCTSTR SPELLCHECK_WITH = _T("&With:");
const LPCTSTR SPELLCHECK_REPLACE = _T("Replace:");
const LPCTSTR SPELLCHECK_NEXTWORD = _T("&Next Word");
const LPCTSTR SPELLCHECK_SETUP_MSG = _T("Before you can spell check for the first time you need \nto specify the location of the Spell Checker.");
const LPCTSTR SPELLCHECK_TITLE = _T("Spell Checker");
const LPCTSTR SPELLCHECK_DICT_FILTER = _T("Dictionaries (*.dic)|*.dic||");
const LPCTSTR SPELLCHECK_ENGINE_FILTER = _T("SpellChecker Engines|*.dll||");
const LPCTSTR SPELLCHECK_ENGINE_TITLE = _T("Locate Spell Check Engine");

const LPCTSTR TIME_YEARS = _T("Y(ears)");
const LPCTSTR TIME_MONTHS = _T("M(onths)");
const LPCTSTR TIME_WEEKS = _T("W(eeks)");
const LPCTSTR TIME_DAYS = _T("D(ays)");
const LPCTSTR TIME_HOURS = _T("H(ours)");
const LPCTSTR TIME_MINS = _T("m(inutes)");
const char	  TIME_YEAR_ABBREV = 'Y';
const char    TIME_MONTH_ABBREV = 'M';
const char    TIME_WEEK_ABBREV = 'W';
const char    TIME_DAY_ABBREV = 'D';
const char    TIME_HOUR_ABBREV = 'H';
const char    TIME_MIN_ABBREV = 'm';
const LPCTSTR TIME_UNITS = _T("Time Units");

const LPCTSTR FILEEDIT_ASSOCAPPFAILURE = _T("The application associated with '%s'\ncould not be launched.\n\nPlease check the file associations in Explorer before trying again.");
const LPCTSTR FILEEDIT_FILEOPENFAILED = _T("'%s' could not be opened (%d).\n\nPlease check that the file exists and that you have the appropriate access rights before trying again.");
const LPCTSTR FILEEDIT_FILENOTFOUND = _T("'%s' could not be found.");
const LPCTSTR FILEEDIT_BROWSE = _T("Browse");
const LPCTSTR FILEEDIT_BROWSE_TITLE = _T("Select File");
const LPCTSTR FILEEDIT_SELECTFOLDER = _T("Select Folder");
const LPCTSTR FILEEDIT_VIEW = _T("View");
const LPCTSTR FILEEDIT_VIEWFOLDER = _T("View Folder");

const LPCTSTR PASSWORD_ENTERPWD = _T("Enter Password:");
const LPCTSTR PASSWORD_CONFIRMPWD = _T("Confirm Password:");
const LPCTSTR PASSWORD_TITLE = _T("Password Required");
const LPCTSTR PASSWORD_CONFIRMFAILED = _T("Unfortunately the confirmation did not match the password.\n\nPlease try again.");
const LPCTSTR PASSWORD_CONFIRMFAILED_TITLE = _T("Password Confirmation Incorrect");

const LPCTSTR ENCRYPT_NOTINSTALLED = _T("The file '%s' is encrypted, but you do not have the necessary components installed to decrypt it.");
const LPCTSTR ENCRYPT_ENTERPWD = _T("The file '%s' is encrypted.\n\nPlease enter your password below.");
const LPCTSTR ENCRYPT_DECRYPTFAILED = _T("The file '%s' could not be decrypted using the supplied password.\n\nWound you like to try again?");

// const LPCTSTR  = _T("");
// const LPCTSTR  = _T("");
// const LPCTSTR  = _T("");
// const LPCTSTR  = _T("");
// const LPCTSTR  = _T("");


#endif // AFX_SHARED_STRING_RESOURCES_H__5951FDE6_508A_4A9D_A55D_D16EB026AEF7__INCLUDED_
