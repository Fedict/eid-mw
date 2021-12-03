#region --- Au3Recorder generated code Start (v3.3.9.5 KeyboardLayout=00000813)  ---

#region --- Internal functions Au3Recorder Start ---
Func _Au3RecordSetup()
Opt('WinWaitDelay',100)
Opt('WinDetectHiddenText',1)
Opt('MouseCoordMode',0)
Local $aResult = DllCall('User32.dll', 'int', 'GetKeyboardLayoutNameW', 'wstr', '')
If $aResult[1] <> '00000813' Then
	MsgBox(64, 'Warning', 'Recording has been done under a different Keyboard layout' & @CRLF & '(00000813->' & $aResult[1] & ')')
EndIf

EndFunc

Func _WinWaitActivate($title,$text,$timeout=0)
	WinWait($title,$text,$timeout)
	If Not WinActive($title,$text) Then WinActivate($title,$text)
	WinWaitActive($title,$text,$timeout)
EndFunc

_AU3RecordSetup()
#endregion --- Internal functions Au3Recorder End ---

If $CmdLine[0] <> 2 Then
	MsgBox(64, 'Error', 'Need a PIN code on the command line!')
	Exit(1)
EndIf
Opt("WinTitleMatchMode",2)
Run('"C:\Program Files\Google\Chrome SxS\Application\Chrome.exe"')
_WinWaitActivate("New Tab","")
Send("https:\\"& $CmdLine[1] & "{ENTER}")
Sleep(5000)
Send("{ENTER}")
Sleep(3000)
_WinWaitActivate("Windows Security","", 120)
If Not WinActive("Windows Security") Then
  Exit(2)
EndIf
Send($CmdLine[2] & "{ENTER}")
_WinWaitActivate("PHP 7.3.31","",120)
If Not WinExists("PHP 7.3.31") Then
   Exit(2)
EndIf
Sleep(5000)
Send("{ALTDOWN}{F4}{ALTUP}")
Exit(0)
#endregion --- Au3Recorder generated code End ---
