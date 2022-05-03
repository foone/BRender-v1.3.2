@echo off
setlocal
rem Copy Tools to dist. directory
rem
set DIST_DIR=%1
mkdir %DIST_DIR%\tools
copy tools\geoconv.exe %DIST_DIR%\tools
copy tools\texconv.exe %DIST_DIR%\tools
copy tools\mkshades.exe %DIST_DIR%\tools
copy tools\mkshades.c %DIST_DIR%\tools
copy tools\mkranges.exe %DIST_DIR%\tools
copy tools\mkranges.c %DIST_DIR%\tools
copy tools\mkranges.txt %DIST_DIR%\tools

copy tools\3ds2br.exe	%DIST_DIR%\tools
copy tools\batconv.bat  %DIST_DIR%\tools
copy tools\batview.bat	%DIST_DIR%\tools
copy tools\test.pal		%DIST_DIR%\tools
copy tools\testshad.tab	%DIST_DIR%\tools

copy demos\dos\testview\testview.exe	%DIST_DIR%\tools

copy c:\bin\vesaq.exe 			%DIST_DIR%\tools
endlocal

