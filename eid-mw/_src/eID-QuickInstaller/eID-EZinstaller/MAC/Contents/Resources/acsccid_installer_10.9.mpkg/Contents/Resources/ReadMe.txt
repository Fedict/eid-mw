ACS CCID PC/SC Driver Installer for Mac OS X
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

Version: 1.0.6
Release Date: 17/4/2014

CCID Readers

VID  PID  Reader              Reader Name
---- ---- ------------------- -----------------------------
072F B301 ACR32-A1            ACS ACR32 ICC Reader
072F 8300 ACR33U-A1           ACS ACR33U-A1 3SAM ICC Reader
072F 8302 ACR33U-A2           ACS ACR33U-A2 3SAM ICC Reader
072F 8307 ACR33U-A3           ACS ACR33U-A3 3SAM ICC Reader
072F 8301 ACR33U              ACS ACR33U 4SAM ICC Reader
072F 90CC ACR38U-CCID         ACS ACR38U-CCID
072F 90CC ACR100-CCID         ACS ACR38U-CCID
072F 90D8 ACR3801             ACS ACR3801
072F B100 ACR39U              ACS ACR39U ICC Reader
072F B101 ACR39K              ACS ACR39K ICC Reader
072F B102 ACR39T              ACS ACR39T ICC Reader
072F B103 ACR39F              ACS ACR39F ICC Reader
072F B104 ACR39U-SAM          ACS ACR39U-SAM ICC Reader
072F B000 ACR3901U            ACS ACR3901U ICC Reader
072F 90D2 ACR83U-A1           ACS ACR83U
072F 8306 ACR85               ACS ACR85 PINPad Reader
072F 2011 ACR88U              ACS ACR88U
072F 8900 ACR89U-A1           ACS ACR89 ICC Reader
072F 8901 ACR89U-A2           ACS ACR89 Dual Reader
072F 8902 ACR89U-A3           ACS ACR89 FP Reader
072F 1205 ACR100I             ACS ACR100 ICC Reader
072F 1204 ACR101              ACS ACR101 ICC Reader
072F 1206 ACR102              ACS ACR102 ICC Reader
072F 2200 ACR122U             ACS ACR122U
072F 2200 ACR122U-SAM         ACS ACR122U
072F 2200 ACR122T             ACS ACR122U
072F 2214 ACR1222U-C1         ACS ACR1222 1SAM PICC Reader
072F 1280 ACR1222U-C3         ACS ACR1222 1SAM Dual Reader
072F 2207 ACR1222U-C6         ACS ACR1222 Dual Reader
072F 222B ACR1222U-C8         ACS ACR1222 1SAM PICC Reader
072F 2206 ACR1222L-D1         ACS ACR1222 3S PICC Reader
072F 222E ACR123U             ACS ACR123 3S Reader
072F 2237 ACR123U             ACS ACR123 PICC Reader
072F 2219 ACR123U Bootloader  ACS ACR123US_BL
072F 2203 ACR125              ACS ACR125 nPA plus
072F 221A ACR1251U-A1         ACS ACR1251 1S CL Reader
072F 2229 ACR1251U-A2         ACS ACR1251 CL Reader
072F 222D [OEM Reader]        [OEM Reader Name]
072F 2218 ACR1251U-C (SAM)    ACS ACR1251U-C Smart Card Reader
072F 221B ACR1251U-C          ACS ACR1251U-C Smart Card Reader
072F 2232 ACR1251UK           ACS ACR1251K Dual Reader
072F 223B ACR1252U-A1         ACS ACR1252 1S CL Reader
072F 223E ACR1252U-A2         ACS ACR1252 CL Reader
072F 223D ACR1252U BL         ACS ACR1252 USB FW_Upgrade v100
072F 2239 ACR1256U            ACS ACR1256U PICC Reader
072F 2100 ACR128U             ACS ACR128U
072F 2224 ACR1281U-C1         ACS ACR1281 1S Dual Reader
072F 220F ACR1281U-C2 (qPBOC) ACS ACR1281 CL Reader
072F 2223 ACR1281U    (qPBOC) ACS ACR1281 PICC Reader
072F 2208 ACR1281U-C3 (qPBOC) ACS ACR1281 Dual Reader
072F 0901 ACR1281U-C4 (BSI)   ACS ACR1281 PICC Reader
072F 220A ACR1281U-C5 (BSI)   ACS ACR1281 Dual Reader
072F 2215 ACR1281U-C6         ACS ACR1281 2S CL Reader
072F 2220 ACR1281U-C7         ACS ACR1281 1S PICC Reader
072F 2233 ACR1281U-K          ACS ACR1281U-K PICC Reader
072F 2234 ACR1281U-K          ACS ACR1281U-K Dual Reader
072F 2235 ACR1281U-K          ACS ACR1281U-K 1S Dual Reader
072F 2236 ACR1281U-K          ACS ACR1281U-K 4S Dual Reader
072F 2213 ACR1283L-D1         ACS ACR1283 4S CL Reader
072F 222C ACR1283L-D2         ACS ACR1283 CL Reader
072F 220C ACR1283 Bootloader  ACS ACR1283U FW Upgrade
072F 0102 AET62               ACS AET62 PICC Reader
072F 0103 AET62               ACS AET62 1SAM PICC Reader
072F 0100 AET65               ACS AET65 ICC Reader
072F 8201 APG8201-A1          ACS APG8201
072F 8202 [OEM Reader]        [OEM Reader Name]
072F 90DB CryptoMate64        ACS CryptoMate64

non-CCID Readers

VID  PID  Reader              Reader Name
---- ---- ------------------- -----------------------------
072F 9000 ACR38U              ACS ACR38U
072F 90CF ACR38U-SAM          ACS ACR38U-SAM
072F 90CE [OEM Reader]        [OEM Reader Name]
072F 0101 AET65               ACS AET65 1SAM ICC Reader
072F 9006 CryptoMate          ACS CryptoMate

Operating Systems

Mac OS X 10.5 or above



2. Installation
---------------

1. In Mac OS X, double click "acsccid_installer-1.0.6.dmg" to mount the disk image file.

2. To install the driver, double click "acsccid_installer.mpkg" and follow onscreen instructions. It will install acsccid driver package and pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/).

3. To uninstall the acsccid driver and pcscd_autostart package, run Terminal program. Enter the following commands in the mounted disk image folder:

# cd "/Volumes/ACS CCID PC:SC Driver Installer"
# sudo ./uninstall_acsccid.sh
# sudo ./uninstall_pcscd_autostart.sh

4. If you want to restore ACS readers support from Apple CCID driver, run Terminal program. Enter the following commands in the mounted disk image folder:

# cd "/Volumes/ACS CCID PC:SC Driver Installer"
# sudo ./uninstall_ccid_remove_acs.sh

5. This driver also supports non-CCID readers such as ACR38. If you want to remove the ACR38 driver, run Terminal program. Enter the following commands in the mounted disk image folder:

# cd "/Volumes/ACS CCID PC:SC Driver Installer"
# sudo ./uninstall_acr38driver.sh



3. History
----------

v1.0.0 (18/9/2009)
1. New release
2. Based on ccid-1.3.11 (http://pcsclite.alioth.debian.org/ccid.html).
3. Include pcscd_autostart package from OpenSC project (http://www.opensc-project.org/sca/). It will make pcscd to run at startup.
4. There is a pcscd problem supporting multi-slot readers on Mac OS X. It will create duplicate reader name for each slot. ACR88U and ACR128U readers are affected by this bug. For more information, please refer to http://www.opensc-project.org/sca/wiki/LeopardBugs.

v1.0.0 (14/10/2009)
1. Test the driver on Mac OS X 10.6.
2. Update uninstall script.

v1.0.1 (9/11/2009)
1. Update driver to v1.0.1.

v1.0.2 (16/3/2011)
1. Update driver to v1.0.2.

v1.0.2 (10/8/2011)
1. Fix pcscd autostart problem on Mac OS X 10.7.
2. Remove ACS readers from Apple CCID driver.

v1.0.3 (13/1/2012)
1. Update driver to v1.0.3.

v1.0.4 (15/6/2012)
1. Update driver to v1.0.4.

v1.0.5 (11/9/2013)
1. Update driver to v1.0.5.

v1.0.6 (17/4/2014)
1. Update driver to v1.0.6.
2. Fix ccid_remove_acs installation problem.



4. File Contents
----------------

License.txt
ReadMe.txt
acsccid_installer.mpkg
uninstall_acr38driver.sh
uninstall_acsccid.sh
uninstall_pcscd_autostart.sh
uninstall_ccid_remove_acs.sh



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
