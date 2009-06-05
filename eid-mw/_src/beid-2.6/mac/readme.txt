This is how the contents in the dir have been made.

- On both the PPC and Intel development Macs, take the
  old development dir
- Update the Virify.cpp and change the Makefile to use
  qt 3.3.8. (Result: in /Users/stef/eid/fedicteid_modif_V35.tar.gz)
- Take the resulting libbeid.2.7.3.dylib of both Intel
  and PPC machine and lipo them together into a universal
  binary. Do the same with the libs in all other dirs.
  and put all universal binary libs to libs/
- copy the contents of the /usr/local/etc and
  usr/local/etc/share dirs -> etc, share
- Modify the postflight file -> postflight/

All those files are used by ../eidmw/_Builds/Makefile_mac
to make the (universal) release.
