:: set the eID Middleware versions
:: for eIDViewer: set the viewer version also in eidmw\plugins_tools\eid-viewer\Windows\eIDViewer\Sources\About.cs


@SET BASE_VERSION1=5
@SET BASE_VERSION2=0
@SET BASE_VERSION3=25
@SET YEAR=2021


@IF EXIST "C:\Program Files (x86)\Git\bin\git.exe" (
	set GIT_PATH="C:\Program Files (x86)\Git\bin\git.exe"
	) ELSE (
		@IF EXIST "C:\Program Files\Git\bin\git.exe" (
			set GIT_PATH="C:\Program Files\Git\bin\git.exe"
		)
	)	
@FOR /F "tokens=1" %%i in ('%GIT_PATH% rev-list --count HEAD') do @SET EIDMW_REVISION=%%i

@IF DEFINED EIDMW_REVISION GOTO set_revisions
:: try if git.exe is in PATH env var
@FOR /F "tokens=1" %%i in ('"git.exe" rev-list --count HEAD') do @SET EIDMW_REVISION=%%i

:set_revisions
@IF NOT DEFINED EIDMW_REVISION GOTO set_zero_revision
@IF "%EIDMW_REVISION%"=="" GOTO set_zero_revision
@IF EIDMW_REVISION==exported GOTO set_zero_revision

@echo BASE_VERSION1 set to %BASE_VERSION1%
@echo BASE_VERSION2 set to %BASE_VERSION2%
@echo BASE_VERSION3 set to %BASE_VERSION3%
@echo EIDMW_REVISION set to %EIDMW_REVISION%
@echo YEAR set to %YEAR%
@GOTO end

:set_zero_revision
@echo [INFO] \eidmw_revision set to 000
@SET EIDMW_REVISION=0000

:end
