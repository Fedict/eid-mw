 on run {link, pincode}
	tell application "Firefox Nightly"
		activate
		delay 1
		tell application "System Events"
			key code 36 -- return key
		end tell
		delay 2
		tell application "System Events"
			keystroke "l" using {command down}
			keystroke link
			key code 36 -- return key
		end tell
	end tell
	delay 5
		tell application "System Events"
			key code 36
			delay 7
			keystroke pincode
			key code 36
		end tell
	delay 3
	tell application "Firefox Nightly"
		if name of front window does not contain "phpinfo" then
			quit
			error "bad certificate" number 401
		else
			log "succes"
			quit
		end if
	end tell
	delay 3
	
	tell application "System Events"
		key code 36
	end tell
end run
