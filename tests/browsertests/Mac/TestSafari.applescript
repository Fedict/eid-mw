on run {link, pin}
	tell application "Safari"
		activate
		delay 2
		tell application "System Events"
			key code 36
		end tell
		delay 2
		set URL of front document to link
	end tell
	delay 3
	try
		tell application "System Events"
			key code 36
			delay 2
			click window 1 of application process "coreautha"
			keystroke pin
			key code 36
		end tell
	on error errStr number errorNumber
		log "failed to load page"
		tell application "Safari" to quit
		error errStr number errorNumber
	end try
	delay 3
	tell application "Safari"
		if name of current tab in window 1 does not contain "phpinfo" then
			log "failed to get certified access"
			tell application "Safari" to quit
			error "Bad certificate" number 401
		else
			log "succes"
			quit
		end if
	end tell
end run