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
###############################################################################
# this is an example install script to create Windows build environment
# for eID MW development
###############################################################################

###############################################################################
# start Config Section
#

$packagesfolder = "c:\eid_test_env\packages"
$packagesfolderurl = "http://dl.dropbox.com/u/2715381/buildbot/"
$toolsfolder = "c:\eid_test_env\tools"
$rubyfolder = "c:\eid_test_env\ruby"


#
# end Config Section
###############################################################################

Import-Module BitsTransfer

Write-Host "- Creating $packagesfolder"
New-Item  $packagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $toolsfolder"
New-Item  $toolsfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $rubyfolder"
New-Item  $rubyfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null


##############################################################################
# functions
##############################################################################
function Extract
{
	param([string]$zipfilename, [string] $destination)
	# use 7zip to extract
	$tool = "$toolsfolder\7za.exe"

	if(test-path($zipfilename))
	{
		invoke-expression "$tool x -y -o$destination $zipfilename"
	}
}
function Download
{
	param([string]$url, [string] $destination)
	if (! (test-path($destination)))
	{
		# file does not exists
		Start-BitsTransfer -Source $url -Destination $destination
	}
	else 
	{
		Write-Host "   $destination already exists. Skipping..."
	}
}

##############################################################################
# install 7zip command line version 9.15
# can be found on http://sourceforge.net/projects/sevenzip/files/7-Zip/9.15/7za915.zip/download
##############################################################################
$toolfilename = "7za.exe"

Write-Host "- Installing 7zip Command Line Version"

# Download file
$tooltarget = "$toolsfolder\$toolfilename"
Start-BitsTransfer -Source "$packagesfolderurl/$toolfilename" -Destination $tooltarget

##############################################################################
# install subversion
# can be found on http://subversion.tigris.org/servlets/ProjectDocumentList?folderID=11151&expandFolder=11151&folderID=91
##############################################################################
$toolfilename = "svn-win32-1.6.6.zip"

Write-Host "- Installing Subversion"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

# cleanup rubyfolder first
Remove-Item -Recurse "$svnfolder\*"

# extract
Extract $tooltarget $svnfolder

# move files
Move-Item -Force "$svnfolder\svn-win32-1.6.6\*" $svnfolder
Remove-Item "$svnfolder\svn-win32-1.6.6"

##############################################################################
# install ruby 1.9.1 p429
# found on http://files.rubyforge.vm.bytemark.co.uk/rubyinstaller/ruby-1.9.1-p429-i386-mingw32.7z
##############################################################################
Write-Host "-- Installing ruby 1.9.1 p429"
$toolfilename = "ruby-1.9.1-p429-i386-mingw32.7z"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Start-BitsTransfer -Source "$packagesfolderurl/$toolfilename" -Destination $tooltarget

# cleanup rubyfolder first
Remove-Item -Recurse "$rubyfolder\*"

# extract
Extract $tooltarget $rubyfolder

# move files
Move-Item -Force "$rubyfolder\ruby-1.9.1-p429-i386-mingw32\*" $rubyfolder
Remove-Item "$rubyfolder\ruby-1.9.1-p429-i386-mingw32"

##############################################################################
# install cucumber 
##############################################################################
Write-Host "-- Installing cucumber"

invoke-expression "$rubyfolder\bin\gem install cucumber"
