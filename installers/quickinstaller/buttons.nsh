; Defines
!define buttonVisible "!insertMacro buttonVisible"
!define buttonEnabled "!insertMacro buttonEnabled"
!define buttonText "!insertMacro buttonText"
 
; Interim macros
!macro buttonVisible button value
	Push ${button}
	Push v
	Push ${value}
	Call Buttons
!macroend
 
!macro buttonEnabled button value
	Push ${button}
	Push e
	Push ${value}
	Call Buttons
!macroend
 
!macro buttonText button value
	Push ${button}
	Push t
	Push "${value}"
	Call Buttons
!macroend

 
; Function
; Buttons FUnction
;   Used to interact with the three default buttons; Back, Next, Cancel
; Usage :
;   Push Back|Next|Cancel
;   Push v|e|t
;   Push 1|0|<string>
;   Call Buttons | Call Un.Buttons
; Result :
;   Interacts with the appropriate button
;   If 'v', then 1 will set visible and 0 will set invisible
;   If 'e', then 1 will set enabled and 0 will set disabled
;   If 't', then <string> will be set as the button's new label text
	Function Buttons
									; Stack: <value> <mode> <button>
		Exch $2 ; value				; Stack: $2 <mode> <button>
		Exch						; Stack: <mode> $2 <button>
		Exch $1 ; mode				; Stack: $1 $2 <button>
		Exch 2						; Stack: <button> $2 $1
		Exch $0 ; button			; Stack: $0 $2 $1
		Push $3						; Stack: $3 $0 $2 $1
 
		StrCmp $0 "Back" 0 +3
			StrCpy $3 3
			goto _setbutton
		StrCmp $0 "Next" 0 +3
			StrCpy $3 1
			goto _setbutton
		StrCmp $0 "Cancel" 0 _end
			StrCpy $3 2
 
		_setbutton:
		GetDlgItem $3 $HWNDPARENT $3
 
		StrCmp $1 "v" 0 +3
			ShowWindow $3 $2
			goto _end
		StrCmp $1 "e" 0 +3
			EnableWindow $3 $2
			goto _end
		StrCmp $1 "t" 0 _end
			SendMessage $3 ${WM_SETTEXT} 0 "STR:$2"
 
		_end:
 
									; Stack: $3 $0 $2 $1
		Pop $3						; Stack: $0 $2 $1
		Pop $0						; Stack: $2 $1
		Pop $2						; Stack: $1
		Pop $1						; Stack: [clean]
	FunctionEnd
