#RequireAdmin
#include <Constants.au3>
#include <File.au3>
#region --- setup ---
Func _Au3RecordSetup()
  Opt('WinWaitDelay',100)
  Opt('WinDetectHiddenText',1)
  Opt('MouseCoordMode',0)
  Local $aResult = DllCall('User32.dll', 'int', 'GetKeyboardLayoutNameW', 'wstr', '')
  If $aResult[1] <> '00000813' Then
    MsgBox(64, 'Warning', 'Recording has been done under a different Keyboard layout' & @CRLF & '(00000813->' & $aResult[1] & ')')
  EndIf
EndFunc
Func _WinWaitActivate($title, $text, $timeout=0)
  WinWait($title,$text,$timeout)
  If Not WinActive($title,$text) Then WinActivate($title,$text)
  WinWaitActive($title,$text,$timeout)
EndFunc

Func checkFile($location, $file, $installfile, $log)

	Local $dlls = _FileListToArrayRec($location, $file, $FLTAR_FILES, $FLTAR_RECUR, $FLTAR_NOSORT, $FLTAR_FULLPATH)
	If @error Then
		_FileWriteLog($log, $file & " not found")
		$errors = $errors + 1
	Else
		Local $dllVersion = FileGetVersion($dlls[1], $FV_FILEVERSION)
		Local $installVersion = FileGetVersion($installfile, $FV_FILEVERSION)
		If $installVersion <> $dllVersion Then
			$errors = $errors + 1
			_FileWriteLog($log, $file & " has wrong version")
		EndIf
	EndIf
EndFunc

Func UnZip($sZipFile, $sDestFolder)
  If Not FileExists($sZipFile) Then Return SetError (1) ; source file does not exists
  If Not FileExists($sDestFolder) Then
    If Not DirCreate($sDestFolder) Then Return SetError (2) ; unable to create destination
  Else
    If Not StringInStr(FileGetAttrib($sDestFolder), "D") Then Return SetError (3) ; destination not folder
  EndIf
  Local $oShell = ObjCreate("shell.application")
  Local $oZip = $oShell.NameSpace($sZipFile)
  Local $iZipFileCount = $oZip.items.Count
  If Not $iZipFileCount Then Return SetError (4) ; zip file empty
  For $oFile In $oZip.items
    $oShell.NameSpace($sDestFolder).copyhere($ofile)
  Next
EndFunc   ;==>UnZip

If $CmdLine[0] <> 3 Then
   MsgBox(64, "Error", "Need a version number and a PIN code on the command line!")
   Exit(1)
EndIf
Local $logFile = FileOpen(@ScriptDir & "\install.log", 1)
Static $errors = 0
#endregion --- setup ---
#region --- installation ---
#region --- download ---
Local $sVersion = $CmdLine[1]
Run('"C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe" "https://dist.eid.belgium.be/releases/' & $sVersion & '/"')
Sleep(2000)
Send("{TAB 9}{ENTER}")
Sleep(2000)
Send("!{F4}")
#endregion --- download ---
#region --- unzip ---
Opt("MustDeclareVars", 1)
Const $sZipFile = EnvGet("USERPROFILE") & "\Downloads\beidmdrv_all_5.0.27.5677.zip"
Const $sDestFolder = EnvGet("USERPROFILE") & "\Downloads\Temp"
Sleep(5000)
UnZip($sZipFile, $sDestFolder)
If @error Then Exit MsgBox ($MB_SYSTEMMODAL,"","Error unzipping file : " & @error)
Sleep(5000)
Local $installfile = $sDestFolder & "\WIN10\beidmdrv.inf"
#endregion --- unzip ---
Run(@ComSpec & ' /k "pnputil -a ' & $installfile & '"')
If @error Then Exit MsgBox ($MB_SYSTEMMODAL,"warning","Error running inf file : " & @error & " and extended: " & @extended)
Sleep(3000)
Send("!{F4}")
#endregion --- installation ---
#region --- check installation ---
_FileWriteLog($logFile, "file check")
CheckFile("C:\Windows\System32", "beid_ff_pkcs11.dll", $installfile, $logFile)
CheckFile("C:\Windows\System32", "beidpkcs11.dll", $installfile, $logFile)
checkFile("C:\Windows\System32", "beidmdrv64.dll", $installfile, $logFile)
CheckFile("C:\Windows\SysWOW64", "beid_ff_pkcs11.dll", $installfile, $logFile)
CheckFile("C:\Windows\SysWOW64", "beidpkcs11.dll", $installfile, $logFile)
checkFile("C:\Windows\SysWOW64", "beidmdrv32.dll", $installfile, $logFile)
If $errors == 0 Then
  MsgBox(64, "info", "test succesfull",1)
  _FileWriteLog($logFile, "test succesfull")
Else
  MsgBox(64, "warning", "test failed, please check logs",1)
  _FileWriteLog($logFile, "test failed with " & $errors & "errors")
EndIf
$errors = 0
fileClose($logFile)
#endregion --- check installation ---
#region --- authentication ---
Opt("WinTitleMatchMode",2)
Run("C:\Program Files (x86)\Microsoft\Edge\Application\msedge.exe")
_WinWaitActivate("New tab","", 120)
Send( "https:\\"&$CmdLine[3] & "{ENTER}")
Sleep(5000)
Send("{TAB 3}{ENTER}")
_WinWaitActivate("Windows Security","", 120)
Send($CmdLine[2] & "{ENTER}")
Sleep(5000)
_WinWaitActivate("PHP 7.3.19","")
Sleep(5000)
Send("{ALTDOWN}{F4}{ALTUP}")
#endregion --- authentication ---
Exit(0)