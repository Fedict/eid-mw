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
# this is an example install script to create Windows build_slave for buildbot
#  this will install buildbot-slave and all dependencies, create a buildslave
#  profile, install the buildbot service and start the buildbot slave
# to invoke:
# buildbot_slave_env.ps1 <buildmasterhostname>:<buildmasterport> <slavename> <password>
#    buildmasterhostname: hostname of the buildmaster (e.g. buildmaster.yourdomain.net)
#    buildmasterport:     portnumber (e.g. 9989)   
#    slavename:           name of the slave on the buildmaster
#    password:            password of the slave to connect to the buildmaster
###############################################################################

###############################################################################
# command line parameters
###############################################################################
$buildmasterhostnameandport = $args[0]
$slavename = $args[1]
$slavepassword = $args[2]

###############################################################################
# start Config Section
#
$packagesfolder = "c:\eid_buildbot_env\packages"
$packagesfolderurl = "http://dl.dropbox.com/u/2715381/buildbot/"
$toolsfolder = "c:\eid_buildbot_env\tools"
$svnfolder = "c:\eid_buildbot_env\svn"
$pythonsitepackagesfolder="c:\Python26\Lib\site-packages"
$pythonscriptsfolder="c:\Python26\Scripts"
$pythonbinaryfolder="c:\Python26"
$buildslavefolder="c:\eid_buildbot_env\slave\$slavename"
$userpath = [Environment]::GetEnvironmentVariable("Path",[System.EnvironmentVariableTarget]::User)
#
# end Config Section
###############################################################################

# save current pwd
$oldpwd = pwd
Import-Module BitsTransfer

###############################################################################
# create folders
###############################################################################
Write-Host "- Creating $packagesfolder"
New-Item  $packagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $toolsfolder"
New-Item  $toolsfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $svnfolder"
New-Item  $svnfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $buildslavefolder"
New-Item  $buildslavefolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $pythonbinaryfolder"
New-Item  $pythonbinaryfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $pythonscriptsfolder"
New-Item  $pythonscriptsfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

Write-Host "- Creating $pythonsitepackagesfolder"
New-Item  $pythonsitepackagesfolder -ItemType Directory -ErrorAction SilentlyContinue | Out-Null

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
		Write-Host "   Extract $zipfilename to $destination..."
		New-Item $destination -type directory
		cmd /c "$tool x -y -o$destination $zipfilename"
	}
}

function Download
{
	param([string]$url, [string] $destination)
	Write-Host "   Download $url..."
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

function InstallMSI
{
	param([string]$msifile)
	$product = [WMIClass] "\\.\root\cimv2:Win32_Product"
	$product.Install($msifile, "", $TRUE)
}

function InstallEXEdistinst
{
	# silent installation of an installer created by python's distinst
	# as there is no silent option to the installer, we just unpack everthing in the right folder
	# and hope for the best :)
	param([string]$exefile)
	$randomdir = Get-Random
	# extract to random dir in tmp folder
	Extract $exefile "$env:Temp\$randomdir"
	Copy-Item "$env:Temp\$randomdir\PURELIB\*" -Destination $pythonsitepackagesfolder -Recurse -Force
	Copy-Item "$env:Temp\$randomdir\SCRIPTS\*" -Destination $pythonscriptsfolder -Recurse -Force
	# cleanup
	Remove-Item -Recurse "$env:Temp\$randomdir"
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
# install python 2.6.5
# can be found on http://python.org/download/releases/
##############################################################################
$toolfilename = "python-2.6.5.msi"

Write-Host "- Installing Python 2.6"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

# install
InstallMSI $tooltarget

##############################################################################
# add python paths to path environmental variable
##############################################################################
Write-Host "- Add python paths ($pythonbinaryfolder and $pythonscriptsfolder) to Path environmental variable."
AddToPathEnv $pythonbinaryfolder
AddToPathEnv $pythonscriptsfolder

##############################################################################
# install setuptools 0.6c11
# can be found on http://pypi.python.org/pypi/setuptools
##############################################################################
$toolfilename = "setuptools-0.6c11.win32-py2.6.exe"

Write-Host "- Installing setuptools"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

# install
InstallEXEdistinst $tooltarget 

##############################################################################
# install twisted 10.1.0
# can be found on http://twistedmatrix.com/trac/wiki/Downloads
##############################################################################
$toolfilename = "Twisted-10.1.0.winxp32-py2.6.msi"

Write-Host "- Installing Twisted"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

# install
InstallMSI $tooltarget

##############################################################################
# install zope.interface
# can be found on http://twistedmatrix.com/trac/wiki/Downloads
##############################################################################
Write-Host "- Installing Zope.Interface"
cmd /c easy_install zope.interface

##############################################################################
# install pywin32
# can be found on http://sourceforge.net/projects/pywin32/
##############################################################################
Write-Host "- Installing pywin32"

$toolfilename = "pywin32-214.win32-py2.6.exe"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

cmd /c easy_install $tooltarget

##############################################################################
# install buildbot-slave 0.8.2
# can be found on http://buildbot.net/trac
##############################################################################
Write-Host "- Installing buildbot-slave"

$toolfilename = "buildbot-slave-0.8.2.zip"

# Download file
$tooltarget = "$packagesfolder\$toolfilename"
Download "$packagesfolderurl/$toolfilename" $tooltarget

Extract $tooltarget $env:Temp

# cd to directory of buildbot-slave source as setup fails if ran from other directory
cd "$env:Temp\buildbot-slave-0.8.2\"
cmd /c python setup.py install



# read out file, replace "buildbot.scripts" with "buildslave.scripts"
# and save it in the python-scripts folder
$sourcefile = "$env:Temp\buildbot-slave-0.8.2\contrib\windows\buildbot_service.py"
$destinationfile = "$pythonscriptsfolder\buildbot_service.py"
$content = Get-Content -Path "$sourcefile"
$content | foreach {
#	$string = $_ -Replace [regex]::escape("buildbot.scripts"), "buildslave.scripts" 
	$string
} | Set-Content $destinationfile

##############################################################################
# create buildslave config directory
##############################################################################
cmd /c $pythonscriptsfolder\buildslave create-slave $buildslavefolder $buildmasterhostnameandport $slavename $slavepassword

cd $pythonscriptsfolder
cmd /c python buildbot_service.py --username .\LocalSystem --password nevermind --startup auto install
cmd /c python buildbot_service.py start '$buildslavefolder'

# return to pwd
cd $oldpwd
} # end try
catch{
  "Error: $_"
  exit 1
}