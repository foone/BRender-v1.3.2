@echo off
rem Build All Libraries
rem
setlocal

set BR_BUILD=RELEASE

rem Borland
rem 
call cbc45.bat
call %BR_MAKE_DIR%\mk_bcc.bat %1 %2 %3 %4 %5 %6 %7 %8 %9

rem Microsoft
rem
call cmvc41.bat
call %BR_MAKE_DIR%\mk_mvc.bat %1 %2 %3 %4 %5 %6 %7 %8 %9

rem Watcom
rem
call cwat106.bat
call %BR_MAKE_DIR%\mk_wtc.bat %1 %2 %3 %4 %5 %6 %7 %8 %9

endlocal
