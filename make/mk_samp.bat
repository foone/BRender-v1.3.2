@echo off
rem Build all the samples
setlocal

set REL=r
set INCLUDE=t:\wtc_105\h;t:\wtc_105\h\nt

cd samples\dos\testview
wmake -a REL=%REL% -f makefile.dst
cd ..\fly

wmake -a REL=%REL% -f makefile.dst

cd ..\minimal
wmake -a REL=%REL% -f makefile.dst

cd ..\minzs
wmake -a REL=%REL% -f makefile.dst

cd ..\rdemo
wmake -a REL=%REL% -f makefile.dst

cd ..\sheet
wmake -a REL=%REL% -f makefile.dst

cd ..\morph
wmake -a REL=%REL% -f makefile.dst

cd ..\pick
wmake -a REL=%REL% -f makefile.dst

cd ..\..\win32\simpview
wmake -a REL=%REL% -f makefile.dst

endlocal

