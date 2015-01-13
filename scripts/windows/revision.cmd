:: creates ..\eidmw_revision and eidmw_revision.h

@FOR /F "tokens=1" %%i in ('"C:\Program Files (x86)\Git\bin\git.exe" rev-list --count HEAD') do @SET EIDMW_REVISION=%%i

@IF NOT DEFINED EIDMW_REVISION GOTO writedummy
@IF "%EIDMW_REVISION%"=="" GOTO writedummy
@IF EIDMW_REVISION==exported GOTO writedummy

:writesvn_revision
@echo %EIDMW_REVISION%>"%~dp0\eidmw_revision"
@echo [INFO] \eidmw_revision set to %EIDMW_REVISION%
@GOTO write_eidmw_revision_h

:writedummy
@IF EXIST "%~dp0\eidmw_revision" GOTO set_eidmw_revision
@echo 000>"%~dp0\eidmw_revision"
@echo [INFO] \eidmw_revision set to 000

:set_eidmw_revision
@FOR /F "tokens=1" %%i in (%~dp0..\eidmw_revision) do @SET EIDMW_REVISION=%%i

:write_eidmw_revision_h
:: create eidmw_revision.h file
@echo #ifndef __EIDMW_REVISION_H__                 >  "%~dp0\eidmw_revision.h"
@echo #define __EIDMW_REVISION_H__                 >> "%~dp0\eidmw_revision.h"
@echo #define EIDMW_REVISION %EIDMW_REVISION%        >> "%~dp0\eidmw_revision.h"
@echo #define EIDMW_REVISION_STR "%EIDMW_REVISION%"  >> "%~dp0\eidmw_revision.h"
@echo #endif //__EIDMW_REVISION_H__                >> "%~dp0\eidmw_revision.h"

:write_eidmw_revision_wix
@echo ^<Include^> > "%~dp0\eidmw_revision.wxs"
@echo ^<?define RevisionNumber=%EIDMW_REVISION%?^> >>"%~dp0\eidmw_revision.wxs"
@echo ^</Include^> >>"%~dp0\eidmw_revision.wxs"
