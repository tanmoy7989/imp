; NSIS (http://nsis.sf.net/) install script

!include "MUI2.nsh"
!include "WinVer.nsh"
!include "LogicLib.nsh"

; Use solid LZMA compression
SetCompressor /SOLID lzma

Var STARTMENU_FOLDER
Var MUI_TEMP

!define PRODUCT "IMP"
; note that VERSION is not defined; must run makensis with e.g. -DVERSION=1.0
!define PRODVER "${PRODUCT}-${VERSION}"

;!define MUI_ICON "tools\w32\imp.ico"
;!define MUI_UNICON "tools\w32\imp.ico"

Name ${PRODVER}

RequestExecutionLevel admin

!ifdef IMP_64BIT
  !define FULL_PRODVER "${PRODUCT} ${VERSION} (64 bit)"
  OutFile "IMP-${VERSION}-64bit.exe"
  InstallDir "$PROGRAMFILES64\${PRODVER}"
  # Note that we don't use InstallDirRegKey on x64 since it ignores SetRegView
  !define PYTHON_ARCH_SUFFIX ""
!else
  !define FULL_PRODVER "${PRODUCT} ${VERSION} (32 bit)"
  OutFile "IMP-${VERSION}-32bit.exe"
  InstallDir "$PROGRAMFILES\${PRODVER}"
  InstallDirRegKey HKLM "Software\${PRODVER}" ""
  !define PYTHON_ARCH_SUFFIX "-32"
!endif

Caption "${FULL_PRODVER} Setup"
!define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKLM"
!define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\${PRODVER}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"

!define MUI_TEMP $R0

!define MUI_ABORTWARNING

!define MUI_WELCOMEPAGE_TITLE "Welcome to the ${FULL_PRODVER} Setup Wizard"
!define MUI_WELCOMEPAGE_TEXT "This wizard will guide you through the installation of the Integrative Modeling Platform (${PRODUCT}) version ${VERSION}."

!define MUI_FINISHPAGE_LINK "https://integrativemodeling.org/"
!define MUI_FINISHPAGE_LINK_LOCATION "https://integrativemodeling.org/"

!define UNINST_KEY "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODVER}"

;Pages
!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU ${PRODVER} $STARTMENU_FOLDER
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES

!insertmacro MUI_LANGUAGE "English"

Section ""
  SetShellVarContext all
  SetOutPath "$INSTDIR"
  ; Generated by top-level Makefile
  !include "w32files.install"

  WriteRegStr HKLM "Software\${PRODVER}" "" $INSTDIR
  WriteUninstaller "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "${UNINST_KEY}" "DisplayName" "${FULL_PRODVER}"
  WriteRegStr HKLM "${UNINST_KEY}" "InstallDirectory" "$INSTDIR"
  WriteRegStr HKLM "${UNINST_KEY}" "UninstallString" "$INSTDIR\Uninstall.exe"
  WriteRegStr HKLM "${UNINST_KEY}" "DisplayVersion" "${VERSION}"
  WriteRegStr HKLM "${UNINST_KEY}" "Publisher" "Andrej Sali Lab, UCSF"
  WriteRegStr HKLM "${UNINST_KEY}" "URLInfoAbout" "https://integrativemodeling.org/"
  WriteRegDWORD HKLM "${UNINST_KEY}" "NoModify" 1
  WriteRegDWORD HKLM "${UNINST_KEY}" "NoRepair" 1

  WriteRegStr HKLM "Software\Python\PythonCore\2.7\PythonPath\${PRODVER}" "" "$INSTDIR\python"
  WriteRegStr HKLM "Software\Python\PythonCore\3.5${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}" "" "$INSTDIR\python"
  WriteRegStr HKLM "Software\Python\PythonCore\3.6${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}" "" "$INSTDIR\python"
  WriteRegStr HKLM "Software\Python\PythonCore\3.7${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}" "" "$INSTDIR\python"
  WriteRegStr HKLM "Software\Python\PythonCore\3.8${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}" "" "$INSTDIR\python"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN ${PRODVER}
    CreateDirectory "$SMPROGRAMS\$STARTMENU_FOLDER"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\README.lnk" "$INSTDIR\README.txt"
    CreateShortCut "$SMPROGRAMS\$STARTMENU_FOLDER\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd

Section "Uninstall"
  SetShellVarContext all
!ifdef IMP_64BIT
  SetRegView 64
!endif
  Delete "$INSTDIR\Uninstall.exe"
  ; Generated by top-level Makefile
  !include "w32files.uninstall"

  ReadRegStr $MUI_TEMP HKLM "Software\${PRODVER}" "Start Menu Folder"

  StrCmp $MUI_TEMP "" noshortcuts
    Delete "$SMPROGRAMS\$MUI_TEMP\README.lnk"
    Delete "$SMPROGRAMS\$MUI_TEMP\Uninstall.lnk"
    RMDir "$SMPROGRAMS\$MUI_TEMP" ;Only if empty, so it won't delete other shortcuts
    
  noshortcuts:

  DeleteRegKey /ifempty HKLM "Software\${PRODVER}"
  DeleteRegKey HKLM "${UNINST_KEY}"
  DeleteRegKey HKLM "Software\Python\PythonCore\2.7\PythonPath\${PRODVER}"
  DeleteRegKey HKLM "Software\Python\PythonCore\3.5${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}"
  DeleteRegKey HKLM "Software\Python\PythonCore\3.6${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}"
  DeleteRegKey HKLM "Software\Python\PythonCore\3.7${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}"
  DeleteRegKey HKLM "Software\Python\PythonCore\3.8${PYTHON_ARCH_SUFFIX}\PythonPath\${PRODVER}"
 
SectionEnd

Function .onInit
!ifdef IMP_64BIT
  SetRegView 64
!endif

!ifdef IMP_64BIT
  ${If} ${AtLeastWinVista}
    Goto version_check_done
  ${Else}
    MessageBox MB_OK \
        "IMP (64 bit) can only be installed on Windows Vista or later. \
         Use the 32 bit version on older versions of Windows." /SD IDOK
    Quit
  ${EndIf}
!else
  ${If} ${IsWinXP}
  ${AndIf} ${AtLeastServicePack} 2
  ${OrIf} ${AtLeastWin2003}
    Goto version_check_done
  ${Else}
    MessageBox MB_OK \
        "IMP can only be installed on Windows XP Service Pack 2 or later." \
        /SD IDOK
    Quit
  ${EndIf}
!endif

  version_check_done:
FunctionEnd
