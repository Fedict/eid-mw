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

$packagesfolder = "c:\eid_dev_env\packages"
$packagesfolderurl = "http://dl.dropbox.com/u/2715381/buildbot/"
$toolsfolder = "c:\eid_dev_env\tools"
$msysfolder = "c:\eid_dev_env\msys"
$mingw32folder = "c:\eid_dev_env\mingw64-32"
$mingw64folder = "c:\eid_dev_env\mingw64-64"
$svnfolder = "c:\eid_dev_env\svn"
$userpath = [Environment]::GetEnvironmentVariable("Path",[System.EnvironmentVariableTarget]::User)
#
# end Config Section
###############################################################################

Import-Module BitsTransfer

Write-Host "- Creating $packagesfolder"
New-Item  $packagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $toolsfolder"
New-Item  $toolsfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $msysfolder"
New-Item  $msysfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $svnfolder"
New-Item  $svnfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

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
Download "$packagesfolderurl/$toolfilename" $tooltarget

##############################################################################
# install subversion
# can be found on http://subversion.tigris.org/servlets/ProjectDocumentList?folderID=11151&expandFolder=11151&folderID=91
##############################################################################
$toolfilename = "svn-win32-1.6.6.zip"

Write-Host "- Installing Subversion"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

if ($svnfolder -ne "") 
{
	# cleanup svnfolder first
	Remove-Item -Recurse "$svnfolder\*"
	# extract
	Extract $tooltarget $env:Temp

	# move files
	Move-Item -Force "$env:Temp\svn-win32-1.6.6\*" $svnfolder
	Remove-Item "$env:Temp\svn-win32-1.6.6"
	
	Write-Host "Add svn path ($svnfolder\bin) to Path environmental variable."
	AddToPathEnv "$svnfolder\bin"	
}
else 
{
	Write-Host "    Unable to install Subversion. \$svnfolder is not set"
}

##############################################################################
# install MSYS packages
# found on http://sourceforge.net/downloads/mingw/MSYS/
##############################################################################
Write-Host "- Installing MSYS 1.0.15"

##############################################################################
# install MSYS Core 1.0.15-1
# found on http://sourceforge.net/projects/mingw/files/MSYS/BaseSystem/msys-1.0.15-1/msysCORE-1.0.15-1-msys-1.0.15-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS Core 1.0.15"
$toolfilename = "msysCORE-1.0.15-1-msys-1.0.15-bin.tar.lzma"
$toolfilenametar = "msysCORE-1.0.15-1-msys-1.0.15-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder


Write-Host "Add svn path ($msysfolder\bin) to Path environmental variable."
AddToPathEnv "$msysfolder\bin"

##############################################################################
# install MSYS coreutils 1.0.13
# found on http://sourceforge.net/projects/mingw/files/MSYS/coreutils/coreutils-5.97-3/coreutils-5.97-3-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS coreutils 1.0.13"
$toolfilename = "coreutils-5.97-3-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "coreutils-5.97-3-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder


##############################################################################
# install MSYS libregex 1.20090805-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/regex/regex-1.20090805-2/libregex-1.20090805-2-msys-1.0.13-dll-1.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS libregex 1.20090805-2"
$toolfilename = "libregex-1.20090805-2-msys-1.0.13-dll-1.tar.lzma"
$toolfilenametar = "libregex-1.20090805-2-msys-1.0.13-dll-1.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS libtermcap 0.20050421_1
# found on http://sourceforge.net/projects/mingw/files/MSYS/termcap/termcap-0.20050421_1-2/libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS libtermcap 0.20050421_1"
$toolfilename = "libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar.lzma"
$toolfilenametar = "libtermcap-0.20050421_1-2-msys-1.0.13-dll-0.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS libintl 0.17-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/gettext/gettext-0.17-2/libintl-0.17-2-msys-dll-8.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS libintl 0.17-2"
$toolfilename = "libintl-0.17-2-msys-dll-8.tar.lzma"
$toolfilenametar = "libintl-0.17-2-msys-dll-8.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS libiconv 1.13.1-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/libiconv/libiconv-1.13.1-2/libiconv-1.13.1-2-msys-1.0.13-dll-2.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS libiconv 1.13.1-2"
$toolfilename = "libiconv-1.13.1-2-msys-1.0.13-dll-2.tar.lzma"
$toolfilenametar = "libiconv-1.13.1-2-msys-1.0.13-dll-2.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS libcrypt 1.1_1-3
# found on http://sourceforge.net/projects/mingw/files/MSYS/crypt/crypt-1.1_1-3/libcrypt-1.1_1-3-msys-1.0.13-dll-0.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS libcrypt 1.1_1-3"
$toolfilename = "libcrypt-1.1_1-3-msys-1.0.13-dll-0.tar.lzma"
$toolfilenametar = "libcrypt-1.1_1-3-msys-1.0.13-dll-0.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS grep 2.5.4-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/grep/grep-2.5.4-2/grep-2.5.4-2-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS grep 2.5.4-2"
$toolfilename = "grep-2.5.4-2-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "grep-2.5.4-2-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS sed 4.2.1-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/sed/sed-4.2.1-2/sed-4.2.1-2-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS sed 4.2.1-2"
$toolfilename = "sed-4.2.1-2-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "sed-4.2.1-2-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS gawk 3.1.7-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/gawk/gawk-3.1.7-2/gawk-3.1.7-2-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS gawk 3.1.7-2"
$toolfilename = "gawk-3.1.7-2-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "gawk-3.1.7-2-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder


##############################################################################
# install MSYS binutils 2.19.51-3
# found on http://sourceforge.net/projects/mingw/files/MSYS/binutils/binutils-2.19.51-3/binutils-2.19.51-3-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS binutils 2.19.51-3"
$toolfilename = "binutils-2.19.51-3-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "binutils-2.19.51-3-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS bash 3.1.17-3
# found on http://sourceforge.net/projects/mingw/files/MSYS/bash/bash-3.1.17-3/bash-3.1.17-3-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS bash 3.1.17-3"
$toolfilename = "bash-3.1.17-3-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "bash-3.1.17-3-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS perl 5.6.1_2-2
# found on http://sourceforge.net/projects/mingw/files/MSYS/perl/perl-5.6.1_2-2/perl-5.6.1_2-2-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS perl 5.6.1_2-2"
$toolfilename = "perl-5.6.1_2-2-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "perl-5.6.1_2-2-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS autoconf 2.65
# found on http://sourceforge.net/projects/mingw/files/MSYS/autoconf/autoconf-2.65-1/autoconf-2.65-1-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS autoconf 2.65"
$toolfilename = "autoconf-2.65-1-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "autoconf-2.65-1-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS automake 1.11.1-1
# found on http://sourceforge.net/projects/mingw/files/MSYS/automake/automake-1.11.1-1/automake-1.11.1-1-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS automake 1.11.1-1"
$toolfilename = "automake-1.11.1-1-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "automake-1.11.1-1-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS m4 1.4.14-1
# found on http://sourceforge.net/projects/mingw/files/MSYS/m4/m4-1.4.14-1/m4-1.4.14-1-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS m4 1.4.14-1"
$toolfilename = "m4-1.4.14-1-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "m4-1.4.14-1-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install MSYS make 3.81-3
# found on http://sourceforge.net/projects/mingw/files/MSYS/make/make-3.81-3/make-3.81-3-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS make 3.81-3"
$toolfilename = "make-3.81-3-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "make-3.81-3-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder


##############################################################################
# install MSYS tar 1.23-1
# found on http://sourceforge.net/projects/mingw/files/MSYS/tar/tar-1.23-1/tar-1.23-1-msys-1.0.13-bin.tar.lzma/download
##############################################################################
Write-Host "-- Installing MSYS tar 1.23-1"
$toolfilename = "tar-1.23-1-msys-1.0.13-bin.tar.lzma"
$toolfilenametar = "tar-1.23-1-msys-1.0.13-bin.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder

##############################################################################
# install libtool 2.2.7c
# built from source by Fedict
# we need at least 2.2.7c to create working Windows binaries
##############################################################################
Write-Host "-- Installing libtool 2.2.7c"
$toolfilename = "libtool-2.2.7c-fedict.tar.lzma"
$toolfilenametar = "libtool-2.2.7c-fedict.tar"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $packagesfolder
# untar
Extract "$packagesfolder\$toolfilenametar" $msysfolder


##############################################################################
# install mingw64
##############################################################################
Write-Host "- Installing mingw64"
##############################################################################
# install mingw64 for 32 bit
# found on http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win32/
##############################################################################
Write-Host "-- Installing mingw64 targetting 32 bit 1.0 - 20100702"
$toolfilename = "mingw-w32-1.0-bin_i686-mingw_20100702.zip"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $mingw32folder

# The compiler does not look in i686-w64-mingw32\lib for libraries so we copy libole32 it to lib32
Copy-Item $mingw32folder\i686-w64-mingw32\lib\libole32.a $mingw32folder\i686-w64-mingw32\lib32

Write-Host "Add mingw32 path ($mingw32folder\bin) to Path environmental variable."
AddToPathEnv "$mingw32folder\bin"

##############################################################################
# install mingw64 for 64 bit
# found on http://sourceforge.net/projects/mingw-w64/files/Toolchains%20targetting%20Win64/
##############################################################################
Write-Host "-- Installing mingw64 targetting 64 bit 1.0 - 20100913"
$toolfilename = "mingw-w64-1.0-bin_i686-mingw_20100913.zip"
$tooltarget = "$packagesfolder\$toolfilename"

# download file
Download "$packagesfolderurl/$toolfilename" $tooltarget

# extract
Extract $tooltarget $mingw64folder

# The compiler does not look in x86_64-w64-mingw32\lib32 for libraries so we copy libole32 it to lib64
Copy-Item $mingw64folder\x86_64-w64-mingw32\lib32\libole32.a $mingw64folder\x86_64-w64-mingw32\lib64

Write-Host "Add mingw64 path ($mingw64folder\bin) to Path environmental variable."
AddToPathEnv "$mingw64folder\bin"
} # end try
catch{
  "Error: $_"
  exit 1
}