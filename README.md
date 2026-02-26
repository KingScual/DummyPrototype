
First time clone and build instructions:

Note: libzmq and cppzmq need to be installed and built first 
dependencies:
libzmq and cppzmq will be here if installed via vcpkg 

C:\vcpkg\installed\x64-windows\bin\libzmq-mt-4_3_5.dll
C:\vcpkg\installed\x64-windows\include\zmq.hpp

1. git clone https://github.com/KingScual/DummyPrototype.git

put under C: , name folder DummyPrototype

2. setup up dependencies for each DummyService to use (i.e. headers Messages.h, Proxy.h, ZeroMQ.h)

Properties > Configuration Properties > C/C++ > General > Additional Include Directories
C:\vcpkg\installed\x64-windows\bin;C:\DummyPrototype\Proxy\Proxy;C:\DummyPrototype\ZeroMQ;C:\DummyPrototype\BitStreamConversion;C:\DummyPrototype\Messages;%(AdditionalIncludeDirectories)
(there might be additional Linker dependencies under
Properties > Configuration Properties > Linker > Input > Additional Dependencies
of which put C:\vcpkg\installed\x64-windows\include\zmq.hpp here

implementation files need to be added to each DummyService.sln
Messages.cpp, Proxy.cpp, ZeroMQ.cpp, BitStreamConversion.cpp

3.  Build
There are 4 solutions to build:
C:\DummyPrototype\DummyService1\DummyService1.sln
C:\DummyPrototype\DummyService2\DummyService2.sln
C:\DummyPrototype\DummyService3\DummyService3.sln
C:\DummyPrototype\Proxy\Proxy.sln

4. If it still doesn't work, reach out to Pascual and Levi and we'll update this readme 
