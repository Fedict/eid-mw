#include <File.au3>

Func checkdetails($file)
	Local $ok = 0
	Local $version="0.0.0.0"
	If FileExists($file)Then
		$version = FileGetVersion($file)
		_FileWriteLog($hFile, $file & " version:" &$version)
	Else
		_FileWriteLog($hFile, $file & " file not found")
	EndIf

	If $version == FileGetVersion($CmdLine[0])Then
		$ok = 1
	Else
		If $version <> "0.0.0.0" Then
			_FileWriteLog($hFile, " wrong version")
		EndIf
	EndIf
	Return $ok
EndFunc

Local $hFile = FileOpen(@ScriptDir & "\instal.log", 1)
If $CmdLine[0] <> 1 Then
   MsgBox(64, "Error", "Need install file location!")
   Exit(1)
EndIf
Local $hFile = FileOpen(@ScriptDir & "\instal.log", 1)
_FileWriteLog($hFile, $CmdLine[1] & " version: " & FileGetVersion($CmdLine[1]))
Run($CmdLine[1])
Local $procede = False
Local $sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "80000001")
If $sRegVal == "beidmdrv64.dll" Then
	$procede = true
Else
	_FileWriteLog($hFile, "80000001: wrong Register value ")
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "80000100")
    If $sRegVal == "en-US,Please enter your PIN;en-EN,Please enter your PIN;nl-BE,Geef uw PIN in;nl-NL,Geef uw PIN in;fr-BE,Entrez votre code PIN;fr-FR,Entrez votre code PIN;de-DE,Bitte geben Sie Ihre PIN ein" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "80000100: wrong Register value ")
	EndIf
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "80000103")
    If $sRegVal == "en-US,Please enter your PIN;en-EN,Please enter your PIN;nl-BE,Geef uw PIN in;nl-NL,Geef uw PIN in;fr-BE,Entrez votre code PIN;fr-FR,Entrez votre code PIN;de-DE,Bitte geben Sie Ihre PIN ein" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "80000103: wrong Register value ")
	EndIf
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "ATR")
    If $sRegVal == "0x3B98004000A503010101AD1300" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "ATR: wrong Register value ")
	EndIf
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "ATRMask")
    If $sRegVal == "0xFFFF00FF00FFFFFFFFFFFFFF00" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "ATRMask: wrong Register value ")
	EndIf
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "Crypto Provider")
    If $sRegVal == "Microsoft Base Smart Card Crypto Provider" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "Crypto Provider: wrong Register value ")
	EndIf
EndIf
If $procede then
	$sRegVal = RegRead("HKEY_LOCAL_MACHINE\SOFTWARE\Microsoft\Cryptography\Calais\SmartCards\Beid", "Smart Card Key Storage Provider")
    If $sRegVal == "Microsoft Smart Card Key Storage Provider" Then
		$procede = true
	Else
		_FileWriteLog($hFile, "Smart Card Key Storage Provider: wrong Register value ")
	EndIf
EndIf
If $procede Then
	$procede = checkdetails("C:\Windows\System32\beid_ff_pkcs11.dll")
EndIf
If $procede Then
	$procede = checkdetails("C:\Windows\System32\beidpkcs11.dll")
EndIf
If $procede Then
	$procede = checkdetails("C:\Windows\System32\DriverStore\FileRepository\beidmdrv.inf_amd64_47cca707f6231b81\beidmdrv32.dll")
EndIf

If $procede Then
	_FileWriteLog($hFile,"succes")
	FileClose($hFile) ; Close the filehandle to release the file.
	Exit(0)
Else
	_FileWriteLog($hFile,"install failed")
	FileClose($hFile) ; Close the filehandle to release the file.
	Exit(2)
EndIf