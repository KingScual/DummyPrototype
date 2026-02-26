There are 3 "Dummy Services" that will individually need to be built using Visual Studio 2022 (it should also work with VS2017 but I don't have that version installed to test)
Each Dummy Service contains a .sln file if each of their respective folders. (I'm working on a script to build all three at the same time)

There is already existing .exe files for each Dummy Service in their current state that can be launched using the Launch.bat script and closed using the Close.bat script.

Each Dummy Service contains an App.cpp and App.h file where all the code will be (I'm assuming) with comments on what each section does.

The main branch doesn't contain any ZeroMQ code.
This is becuase I wanted a clean version of the prototype that I know can be built and run.
I'm thinking we can download or create our own branches to work on the code and keep the main branch as a clean version.

First time clone and build instructions:

*** Note: libzmq and cppzmq need to be **installed** and **built** first ***
*** dependencies:
*** libzmq and cppzmq will be here if installed via vcpkg:
*** C:\vcpkg\installed\x64-windows\bin\libzmq-mt-4_3_5.dll
*** C:\vcpkg\installed\x64-windows\include\zmq.hpp

git clone https://github.com/KingScual/DummyPrototype.git

