; Inno Setup template, derived from the PrettyReMark installer script.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!
; Non-commercial use only.
;
; To use: search for "TODO" and fill in each item.

#define MyAppName "wbigcalc"
#ifndef MyAppVersion
#define MyAppVersion "1.00"
#endif
#define MyAppPublisher "Derell Licht"
#define MyAppURL "https://derelllicht.42web.io/wbigcalc.html"
#define MyAppExeName "wbigcalc.exe"
#define DoubleAmp(Value) StringChange(Value, "&", "&&")
#define EscapeConstArgument(Value) StringChange(StringChange(StringChange(Value, "%", "%25"), ",", "%2c"), "}", "%7d")
#define RepoRoot "D:\SourceCode\Git\wbigcalc"

; Uncomment to offer a file association (task, registry entries, shell notify).
; Also set AssocExt / AssocProgId / AssocDesc below.
;#define UseFileAssoc
#define AssocExt "TODO"
#define AssocProgId MyAppName + "." + AssocExt
#define AssocDesc MyAppName + " Document"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
;       Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
; python -c "import uuid; print('{{' + str(uuid.uuid4()).upper() + '}')"
AppId={{9D5652ED-924D-4908-A10D-0D29CDABB246}
AppName={#MyAppName}
AppVersion={#MyAppVersion}
;AppVerName={cm:NameAndVersion,{#EscapeConstArgument(MyAppName)},{#EscapeConstArgument(MyAppVersion)}}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
; {autopf} resolves to the per-user Program Files equivalent
; (%LocalAppData%\Programs) since PrivilegesRequired=lowest below means this
; never runs elevated -- no hardcoded drive letter, no UAC prompt, and it
; matches the per-user install PrivilegesRequired=lowest already declares.
; (If PrivilegesRequired is ever changed to "admin", {autopf} would instead
; resolve to the real Program Files under HKLM, with no change needed here.)
DefaultDirName={autopf}\{#MyAppName}
; Explicit rather than relying on the "auto" default, so the destination
; picker always shows for an interactive install regardless of how Inno's
; heuristic reads DefaultDirName. Has no effect on /SILENT or /VERYSILENT --
; Inno skips every wizard page unattended either way, so this stays
; compliant with winget's "no interaction required" policy.
DisableDirPage=no
UninstallDisplayIcon={app}\{#MyAppExeName}
; "ArchitecturesAllowed=x64compatible" specifies that Setup cannot run on anything but x64 and Windows 11 on Arm.
; remove both Architectures lines if this app also needs to run on 32-bit Windows.
ArchitecturesAllowed=x64compatible
; "ArchitecturesInstallIn64BitMode=x64compatible" requests that the install be done in "64-bit mode" on x64 or Windows 11 on Arm.
; This means it should use the native 64-bit Program Files directory and the 64-bit view of the registry.
ArchitecturesInstallIn64BitMode=x64compatible
; Uncomment the following line to use a 64-bit installer.
;SetupArchitecture=x64
DefaultGroupName={#MyAppName}
; point at the real license file (or comment out to skip the license page).
LicenseFile={#RepoRoot}\LICENSE.txt
; Uncomment the following line to run in non administrative install mode (install for current user only).
PrivilegesRequired=lowest
OutputBaseFilename={#MyAppName}V{#MyAppVersion}.setup
SolidCompression=yes
WizardStyle=modern slate
#ifdef UseFileAssoc
; Lets the uninstaller warn about / clean up the file association added below.
ChangesAssociations=yes
#endif

[Languages]
Name: "english"; MessagesFile: "compiler:Default.isl"

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; GroupDescription: "{cm:AdditionalIcons}"; Flags: unchecked
#ifdef UseFileAssoc
Name: "associateext"; Description: "Associate .{#AssocExt} files with {#MyAppName}"; GroupDescription: "File associations:"
#endif

[Files]
; list the files to install. Paths below are examples only.
Source: "{#RepoRoot}\{#MyAppExeName}"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\{#MyAppName}.chm"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\{#MyAppName}.ini"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\README.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\CHANGELOG.md"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\LICENSE.txt"; DestDir: "{app}"; Flags: ignoreversion
Source: "{#RepoRoot}\bigcalc.txt"; DestDir: "{app}"; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files.

[Icons]
Name: "{group}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"
Name: "{group}\{cm:UninstallProgram,{#MyAppName}}"; Filename: "{uninstallexe}"
Name: "{autodesktop}\{#MyAppName}"; Filename: "{app}\{#MyAppExeName}"; Tasks: desktopicon
; add Start Menu entries for the other installed files (match the [Files] list above).
Name: "{group}\{#MyAppName} Help"; Filename: "{app}\{#MyAppName}.chm"
Name: "{group}\Readme"; Filename: "{app}\README.md"
Name: "{group}\ChangeLog"; Filename: "{app}\CHANGELOG.md"
Name: "{group}\License"; Filename: "{app}\LICENSE.txt"

[Run]
; This runs the INSTALLED app (post-install "Launch program now" checkbox) --
; it has nothing to do with the installer's own filename, so it must reference
; {#MyAppExeName} (the #define'd app exe), not OutputBaseFilename.
Filename: "{app}\{#MyAppExeName}"; Description: "{cm:LaunchProgram,{#DoubleAmp(MyAppName)}}"; Flags: nowait postinstall skipifsilent

#ifdef UseFileAssoc
[Registry]
; HKA auto-resolves to HKCU\Software\Classes (matches PrivilegesRequired=lowest,
; a per-user install) or HKLM\Software\Classes for an admin install -- no need to
; hardcode which.
Root: HKA; Subkey: "Software\Classes\.{#AssocExt}"; ValueType: string; ValueName: ""; ValueData: "{#AssocProgId}"; Flags: uninsdeletevalue; Tasks: associateext
Root: HKA; Subkey: "Software\Classes\{#AssocProgId}"; ValueType: string; ValueName: ""; ValueData: "{#AssocDesc}"; Flags: uninsdeletekey; Tasks: associateext
Root: HKA; Subkey: "Software\Classes\{#AssocProgId}\DefaultIcon"; ValueType: string; ValueName: ""; ValueData: "{app}\{#MyAppExeName},0"; Tasks: associateext
Root: HKA; Subkey: "Software\Classes\{#AssocProgId}\shell\open\command"; ValueType: string; ValueName: ""; ValueData: """{app}\{#MyAppExeName}"" ""%1"""; Tasks: associateext

[Code]
// SHChangeNotify tells Explorer to re-read file associations/icons immediately,
// instead of leaving the icon/association stale until next logon.
procedure SHChangeNotify(wEventId: Integer; uFlags: Integer; dwItem1: Integer; dwItem2: Integer);
  external 'SHChangeNotify@shell32.dll stdcall';

procedure CurStepChanged(CurStep: TSetupStep);
begin
  if CurStep = ssPostInstall then begin
    SHChangeNotify($8000000, $1000, 0, 0); // SHCNE_ASSOCCHANGED, SHCNF_IDLIST
  end;
end;
#endif
