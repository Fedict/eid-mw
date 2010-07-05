ACR38 PC/SC Driver Installer 1.7.10 for Mac OS X
Advanced Card Systems Ltd.



Contents
----------------

   1. Release Notes
   2. Installation
   3. History
   4. File Contents
   5. Limitations
   6. Support



1. Release Notes
----------------

Version: 1.7.10
Release Date: 14/10/2009

Operating Systems
Mac OS X 10.5/10.6



2. Installation
---------------

1. In Mac OS X, double click "acr38driver_installer-1.7.10.dmg" to mount the disk image file.

2. To install the driver, double click "acr38driver_installer.mpkg" and follow onscreen instructions. It will install acr38driver package and pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/).

3. To uninstall the acr38driver and pcscd_autostart package, run Terminal program. Enter the following commands in the mounted disk image folder:
   
# sudo ./uninstall_acr38driver.sh
# sudo ./uninstall_pcscd_autostart.sh



3. History
----------

v1.7.9-10.4 (1/8/2007)
1. Fix the USB read/write problem in Mac OS X v10.4.9 or above.

v1.7.9-10.5 (15/1/2008)
1. Update installer for Mac OS X 10.5

v1.7.9-10.5 (1/3/2008)
1. pcscd has a bug that it sets T=1 when the SetCOS card is inserted. IFDHSetProtocolParameters rejects incorrect protocol settings now and pcscd will try T=0 on the card if T=1 is failed.

v1.7.10-10.5 (29/6/2009)
1. Fix PPS T0/T1 problem (import from Linux v1.7.10).
2. Rewrite Adm_Transmit function. The response buffer relies on parameter (import from Linux v1.7.10).
3. Support multiple readers (import from Linux v1.7.9).

v1.7.10-10.5 (7/9/2009)
1. Increase the number of supported readers to 16 (Maximum number of supported readers in pcsc-lite).
2. Call Mac OS X USB API directly instead of using libusb.
3. Modify OpenUSB function to have channel parameter in order to identify the device using LocationID from pcsc-lite.

v1.7.10-10.5-10.6 (14/10/2009)
1. Test the driver on Mac OS X 10.6.
2. Modify the installer to use pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/). It will make pcscd to run at startup.
3. Add uninstall scripts.



4. File Contents
----------------

License.txt
ReadMe.txt
acr38driver_installer.mpkg
uninstall_acr38driver.sh
uninstall_pcscd_autostart.sh



5. Limitations
--------------



6. Support
----------

In case of problem, please contact ACS through:

Web Site: http://www.acs.com.hk/
E-mail: info@acs.com.hk
Tel: +852 2796 7873
Fax: +852 2796 1286



-----------------------------------------------------------------


Copyright
Copyright by Advanced Card Systems Ltd. (ACS) No part of this reference manual may be reproduced or transmitted in any from without the expressed, written permission of ACS.

Notice
Due to rapid change in technology, some of specifications mentioned in this publication are subject to change without notice. Information furnished is believed to be accurate and reliable. ACS assumes no responsibility for any errors or omissions, which may appear in this document.
