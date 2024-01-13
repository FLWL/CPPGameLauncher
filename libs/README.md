Step 1: Download and Build Dependencies
OpenSSL:

Download OpenSSL from the official website: OpenSSL Downloads.
Follow the instructions in the OpenSSL documentation for building on your platform.
zlib:

Download zlib from the official website: zlib Home Page.
Follow the instructions in the zlib documentation for building on your platform.
libcurl:

Download libcurl from the official website: libcurl Downloads.
Follow the instructions in the libcurl documentation for building on your platform.
Step 2: Update CMakeLists.txt
Update your CMakeLists.txt file to include the necessary libraries:


#-------------
cmake_minimum_required(VERSION 3.15)

project(CPPGameLauncher)

set(CMAKE_CXX_STANDARD 20)

# ...

# OpenSSL, zlib, and libcurl include directories
target_include_directories(${PROJECT_NAME} PRIVATE
    libs/OpenSSL-Win64/include
    libs/zlib/include
    libs/curl/include
)

# OpenSSL, zlib, and libcurl link directories
target_link_directories(${PROJECT_NAME} PRIVATE
    libs/OpenSSL-Win64/lib/VC/x64/MT
    libs/zlib/bin
    libs/zlib/lib
    libs/curl/lib
)

# Link libraries
target_link_libraries(${PROJECT_NAME} PRIVATE
    libssl.lib
    libcrypto.lib
    libssl_static.lib
    zlibstatic.lib
    libcurl.lib
)

# ...
Step 3: Build Your Project
Now, open a terminal and navigate to your project directory:

bash example: powershell vs2022
mkdir build
cd build
cmake -A Win32 ..
This will generate the necessary build files.

Step 4: Readme.txt
Create a readme.txt file with instructions for building the project. Include the steps for downloading and building the dependencies and building the project using CMake.
