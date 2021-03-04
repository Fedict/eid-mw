on run
	tell application "Safari"
		activate
		set URL of front document to "https://download.mozilla.org/?product=firefox-nightly-latest-l10n-ssl&os=osx&lang=nl"
	end tell
	delay 30
	do shell script "hdiutil mount ~/Downloads/firefox*dmg"
	delay 5
	set nightlyapp to "Firefox Nightly:Firefox Nightly.app"
	tell application "Finder"
		duplicate alias nightlyapp to alias "Macintosh HD:Applications" with replacing
		delay 10
		eject "Firefox Nightly"
	end tell
	do shell script "rm ~/Downloads/firefox*dmg"
end run
