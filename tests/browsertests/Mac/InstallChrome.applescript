on run
	tell application "Safari"
		activate
		set URL of front document to "https://www.google.com/chrome/browser/canary.html"
	end tell
	delay 15
	tell application "System Events"
		click UI element "Download Chrome Canary" of group 2 of group 3 of UI element 1 of scroll area 1 of group 1 of group 1 of tab group 1 of splitter group 1 of window "Chrome Canary Features For Developers - Google Chrome" of application process "Safari"
		delay 0.1
		click UI element "Mac with Intel chip" of group 1 of group 14 of UI element 1 of scroll area 1 of group 1 of group 1 of tab group 1 of splitter group 1 of window "Chrome Canary Features For Developers - Google Chrome" of application process "Safari"
	end tell
	-- allow for download to finish
	delay 20
	do shell script "hdiutil mount ~/Downloads/googlechromecanary.dmg"
	set canaryapp to "Google Chrome Canary:Google Chrome Canary.app"
	tell application "Finder"
		duplicate alias canaryapp to alias "Macintosh HD:Applications" with replacing
		eject "Google Chrome Canary"
	end tell
	do shell script "rm ~/Downloads/googlechromecanary.dmg"
end run
