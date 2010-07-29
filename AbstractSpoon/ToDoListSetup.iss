;; Inno Setup script for the ToDoList and its plug-ins.
;; Written by AbstractSpoon aka SchweinDeBurg (elijah.zarezky@gmail.com).

[Setup]
AppName=ToDoList
AppVerName=ToDoList 6.0.8
AppID={{E0FD8591-CBB1-475F-A4EA-EF4E3A58F78F}
AppPublisher=AbstractSpoon Software
AppPublisherURL=http://www.abstractspoon.com/
AppSupportURL=http://www.abstractspoon.com/tdl_resources.html
AppUpdatesURL=http://www.abstractspoon.com/tdl_resources.html
AppVersion=6.0.8
AppCopyright=Copyright © 2003–2009 by AbstractSpoon Software
DefaultDirName={pf}\AbstractSpoon\ToDoList
DefaultGroupName=AbstractSpoon\ToDoList
AllowNoIcons=true
Compression=lzma
SolidCompression=true
OutputDir=.\SETUP
OutputBaseFilename=ToDoList-6.0.8-setup
VersionInfoVersion=6.0.8.0
VersionInfoProductName=ToDoList
VersionInfoProductVersion=6.0
MinVersion=0,5.0.2195
WizardImageFile=compiler:WizModernImage-IS.bmp
WizardSmallImageFile=compiler:WizModernSmallImage-IS.bmp
LicenseFile=EclipsePublicLicense.rtf

[LangOptions]
DialogFontName=MS Shell Dlg 2
DialogFontSize=8

[Code]
//  Code Page default values
const
	CP_ACP = 0;
	CP_OEMCP = 1;
	CP_MACCP = 2;
	CP_THREAD_ACP = 3;
	CP_SYMBOL = 42;
	CP_UTF7 = 65000;
	CP_UTF8 = 65001;

// MBCS and Unicode translation flags
const
	MB_PRECOMPOSED = $0001;
	MB_COMPOSITE = $0002;
	MB_USEGLYPHCHARS = $0004;
	MB_ERR_INVALID_CHARS = $0008;

	WC_COMPOSITECHECK = $0200;
	WC_DISCARDNS = $0010;
	WC_SEPCHARS = $0020;
	WC_DEFAULTCHAR = $0040;
	WC_ERR_INVALID_CHARS = $0080;
	WC_NO_BEST_FIT_CHARS = $0400;

	DefFlagsMB = MB_PRECOMPOSED;
	DefFlagsWC = WC_COMPOSITECHECK or WC_DISCARDNS or WC_SEPCHARS or WC_DEFAULTCHAR;

function MultiByteToWideChar(CodePage, Flags: Integer; SrcStr: AnsiString; SrcLen: Integer; DestStr: String; DestLen: Integer): Integer;
external 'MultiByteToWideChar@kernel32.dll stdcall';
function WideCharToMultiByte(CodePage, Flags: Integer; SrcStr: String; SrcLen: Integer; DestStr: AnsiString; DestLen: Integer; DefChar: Integer; DefCharUsed: Integer): Integer;
external 'WideCharToMultiByte@kernel32.dll stdcall';

function AnsiStringToString(const SrcStr: AnsiString; CodePage: Integer): String;
var
	WideLen: Integer;
begin
	if (SrcStr <> '') then
	begin
		WideLen := MultiByteToWideChar(CodePage, DefFlagsMB, SrcStr, -1, Result, 0);
		SetLength(Result, WideLen - 1);
		MultiByteToWideChar(CodePage, DefFlagsMB, SrcStr, -1, Result, WideLen - 1);
	end
	else begin
		Result := '';
	end;
end;

function StringToAnsiString(const SrcStr: String; CodePage: Integer): AnsiString;
var
	AnsiLen: Integer;
begin
	if (SrcStr <> '') then
	begin
		AnsiLen := WideCharToMultiByte(CodePage, DefFlagsWC, SrcStr, -1, Result, 0, 0, 0);
		SetLength(Result, AnsiLen - 1);
		WideCharToMultiByte(CodePage, DefFlagsWC, SrcStr, -1, Result, AnsiLen - 1, 0, 0);
	end
	else begin
		Result := '';
	end;
end;

function IsWinSxS(): Boolean;
var
	osVersion: TWindowsVersion;
begin
	GetWindowsVersionEx(osVersion);
	Result := ((osVersion.Major = 5) and (osVersion.Minor >= 1)) or (osVersion.Major > 5);
end;

const
	MagicStr = '{4ADE7CBA-27B5-4E33-987D-802509D2D34A}';

procedure AdjustAfxConfig();
var
	AfxConfigPath: String;
	AfxConfigXML: AnsiString;
	WideXML: String;
begin
	if (IsWinSxS()) then
	begin
		AfxConfigPath := ExpandConstant(CurrentFileName());
		if (LoadStringFromFile(AfxConfigPath, AfxConfigXML)) then
		begin
			WideXML := AnsiStringToString(AfxConfigXML, CP_ACP);
			StringChangeEx(WideXML, MagicStr, ExpandConstant('{app}'), True);
			SaveStringToFile(AfxConfigPath, StringToAnsiString(WideXML, CP_ACP), False);
		end;
	end;
end;

[Files]
;; core application files
Source: ".\ToDoList\Output.2008\x86\Release\Unicode\ToDoList.exe"; DestDir: "{app}"
Source: ".\CalendarExt\Output.2008\x86\Release\Unicode\CalendarExt.dll"; DestDir: "{app}"
Source: ".\EncryptDecrypt\Output.2008\x86\Release\Unicode\EncryptDecrypt.dll"; DestDir: "{app}"
Source: ".\FMindImportExport\Output.2008\x86\Release\Unicode\FMindImportExport.dll"; DestDir: "{app}"
Source: ".\GPExport\Output.2008\x86\Release\Unicode\GPExport.dll"; DestDir: "{app}"
Source: ".\iCalImportExport\Output.2008\x86\Release\Unicode\iCalImportExport.dll"; DestDir: "{app}"
Source: ".\MLOImport\Output.2008\x86\Release\Unicode\MLOImport.dll"; DestDir: "{app}"
Source: ".\MySpellCheck\Output.2008\x86\Release\Unicode\MySpellCheck.dll"; DestDir: "{app}"
Source: ".\OutlookImpExp\Output.2008\x86\Release\Unicode\OutlookImpExp.dll"; DestDir: "{app}"
Source: ".\PlainTextImport\Output.2008\x86\Release\Unicode\PlainTextImport.dll"; DestDir: "{app}"
Source: ".\RTFContentCtrl\Output.2008\x86\Release\Unicode\RTFContentCtrl.dll"; DestDir: "{app}"
Source: ".\EclipsePublicLicense.rtf"; DestDir: "{app}"; Flags: ignoreversion

;; CRT redistributables
Source: ".\REDIST\Microsoft.VC90.CRT\msvcr90.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.CRT\msvcp90.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.CRT\msvcm90.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.CRT\Microsoft.VC90.PrivateCRT.manifest"; DestDir: "{app}"; MinVersion: 0,5.01.2600
Source: ".\REDIST\Microsoft.VC90.CRT\Microsoft.VC90.CRT.manifest"; DestDir: "{app}"; MinVersion: 0,5.01.2600

;; MFC library redistributables
Source: ".\REDIST\Microsoft.VC90.MFC\mfc90u.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.MFC\mfc90u.dll.1000.config"; DestDir: "{app}"; MinVersion: 0,5.01.2600; AfterInstall: AdjustAfxConfig
Source: ".\REDIST\Microsoft.VC90.MFC\mfcm90u.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.MFC\Microsoft.VC90.PrivateMFC.manifest"; DestDir: "{app}"; MinVersion: 0,5.01.2600

;; MFC library localizations
Source: ".\REDIST\Microsoft.VC90.MFCLOC\mfc90enu.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.MFCLOC\mfc90rus.dll"; DestDir: "{app}"
Source: ".\REDIST\Microsoft.VC90.MFCLOC\Microsoft.VC90.MFCLOC.manifest"; DestDir: "{app}"; MinVersion: 0,5.01.2600

[Icons]
Name: "{group}\ToDoList"; Filename: "{app}\ToDoList.exe"
Name: "{group}\License Agreement"; Filename: "{app}\EclipsePublicLicense.rtf"
Name: "{group}\ToDoList on the Web"; Filename: "{app}\ToDoList.url"
Name: "{group}\Uninstall ToDoList"; Filename: "{uninstallexe}"
Name: "{userdesktop}\ToDoList"; Filename: "{app}\ToDoList.exe"; Tasks: desktopicon
Name: "{userappdata}\Microsoft\Internet Explorer\Quick Launch\ToDoList"; Filename: "{app}\ToDoList.exe"; Tasks: quicklaunchicon

[INI]
Filename: "{app}\ToDoList.url"; Section: "InternetShortcut"; Key: "URL"; String: "http://zarezky.spb.ru/projects/afx_scratch.html"

[Tasks]
Name: desktopicon; Description: "Create a &desktop icon"; GroupDescription: "Additional icons:"; Flags: unchecked
Name: quicklaunchicon; Description: "Create a &Quick Launch icon"; GroupDescription: "Additional icons:"; Flags: unchecked

[Run]
Filename: "{app}\ToDoList.exe"; Description: "Launch ToDoList"; Flags: nowait postinstall skipifsilent

[UninstallDelete]
Type: files; Name: "{app}\ToDoList.url"

;; end of file
