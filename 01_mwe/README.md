Prerequisities:
Installed Vulkan SDK and environment variable VULKAN_SDK pointing to
vulkan directory.

How to build on Windows:
1) >cd /path/to/01_mwe/build
2) >cmake ../ -G "MinGW Makefiles"
3) >mingw32-make
4) >example with debug symbols is built under 01_mwe/build/bin
5) run the example from root directory, otherwise it won't be able to find shader resources located in shaders folder