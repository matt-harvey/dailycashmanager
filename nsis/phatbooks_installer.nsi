# Put whatever files we want to install to the user's system,
# into same folder as this script. Then compile the script
# using the NSIS compiler, makensisw.


# This script is adapted from one posted at stackoverflow.com/questions/10695674/simple-nsis-recipe-for-basic-installation


!define SHORT_NAME "Phatbooks"
!define LONG_NAME "${SHORT_NAME} by Matthew Harvey"
!define REGPATH_WINUNINST "Software\Microsoft\Windows\CurrentVersion\Uninstall"
!define EXECUTABLE_NAME "${SHORT_NAME}.exe"
!define INSTALLER_NAME "${SHORT_NAME} setup.exe"
!define UNINSTALLER_NAME "Uninstall.exe"

Name "${SHORT_NAME}"
Outfile "${INSTALLER_NAME}"

RequestExecutionLevel admin ;Require admin rights on NT6+ (when UAC is turned on)
!define REGHKEY "HKLM"

InstallDir "$PROGRAMFILES64\${SHORT_NAME}"

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
!insertmacro MUI_PAGE_LICENSE "licence.txt"
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"

Section
	SetShellVarContext all  ;To ensure the shortcut is created in the All Users' folder (where Windows is prone to automatically moving it anyway)
	SetOutPath "$INSTDIR"
	WriteUninstaller "$INSTDIR\${UNINSTALLER_NAME}"
	WriteRegStr "${REGHKEY}" "${REGPATH_WINUNINST}\${LONG_NAME}" "DisplayName" "${SHORT_NAME}"
	WriteRegStr "${REGHKEY}" "${REGPATH_WINUNINST}\${LONG_NAME}" "UninstallString" "$INSTDIR\${UNINSTALLER_NAME}"
	File "${EXECUTABLE_NAME}"
	CreateShortCut "$SMPROGRAMS\${SHORT_NAME}.lnk" "$INSTDIR\${EXECUTABLE_NAME}"
SectionEnd

Section "Uninstall"
	SetShellVarContext all  ;We want to uninstall it from the All Users' folder (see default Section above)
	Delete "$INSTDIR\${UNINSTALLER_NAME}"
	Delete "$INSTDIR\${EXECUTABLE_NAME}"
	DeleteRegKey "${REGHKEY}" "${REGPATH_WINUNINST}\${LONG_NAME}"
	Delete "$SMPROGRAMS\${SHORT_NAME}.lnk"
	RMDir "$INSTDIR"
SectionEnd





