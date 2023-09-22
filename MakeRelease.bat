@echo off

set BUILDDIR=vsbuild-release

echo Purge previous build
rd /s /q %BUILDDIR%

echo Configuring
mkdir %BUILDDIR%
cd %BUILDDIR%
cmake .. -DDISABLE_BENCHMARKS=ON
rem When executed from script, first call to cmake tends to fail, rerun will properly configure the project 
cmake ..

echo Building
cmake --build . --config Release
ctest -C Release

echo Packing
cpack
mkdir ..\RELEASE
copy *.zip "..\RELEASE\"
