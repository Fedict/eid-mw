:: if you are building from svn repo: make sure svnversion.exe is in your path!
::
:: creates svn_revision file in root folder with last svn_revision
:: creates ..\svn_revision and svn_revision.h

:: uses svnversion.exe (searched in PATH)
:: if svnversion.exe is not found 
::    or the root dir is not an svn repo
::    and no svn_revision file exists
:: then, a dummy value is written (should not happen!)


:: tortoise subwcrev
:subwcrev
subwcrev "%~dp0.." "%~dp0.\svn_revision_template" "%~dp0..\svn_revision.txt"
@IF NOT ERRORLEVEL 1 GOTO write_svn_revision_h

:: svnversion
:svnversion
@SET SVN_REVISION=
@FOR /F "tokens=1" %%i in ('svnversion.exe "%~dp0.."') do @SET SVN_REVISION=%%i

@IF NOT DEFINED SVN_REVISION GOTO writedummy
@IF "%SVN_REVISION%"=="" GOTO writedummy
@IF SVN_REVISION==exported GOTO writedummy

:writesvn_revision
@echo %SVN_REVISION%>"%~dp0..\svn_revision"
@echo [INFO] ..\svn_revision set to %SVN_REVISION%
@GOTO write_svn_revision_h

:writedummy
@IF EXIST "%~dp0..\svn_revision" GOTO write_svn_revision_h
@echo 666>"%~dp0..\svn_revision"
@echo [INFO] ..\svn_revision set to 666

:write_svn_revision_h
@FOR /F "tokens=1" %%i in (%~dp0..\svn_revision.txt) do @SET SVN_REVISION=%%i
@SET /A SVN_REVISION+=6000

:: create svn_revision.h file
@echo #ifndef __SVN_REVISION_H__                 >  "%~dp0\svn_revision.h"
@echo #define __SVN_REVISION_H__                 >> "%~dp0\svn_revision.h"
@echo #define SVN_REVISION %SVN_REVISION%        >> "%~dp0\svn_revision.h"
@echo #define SVN_REVISION_STR "%SVN_REVISION%"  >> "%~dp0\svn_revision.h"

@echo #endif //__SVN_REVISION_H__                >> "%~dp0\svn_revision.h"

