#!/usr/bin/python
import plistlib, platform

mac_version=platform.mac_ver()[0]

if '10.10' in mac_version:
 print (mac_version)
 securityd_plist_file = "/System/Library/LaunchDaemons/com.apple.securityd.plist"
 plist = plistlib.Plist.fromFile(securityd_plist_file)

 if 'off' in plist.ProgramArguments:
  print ("off found")
  plist.ProgramArguments = ['/usr/sbin/securityd', '-i']
  plist.write(securityd_plist_file)
