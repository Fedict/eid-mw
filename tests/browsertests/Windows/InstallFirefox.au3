#RequireAdmin
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
Run('"C:\Program Files\Internet Explorer\iexplore.exe" "https://download.mozilla.org/?product=firefox-nightly-stub&os=win&lang=en-US"')
_WinWaitActivate("View Downloads - Internet Explorer","")
Sleep(100)
Send("{RIGHT}{SPACE}")
_WinWaitActivate("Nightly Setup","")
Send("{SPACE}")
_WinWaitActivate("Firefox Nightly First Run Page - Nightly","")
Send("{ALTDOWN}{F4}{ALTUP}")
Sleep(100)
If WinExists("Confirm close","") Then
   WinActivate("Confirm close","")
   Send("{SPACE}")
EndIf
_WinWaitActivate("View Downloads - Internet Explorer","")
Send("{ALTDOWN}{F4}{ALTUP}")
Exit(0)