@echo off
set "LLVMPath=C:\Program Files\LLVM"
set "VSPath=C:\Program Files (x86)\Microsoft Visual Studio\2017\Community"
set "VSVersion=14.10.25017"
set "WinSDKVersion=10.0.14393.0"
set "WinSDKPath=C:\Program Files (x86)\Windows Kits\10"
set "OUT=fasm.exe"

set "VSBasePath=%VSPath%\VC\Tools\MSVC\%VSVersion%"
set "PATH=%PATH%;%LLVMPath%\bin;%VSBasePath%\bin\HostX64\x64"

set CFLAGS= ^
	-std=c++14 ^
	-Wno-everything

set LDFLAGS= ^
 -machine:x64 ^
 -subsystem:console 

@ctime -begin fasm.ctm
set LDLIBS= ^
	/libpath:"%VSBasePath%\lib\x64" ^
	/libpath:"%WinSDKPath%\Lib\%WinSDKVersion%\ucrt\x64" ^
	/libpath:"%WinSDKPath%\Lib\%WinSDKVersion%\um\x64" ^
	/libpath:"C:\Program Files\LLVM\lib" ^
	libcmt.lib User32.lib

@echo on
@echo Compiling...
@clang.exe -g -gcodeview fasm.cc -o fasm.o -c %CFLAGS%

@link.exe /debug fasm.o -out:"%OUT%" %LDFLAGS% %LDLIBS%
@ctime -end fasm.ctm
@echo Done