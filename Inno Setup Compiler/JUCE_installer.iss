[Setup]
AppName=BosKobol
AppVersion=1.0.0
DefaultGroupName=BoscoOlives
OutputBaseFilename=BoscoKobol-windows
DefaultDirName={commoncf64}\VST3

[Code]
var
  InstallDirPage: TInputDirWizardPage;
  DefaultInstallDir: string;

procedure InitializeWizard;
begin
  MsgBox('This plugin has been developed as a final project of the TFG for the UPF, by Bosco Olives. This is a sample-based recreation of RSFs Kobol oscillator.', mbInformation, MB_OK);
  DefaultInstallDir := ExpandConstant('{commoncf64}\VST3');
  if not DirExists(DefaultInstallDir) then
    CreateDir(DefaultInstallDir);

  InstallDirPage := CreateInputDirPage(wpSelectDir, 'Select Installation Directory', 'Select the directory where you want to install the application.', 'Installation will be done in this directory:', False, DefaultInstallDir);
  InstallDirPage.Add('');
  InstallDirPage.Values[0] := DefaultInstallDir;
end;

function GetDir(Param: string): string;
begin
  Result := InstallDirPage.Values[0];
end;

[Dirs]
Name: "{commoncf64}\VST3"

[Files]
Source: "C:\Users\bosco\Desktop\TFG\TFG-Kobol-BoscoOlives\JUCE_Projects\Plugin_Kobol_VCO_All_Samples\Kobol_VCO\*"; DestDir: "{code:GetDir|InstallDir}"; Flags: recursesubdirs createallsubdirs
