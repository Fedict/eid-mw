#RequireAdmin
#include <File.au3>
#include <Array.au3>

Func _WinWaitActivate($title,$text,$timeout=0)
	WinWait($title,$text,$timeout)
	If Not WinActive($title,$text) Then WinActivate($title,$text)
	WinWaitActive($title,$text,$timeout)
EndFunc

Func _Au3RecordSetup()
Opt('WinWaitDelay',100)
Opt('WinDetectHiddenText',1)
Opt('MouseCoordMode',0)
EndFunc

_AU3RecordSetup()

Func checkFile($location, $file, $installfile, $log)

	$dlls = _FileListToArrayRec($location, $file, $FLTAR_FILES, $FLTAR_RECUR, $FLTAR_NOSORT, $FLTAR_FULLPATH)
	If @error Then
		_FileWriteLog($log, $file & " not found")
		$errors = $errors + 1
	ElseIf FileGetVersion($installfile, $FV_FILEVERSION) <> FileGetVersion($aArray[1], $FV_FILEVERSION) Then
		   $errors = $errors + 1
		   _FileWriteLog($log, $file & " has wrong version")
	EndIf
EndFunc

Func CheckReg($value, $expected, $log)
	If $expected <> RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid",$value) Then
		If @error Then
			_FileWriteLog($log, $value & " not found")
			$errors = $errors + 1
		Else
			MsgBox(64, "Warning", RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid",$value))
			_FileWriteLog($log, $value & " data does not match data expected")
			$errors = $errors + 1
		EndIf
	EndIf
EndFunc

_Au3RecordSetup()

If $CmdLine[0] <> 1 Then
   MsgBox(64, "Error", "no file given",3)
   Exit(1)
EndIf

Local $logFile = FileOpen(@ScriptDir & "\install.log", 1)
Static $errors = 0
Local $location = $CmdLine[1]
$exefiles = _FileListToArrayRec($location, "*.exe", $FLTAR_FILES, $FLTAR_NORECUR, $FLTAR_NOSORT, $FLTAR_FULLPATH)
If @error Then
	_FileWriteLog($logFile, "No quickinstaller found")
	Exit(1)
Else
	Local $installfile = $exefiles[1]
EndIf

Run($installfile)
_WinWaitActivate("eID Software installation","",30)
Send("{ENTER}")
Sleep(3000)
Send("{ENTER}")
Sleep(100)
Send("{TAB}{ENTER}")

_FileWriteLog($logFile, "file check")
Checkfile("C:\Windows\System32", "beid_ff_pkcs11.dll", $installfile, $logFile)
Checkfile("C:\Windows\System32", "beidpkcs11.dll", $installfile, $logFile)
checkfile("C:\Windows\System32", "beidmdrv64.dll", $installfile, $logFile)
Checkfile("C:\Windows\SysWOW64", "beid_ff_pkcs11.dll", $installfile, $logFile)
Checkfile("C:\Windows\SysWOW64", "beidpkcs11.dll", $installfile, $logFile)
checkfile("C:\Windows\SysWOW64", "beidmdrv32.dll", $installfile, $logFile)

_FileWriteLog($logFile, "Registery check")
CheckReg("80000001", "beidmdrv32.dll", $logFile)
CheckReg("80000100", "en-US,Please enter your PIN;en-EN,Please enter your PIN;nl-BE,Geef uw PIN in;nl-NL,Geef uw PIN in;fr-BE,Entrez votre code PIN;fr-FR,Entrez votre code PIN;de-DE,Bitte geben Sie Ihre PIN ein", $logFile)
CheckReg("80000103", "en-US,Please enter your PIN;en-EN,Please enter your PIN;nl-BE,Geef uw PIN in;nl-NL,Geef uw PIN in;fr-BE,Entrez votre code PIN;fr-FR,Entrez votre code PIN;de-DE,Bitte geben Sie Ihre PIN ein", $logFile)
CheckReg("ATR", "0x3b98004000a503010101ad1300", $logFile)
CheckReg("ATRMask", "0xffff00ff00ffffffffffffff00", $logFile)
CheckReg("Crypto Provider", "Microsoft Base Smart Card Crypto Provider", $logFile)
CheckReg("Smart Card Key Storage Provider", "Microsoft Smart Card Key Storage Provider", $logFile)

If $errors == 0 Then
  MsgBox(64, "info", "test succesfull",1)
  _FileWriteLog($logFile, "test succesfull")
Else
  MsgBox(64, "warning", "test failed, please check logs",1)
  _FileWriteLog($logFile, "test failed with " & $errors & "errors")
EndIf
$errors = 0
fileClose($logFile)
Exit(0)
