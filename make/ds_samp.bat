@echo off
setlocal
rem copy samples to dist. directory
rem
set DIST_DIR=%1
mkdir %DIST_DIR%\samples
mkdir %DIST_DIR%\samples\dos
mkdir %DIST_DIR%\samples\win32

mkdir %DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\minimal.exe    %DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\minimal.c      %DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\std.pal		%DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\makefile.msc	%DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\makefile.bcc	%DIST_DIR%\samples\dos\minimal
copy samples\dos\minimal\makefile.wat	%DIST_DIR%\samples\dos\minimal

mkdir %DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\minimal.exe    	%DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\minimal.c      	%DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\std.pal			%DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\makefile.msc		%DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\makefile.bcc		%DIST_DIR%\samples\dos\minzs
copy samples\dos\minzs\makefile.wat		%DIST_DIR%\samples\dos\minzs

mkdir %DIST_DIR%\samples\dos\testview
copy samples\dos\testview\testview.exe    %DIST_DIR%\samples\dos\testview
copy samples\dos\testview\makefile.bcc	%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\testview.c      %DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.c		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\venus.dat		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\std.pal			%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\torus.dat		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\shade.tab		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.act		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.mat		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\makefile.wat	%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.pix		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.tab		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example.pal		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\makefile.msc	%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example2.mat	%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\example2.pix	%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\runme.bat		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\readme.txt		%DIST_DIR%\samples\dos\testview
copy samples\dos\testview\file			%DIST_DIR%\samples\dos\testview

mkdir %DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.exe			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\flight				%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\makefile.msc		%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\runme.bat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\makefile.wat		%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\test.c				%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\std.pal			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\makefile.bcc		%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.mat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\terrain5.dat		%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\terrain.pix		%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.h				%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\saucer.dat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\saucer.pix			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.pal			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.tab			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\logo.dat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\logo.pix			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\billb.dat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.dat			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\readme.txt			%DIST_DIR%\samples\dos\fly
copy samples\dos\fly\fly.c				%DIST_DIR%\samples\dos\fly

mkdir %DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\rdemo.exe		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\makefile.bcc		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\rdemo.c			%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\rdemo.mat		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\makefile.wat		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\std.pal			%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\makefile.msc		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\shade.tab		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\ast.pix			%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\moon.pix			%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\earth.pix		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\planet.pix		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\testenv1.pix		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\teapot.dat		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\sph32.dat		%DIST_DIR%\samples\dos\rdemo
copy samples\dos\rdemo\readme.txt		%DIST_DIR%\samples\dos\rdemo

mkdir %DIST_DIR%\samples\dos\pick		%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\makefile.bcc		%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\makefile.msc		%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\makefile.wat		%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\pick.c			%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\pick.exe			%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\pick.mat			%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\readme.txt		%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\shade.tab			%DIST_DIR%\samples\dos\pick
copy samples\dos\pick\std.pal			%DIST_DIR%\samples\dos\pick

mkdir %DIST_DIR%\samples\dos\morph		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\blowup.dat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\cubebend.dat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\cubeblow.dat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\cubemelt.dat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\deform.dat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\makefile.bcc		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\makefile.msc		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\makefile.wat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\morph.c			%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\morph.exe		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\morph.mat		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\shade.tab		%DIST_DIR%\samples\dos\morph
copy samples\dos\morph\std.pal			%DIST_DIR%\samples\dos\morph

mkdir %DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\sheet.exe		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\makefile.wat		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\makefile.bcc     %DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\sheet.mat		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\std.pal			%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\makefile.msc		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\wirscrn2.pix		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\shade.tab		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\cube.h			%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\sheet.c			%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\earth.pix		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\rivrock1.pix		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\sph32.dat		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\8ball.pix		%DIST_DIR%\samples\dos\sheet
copy samples\dos\sheet\readme.txt		%DIST_DIR%\samples\dos\sheet

mkdir %DIST_DIR%\samples\win32\simpview

copy samples\win32\simpview\simpview.exe	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\app.c			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\app.h			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\app.rc			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\argonaut.ico	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\buffer.c		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\buffer.h		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\dispatch.c		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\dispatch.h		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\main.c			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\makefile.wat	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\resource.h		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\simp_vc2.mak	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\simp_vc4.mak	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\wip.ico			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\world.c			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\world.h			%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\mlibcmnt.c		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\earth.pix		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\mandrill.pix	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\teapot.dat		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\cube2.dat		%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\winshade.tab	%DIST_DIR%\samples\win32\simpview
copy samples\win32\simpview\simpview.mat	%DIST_DIR%\samples\win32\simpview

rem mkdir %DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\brwrap.h			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\dfwlib.lib			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\minimal.c			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\mlibcmnt.c			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\os2pal.pal			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\os2pal.txt			%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\os2shade.tab		%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\winimal.exe		%DIST_DIR%\samples\win32\minzs
rem copy samples\win32\minzs\winimal.mak		%DIST_DIR%\samples\win32\minzs

rem mkdir %DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\brwrap.h			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\dfwlib.lib			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\minimal.c			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\mlibcmnt.c			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\os2pal.pal			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\os2pal.txt			%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\os2shade.tab		%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\winimal.exe		%DIST_DIR%\samples\win32\minzb
rem copy samples\win32\minzb\winimal.mak		%DIST_DIR%\samples\win32\minzb

rem mkdir %DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\brwrap.h			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\dfwlib.lib			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\earth.pix			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\help.pix			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\intro.pix			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\logo.pix			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\mlibcmnt.c			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\moon.pix			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\os2pal.pal			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\os2shade.tab		%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\rdemo.c			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\rdemo.exe			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\rdemo.mak			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\rdemo.mat			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\reflect.tab		%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\sph32.dat			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\sprite.h			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\teapot.dat			%DIST_DIR%\samples\win32\rdemo
rem copy samples\win32\rdemo\testenv1.pix		%DIST_DIR%\samples\win32\rdemo

rem mkdir %DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\8ball.pix			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\brwrap.h			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\cube.dat			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\cube.h				%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\dfwlib.lib			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\earth.pix			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\help.pix			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\intro.pix			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\logo.pix			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\mlibcmnt.c			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\os2pal.pal			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\os2pal.txt			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\os2shade.tab		%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\physics.c			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\rivrock1.pix		%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\shade.tab			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.c			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.exe			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.h			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.mak			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.mat			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sheet.txt			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sph32.dat			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\sprite.h			%DIST_DIR%\samples\win32\sheet
rem copy samples\win32\sheet\std.mat			%DIST_DIR%\samples\win32\sheet

endlocal

