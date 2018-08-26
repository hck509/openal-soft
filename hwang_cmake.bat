@echo "Run from Visual Studio Command Prompt!"

mkdir build_vs2015_win32
cd build_vs2015_win32
..\..\cmake-3.12.1-win64-x64\bin\cmake -G "Visual Studio 14 2015" ..\

cd ..

mkdir build_vs2015_win64
cd build_vs2015_win64
..\..\cmake-3.12.1-win64-x64\bin\cmake -G "Visual Studio 14 2015 Win64" ..\

cd ..