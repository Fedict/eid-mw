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
# this is an example install script to create Windows test environment
# for eID MW development
###############################################################################

###############################################################################
# start Config Section
#

$packagesfolder = "c:\eid_minidriver_certification_env\packages"
$packagesfolderurl = "http://dl.dropbox.com/u/2715381/buildbot/"
$toolsfolder = "c:\eid_minidriver_certification_env\tools"
$minidrivertestfolder = "c:\smartcardminidrivertest"

$userpath = [Environment]::GetEnvironmentVariable("Path",[System.EnvironmentVariableTarget]::User)
#
# end Config Section
###############################################################################

Import-Module BitsTransfer

Write-Host "- Creating $packagesfolder"
New-Item  $packagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $toolsfolder"
New-Item  $toolsfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $minidrivertestfolder"
New-Item  $minidrivertestfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

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
		Start-BitsTransfer -Source $url -Destination $destination -ErrorAction "Stop"
	}
	else 
	{
		Write-Host "   $destination already downloaded. Skip download..."
	}
}
function AddToPathEnv
{
	param([string]$folder)

	$path = [Environment]::GetEnvironmentVariable("Path",[System.EnvironmentVariableTarget]::User) + 
		";" + [Environment]::GetEnvironmentVariable("Path",[System.EnvironmentVariableTarget]::Machine) + 
		";" + $Env:Path
		
	If (!(select-string -InputObject $path -Pattern ("(^|;)" + [regex]::escape($folder) + "(;|`$)") -Quiet)) 
	{
		Write-Host "    $folder not yet in Path. Adding..."
		$env:Path = $env:Path + ";$folder"
		Set-Variable -Name userpath -value "$userpath;$folder" -Scope Global
		### Modify user environment variable ###
		[Environment]::SetEnvironmentVariable( "Path", $userpath , [System.EnvironmentVariableTarget]::User )
	}
}
try {
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
# download and install test code signing cert
# can be found on http://eid-mw.googlecode.com/svn/trunk/windows/fedicteidtest.cer
##############################################################################
$toolfilename = "fedicteidtest.cer"

Write-Host "- Installing Fedict Test Code Signing cert"

# Download file
$tooltarget = "$toolsfolder\$toolfilename"
Start-BitsTransfer -Source "http://eid-mw.googlecode.com/svn/trunk/windows/fedicteidtest.cer" -Destination $tooltarget

# Add to trusted root 
invoke-expression "certutil -addstore root $toolsfolder\$toolfilename"
invoke-expression "certutil -addstore TrustedPublisher $toolsfolder\$toolfilename"

##############################################################################
# download beidmdrv.inf
# can be found on http://eid-mw.googlecode.com/svn/trunk/minidriver/certification/beidmdrv.inf
##############################################################################
$toolfilename = "beidmdrv.inf"

Write-Host "- Download beidmdrv.inf"

# Download file
$tooltarget = "$minidrivertestfolder\$toolfilename"
Start-BitsTransfer -Source "http://eid-mw.googlecode.com/svn/trunk/minidriver/certification/beidmdrv.inf" -Destination $tooltarget

##############################################################################
# download cmck_config.xml
# can be found on http://eid-mw.googlecode.com/svn/trunk/minidriver/certification/cmck_config.xml
##############################################################################
$toolfilename = "cmck_config.xml"

Write-Host "- Download cmck_config.xml"

# Download file
$tooltarget = "$minidrivertestfolder\$toolfilename"
Start-BitsTransfer -Source "http://eid-mw.googlecode.com/svn/trunk/minidriver/certification/cmck_config.xml" -Destination $tooltarget

##############################################################################
# check for cmck.exe (minidriver certification test tool)
##############################################################################

if (! (test-path("$toolsfolder\amd64\cmck.exe")))
{
  Write-Host "- Could not find $toolsfolder\amd64\cmck.exe"
  Write-Host "  Please put cmck.exe, wttlog.dll and intrcptr.dll in $toolsfolder\amd64 and $toolsfolder\x86"
  Write-Host "  More information: http://eid-mw.googlecode.com/svn/trunk/minidriver/certification/readme.txt"
}

} # end try
catch{
  "Error: $_"
  exit 1
}