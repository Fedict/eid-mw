on run (pincode)
	tell application "Safari"
		activate
		set URL of front document to "https://iamapps.belgium.be/tma/?lang=nl"
	end tell
	delay 3
	tell application "System Events"
		click UI element "Start test" of UI element 1 of group 5 of UI element 1 of scroll area 1 of group 1 of group 1 of tab group 1 of splitter group 1 of window "Aanmelden testen" of application process "Safari"
		delay 2
		click UI element "Log in" of group 5 of UI element 1 of scroll area 1 of group 1 of group 1 of tab group 1 of splitter group 1 of window "Log in" of application process "Safari"
		delay 2
	end tell
	tell application "System Events" to tell application process "coreautha"
		set target to a reference to (text field 1 of window 1)
		if target exists then
			click target
			keystroke "0075"
			key code 36 (* enter *)
		end if
	end tell
end run
