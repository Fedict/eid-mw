###############################################################################

# eID Middleware Project.
# Copyright (C) 2008-2010 FedICT.
#
# This is free software; you can redistribute it and/or modify it
# under the terms of the GNU Lesser General Public License version
# 3.0 as published by the Free Software Foundation.
#
# This software is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
# Lesser General Public License for more details.
#
# You should have received a copy of the GNU Lesser General Public
# License along with this software; if not, see
# http://www.gnu.org/licenses/.

###############################################################################

# example install script to create Windows build environment
Import-Module BitsTransfer

$packagesfolder = "c:\eid_dev_env\packages\"
$packagesfolderurl = "http://dl.dropbox.com/u/2715381/buildbot/"

Write-Host "- Creating $packagesfolder"
New-Item  $packagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

# install subversion
# can be found on http://www.sliksvn.com/en/download

Write-Host "- Installing Slik-Subversion"
$toolfilename = "Slik-Subversion-1.6.12-x64.msi"
$tooltarget = "$packagesfolder\$toolfilename"

Start-BitsTransfer -Source "$packagesfolderurl$toolfilename" -Destination $packagesfolder

$args = "/passive"
[diagnostics.process]::start($tooltarget, $args).WaitForExit()

# install MSYS
# found on http://downloads.sourceforge.net/mingw/MSYS-1.0.11.exe

#Write-Host "- Installing MSYS 1.0.11"
#$toolfilename = "MSYS-1.0.11.exe"
#$tooltarget = "$packagesfolder\$toolfilename"

#Start-BitsTransfer -Source "$packagesfolderurl$toolfilename" -Destination $packagesfolder

#$args = "/SILENT /SP-"
#[diagnostics.process]::start($tooltarget, $args).WaitForExit()

