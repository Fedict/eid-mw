on run {link, pincode}
	tell application "System Events" to tell application "Firefox Nightly"
		activate
		open location link
	end tell
	delay 5
	try
		tell application "System Events"
			key code 36
			delay 5
			click text field 1 of window 1 of application process "UserNotificationCenter"
			keystroke pincode
			key code 36
		end tell
	on error errStr number errNr
		log "failed to load page"
		tell application "Firefox Nightly" to quit
		error errStr number errNr
	end try
	delay 3
	tell application "Firefox Nightly"
		if name of front window does not contain "phpinfo" then
			quit
			error "bad certificate" number 401
		else
			quit
		end if
	end tell
end run
