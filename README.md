
First time clone and build instructions:

Note: libzmq and cppzmq need to be installed and built first 
dependencies:
libzmq and cppzmq will be here if installed via vcpkg 

1. Install libzmq and cppzmq from vckpg.zip
- extract vcpkg to C:
- run install_all.bat

2. Clone DummyPrototype and put into C:
- git clone https://github.com/KingScual/DummyPrototype.git

3. Set include paths up under Visual Studio (using VS 2022)

For each Dummy1 / 2 / 3 sln.
Also for Proxy.sln

C:\DummyPrototype\DummyService1\DummyService1.sln
C:\DummyPrototype\DummyService2\DummyService2.sln
C:\DummyPrototype\DummyService3\DummyService3.sln
C:\DummyPrototype\Proxy\Proxy.sln

-Project Properties -> Configuration Properties -> C/C++ -> General -> Additional Include Directories
-add path to zmp.hpp  (C: >whatever you named your folder, default is vcpkg< \installed\x64-windows\include)
-make sure that paths to Proxy folder, ZeroMQ folder, and BitStreamConversion folders are still accurate (might have to update)

-Project Properties -> Configuration Properties -> Linker -> Input -> Additional Dependencies
-add path to libzmq-mt-4_3_5.lib ("C:>whatever you named it, default is vcpkg< \installed\x64-windows\lib\libzmq-mt-4_3_5.lib")

-Project Properties -> Configuration Properties -> Build Events -> Post-Build Events -> Command Line
-add command to add copy of libzmq.dll into .exe directory. Command is "xcopy /y /d "C:>your folder name, default is vcpkg< \installed\x64-windows\bin\libzmq-mt-4_3_5.dll"


4. If it still doesn't work, reach out to Pascual and Levi and we'll update this readme 
