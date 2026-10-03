; tesseract Windows installer (Inno Setup 6).
; Build on Windows: iscc installer.iss  (output: dist/tesseract-setup.exe)
; dist-win/ must hold tesseract.exe + DLLs + Assets (see README packaging).

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
Source: "dist-win\*"; DestDir: "{app}"; Flags: ignoreversion recursesubdirs

[Icons]
Name: "{group}\tesseract"; Filename: "{app}\tesseract.exe"; WorkingDir: "{app}"
Name: "{autodesktop}\tesseract"; Filename: "{app}\tesseract.exe"; WorkingDir: "{app}"; Tasks: desktopicon

[Tasks]
Name: desktopicon; Description: "Desktop shortcut"; Flags: unchecked

[Run]
Filename: "{app}\tesseract.exe"; Description: "Launch tesseract"; Flags: nowait postinstall skipifsilent
