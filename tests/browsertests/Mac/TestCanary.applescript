on run {link, pin}
	tell application "Google Chrome Canary"
		activate
		delay 2
		tell application "System Events"
			key code 36
		end tell
		delay 5
		set URL of last tab of first window to link
	end tell
	delay 5
	try
		tell application "System Events"
			keystroke tab
			keystroke return
			delay 1
			keystroke tab
			keystroke tab
			keystroke return
			click button "OK" of group "Select a certificate" of window "Select a certificate" of application process "Chrome Canary"
			delay 2
			click window 1 of application process "coreautha"
			keystroke pin
			key code 36 (* enter *)
		end tell
	on error errStr number errorNumber
		log "failed to load page"
		tell application "Google Chrome Canary" to quit
		error errStr number errorNumber
	end try
	delay 3
	tell application "Google Chrome Canary"
		if title of active tab of front window does not contain "phpinfo" then
			log "failed to get certified access"
			quit
			error "Bad certificate" number 401
		else
			log "succes"
			quit
		end if
	end tell
end run
