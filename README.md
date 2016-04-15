# SurgeryNavigation

Installing:

* Build and install OpenCV (debug and release)
* Edit CmakeLists.txt with required paths for OpenCV
* to generate build files, use command line syntax of CMake, e.g.:

mkdir build.debug
cd build.debug
cmake -G"Visual Studio 14 Win64" -DCMAKE_BUILD_TYPE=Debug <path_to_source>
