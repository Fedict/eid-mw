; Function
; FileReadFirstLine Function
;   Read and returns the first line of a given file
; Usage :
;	Push File
;   Call FileReadFirstLine
;	Pop  FirstLine
; Result :
;   Return first line of the file, or a blank string when an error occured

Function FileReadFirstLine
;; store register values to stack
Exch $R0 ;swap file url and R0
Push $R1 ;filehandle of opened file R1
Push $R2

StrCpy $R1 0

ClearErrors 		#get a clean start
FileOpen $R1 $R0 r
IfErrors Done
 
FileRead $R1 $0

;remove line endings and spaces
CheckFinalChar:
	StrCpy $R2 "$0" 1 -1		;put final char in $R2
	StrCmp "$R2" " " TrimFinalChar
	StrCmp "$R2" "$\r" TrimFinalChar
	StrCmp "$R2" "$\n" TrimFinalChar
	StrCmp "$R2" "$\t" TrimFinalChar
	GoTo CloseFile
TrimFinalChar:	
	StrCpy $0 "$0" -1			;trim final char
	Goto CheckFinalChar

CloseFile:
FileClose $R1

Done:
 
; restore register values from stack
Pop $R2
Pop $R1
Exch $R0 ; restore R0

Push $0 ;put on stack whether searchstring was found
FunctionEnd

 
 
; Function
; FileSearch Function
;   Searches a file for the occurance of a specified search string
; Usage :
;	Push FileToSearch
;   Push StringToSearch
;   Call FileSearch
;	Push StringFound
; Result :
;   Return 0 when search string was not found in file, or an error occured
;	return 1 when the search string was found in the file

Function FileSearch
;;Delete /REBOOTOK '$ExeDir\Dissociation.log'

;; store register values to stack
Exch $R0 ;swap search string and R0
Exch	 ;put input file on top of stack
Exch $R1 ;swap input file with R1

Push $R2 ;filehandle of opened file R1
Push $R3 ;read line from file
Push $R4 ;length of search string
Push $R5 ;offset in string R3
Push $R6 ;temp string in R3 that contains R4 bytes
 
StrCpy $0 0 		#by default, set to 0 (not found)
StrLen $R4 $R0		


ClearErrors 		#get a clean start
FileOpen $R2 $R1 r
IfErrors Done
 
ReadNextLine:
  ClearErrors
  FileRead $R2 $R3
  IfErrors DoneRead
  StrCpy $R5 0					#reset LineOffset to zero

  SearchLine:
    StrCpy $R6 $R3 $R4 $R5
	StrCmp $R6 "" ReadNextLine 0	#read next line if R6 is empty
	IntOp $R5 $R5 + 1				#increment offset
 
	;check if Line with R5 offset is the search string
    StrCmp $R6 $R0 0 SearchLine	#go back to SearchLine if this was not the string we're looking for
    StrCpy $0 1 				#searchstring was found
			
DoneRead:
  FileClose $R2
Done:
 
 ;; restore register values from stack
Pop $R6
Pop $R5
Pop $R4
Pop $R3
Pop $R2
Exch $R1
Exch
Exch $R0

Push $0 ;put on stack whether searchstring was found
FunctionEnd


; Macros
!macro SearchStringInFile FOUND SEARCH_STRING FILE_TO_SEARCH
  Push `${FILE_TO_SEARCH}`
  Push `${SEARCH_STRING}`
  Call FileSearch
  Pop `${FOUND}`
!macroend

!macro GetFirstLineOfFile FILE_TO_READ FIRSTLINE
  Push `${FILE_TO_READ}`
  Call FileReadFirstLine
  Pop `${FIRSTLINE}`
!macroend