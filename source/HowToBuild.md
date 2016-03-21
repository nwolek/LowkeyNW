# How To Build

The build system for this project is based on [Max-SDK-Lite](https://github.com/tap/max-sdk-lite).


## Prerequisites

To build the externals in this package you will need some form of compiler support on your system. 

* On the Mac this means Xcode (you can get from the App Store for free). 
* On Windows this most likely means some version of Visual Studio (the free versions should work fine).

You will also need to install [CMake](https://cmake.org/download/).


## Building

0. Get the code from Github, or download a zip and unpack it into a folder.
1. In the Terminal or Console app of your choice, change directories (cd) into that folder.
2. `mkdir build` to create a folder with your various build files
3. `cd build` to put yourself into that folder
4. Now you can generate the projects for your choosen build environment:

### Mac 

You can build on the command line using Makefiles, or you can generate an Xcode project and use the GUI to build.

* Xcode: Run `cmake -G Xcode ..` and then open the Xcode project from this "build" folder.
* Make: Run `cmake ..` and then run `make`

### Windows

The exact command line you use will depend on what version of Visual Studio you have installed.  You can run `cmake --help` to get a list of the options available.  Assuming some version of Visual Studio 2013, the commands to generate the projects will look like this:

* 32 bit: `cmake -G "Visual Studio 12" ..`
* 64 bit: `cmake -G "Visual Studio 12 Win64" -DWIN64:Bool=True ..`

Having generated the projects, you can now build by opening the .sln file in the build folder with the Visual Studio app (just double-click the .sln file) or you can build on the command line like this:

`cmake --build . --config Release`
