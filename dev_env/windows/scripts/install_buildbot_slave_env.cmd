::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: eID Middleware Project.
:: Copyright (C) 2008-2010 FedICT.
::
:: This is free software; you can redistribute it and/or modify it
:: under the terms of the GNU Lesser General Public License version
:: 3.0 as published by the Free Software Foundation.
::
:: This software is distributed in the hope that it will be useful,
:: but WITHOUT ANY WARRANTY; without even the implied warranty of
:: MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
:: Lesser General Public License for more details.
::
:: You should have received a copy of the GNU Lesser General Public
:: License along with this software; if not, see
:: http://www.gnu.org/licenses/.
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
:: use this script to download and run the windows buildbot-slave environment  
:: script buildbot_slave_env.ps1
:: Usage: Run as administrator:
::  install_buildbot_slave_env  <buildmasterhostname>:<buildmasterport> <slavename> <password>
::    buildmasterhostname: hostname of the buildmaster (e.g. buildmaster.yourdomain.net)
::    buildmasterport:     portnumber (e.g. 9989)   
::    slavename:           name of the slave on the buildmaster
::    password:            password of the slave to connect to the buildmaster
:::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::::
mkdir c:\tmp
:: download build_env.ps1 script
powershell.exe -NonInteractive -ExecutionPolicy unrestricted -command "Import-Module BitsTransfer; Start-BitsTransfer -Source http://eid-mw.googlecode.com/svn-history/trunk/dev_env/windows/scripts/buildbot_slave_env.ps1 -Destination c:\tmp\buildbot_slave_env.ps1"
:: run buildbot_slave_env.ps1
powershell.exe -NonInteractive -ExecutionPolicy unrestricted -File c:\tmp\buildbot_slave_env.ps1 %* 