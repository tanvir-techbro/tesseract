; tesseract Windows installer (Inno Setup 6).
; Build on Windows: iscc installer.iss  (output: dist/tesseract-setup.exe)
; Needs dist-win/win64/ staged (run ./build_win64.sh on Linux,
; or copy an equivalent folder on Windows).

#define AppVersion "0.1.0"

[Setup]
AppName=tesseract
AppVersion={#AppVersion}
AppPublisher=tanvir-techbro
DefaultDirName={autopf}\tesseract
DefaultGroupName=tesseract
OutputBaseFilename=tesseract-setup
Compression=lzma2
SolidCompression=yes
PrivilegesRequired=lowest
ArchitecturesAllowed=x64compatible
ArchitecturesInstallIn64BitMode=x64compatible

[Files]
Source: "dist-win\win64\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\tesseract"; Filename: "{app}\tesseract.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\tesseract"; Filename: "{app}\tesseract.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Desktop shortcut"; Flags: unchecked

[Run]
Filename: "{app}\tesseract.exe"; Description: "Launch tesseract"; Flags: nowait postinstall skipifsilent
