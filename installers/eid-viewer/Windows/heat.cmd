:: Check WiX
:: =========
@call "%~dp0..\..\..\windows\SetPaths.bat"
@if %ERRORLEVEL%==1 goto end


:: Create the revision number
:: ==========================
set SVNREvision=
@call "%~dp0..\..\..\windows\svn_revision.cmd"

if not "%SVNRevision%"=="" goto skiptarfile
Set /P SVNRevision=< ..\..\..\svn_revision                                                      

:skiptarfile
Set /A SVNRevision+=6000

echo ^<Include^>>svn_revision.wxs
echo ^<?define RevisionNumber=%SVNRevision%?^>>>svn_revision.wxs
echo ^</Include^>>>svn_revision.wxs


:: create the lib files' identifiers
:: ==================================
::heat dir path -srd -dr MyDirName -cg SampleGroup -out SampleGroup.wxs

::"%BEID_DIR_WIX%\bin\heat"
"%BEID_DIR_WIX%\bin\heat" dir ..\..\..\eid-viewer\eid-viewer\lib -srd -dr BEIDVIEWERLIBDIR -cg BEIDVIEWERLIB -gg -out libfiles.wxs


echo [INFO] Done creating lib files' identifiers
:END
echo end