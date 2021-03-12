on run
	tell application "System Events" to tell application "Firefox Nightly"
		activate
		open location "https://addons.mozilla.org/en-US/firefox/addon/belgium-eid"
	end tell
	delay 5
	tell application "System Events"
		keystroke "'"
		delay 1
		keystroke "Add to Firefox"
		delay 1
		key code 36
		delay 2
		keystroke "t" using control down
		keystroke "a" using control down
		delay 1
		keystroke "o" using control down
	end tell
end run
