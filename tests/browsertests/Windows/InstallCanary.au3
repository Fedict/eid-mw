#AutoIt3Wrapper_Res_requestedExecutionLevel=requireAdministrator
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

_Au3RecordSetup()
Run('C:\Users\yannick.schoels\Downloads\ChromeSetup.exe')
If IsAdmin Then
	MsgBox(0, 'Admin', 'Hello', 2)
EndIf
_WinWaitActivate("Welcome to Chrome - Google Chrome","",10)
Send("{ALTDOWN}{F4}{ALTUP}")
Exit(0)