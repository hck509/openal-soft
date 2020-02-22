@echo "Run from Visual Studio Command Prompt!"

rmdir build_vs2015_x86 /s /q
mkdir build_vs2015_x86
cd build_vs2015_x86
..\..\cmake-3.12.1-win64-x64\bin\cmake -G "Visual Studio 14 2015" ..\

cd ..

rmdir build_vs2015_x64 /s /q
mkdir build_vs2015_x64
cd build_vs2015_x64
..\..\cmake-3.12.1-win64-x64\bin\cmake -G "Visual Studio 14 2015 Win64" ..\

cd ..

msbuild build_vs2015_x86/OpenAL.sln
msbuild build_vs2015_x64/OpenAL.sln