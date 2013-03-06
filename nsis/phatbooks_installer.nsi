# Put whatever files we want to install to the user's system,
# into same folder as this script. Then compile the script
# using the NSIS compiler, makensisw.


# This script is adapted from one posted at stackoverflow.com/questions/10695674/simple-nsis-recipe-for-basic-installation


!define Name "Phatbooks"
Name "${Name}"
Outfile "${Name} setup.exe"
RequestExecutionLevel admin ;Require admin rights on NT6+ (when UAC is turned on)
InstallDir "$PROGRAMFILES64\${Name}"

!include LogicLib.nsh
!include MUI.nsh

Function .onInit
	SetShellVarContext all
	UserInfo::GetAccountType
	pop $0
	${If} $0 != "admin" ;Require admin rights on NT4+
		MessageBox mb_iconstop "Administrator rights required!"
		SetErrorLevel 740 ;ERROR_ELEVATION_REQUIRED
		Quit
	${EndIf}
FunctionEnd

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_LANGUAGE "English"

Section
	SetOutPath "$INSTDIR"
	WriteUninstaller "$INSTDIR\Uninstall.exe"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Phatbooks By Matthew Harvey"  "DisplayName" "${Name}"
	WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Phatbooks By Matthew Harvey"  "UninstallString" "$INSTDIR\Uninstall.exe"
	File "phatbooks.exe"
	CreateShortCut "$SMPROGRAMS\${Name}.lnk" "$INSTDIR\phatbooks.exe"
SectionEnd

Section "Uninstall"
	Delete "$SMPROGRAMS\${Name}.lnk"
	Delete "$INSTDIR\phatbooks.exe"
	DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\Phatbooks by Matthew Harvey"
	Delete "$INSTDIR\Uninstall.exe"
	RMDir "$INSTDIR"
SectionEnd





