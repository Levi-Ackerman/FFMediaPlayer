# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

include_directories(include ./ ffplayer/)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/../jniLibs/${ANDROID_ABI} -std=c++11 ")

set(CMAKE_ANDROID_NDK "/Users/bytedance/bytedance/android-ndk-r14b")

file(GLOB src_files *.cpp ffplayer/*.cpp)


# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( # Sets the name of the library.
        nativeffmpeg

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        #native-lib.cpp
        ${src_files})

# Searches for a specified prebuilt library and stores the path as a
# variable. Because CMake includes system libraries in the search path by
# default, you only need to specify the name of the public NDK library
# you want to add. CMake verifies that the library exists before
# completing its build.

find_library( # Sets the name of the path variable.
        log-lib

        # Specifies the name of the NDK library that
        # you want CMake to locate.
        log)

# Specifies libraries CMake should link to your target library. You
# can link multiple libraries, such as libraries you define in this
# build script, prebuilt third-party libraries, or system libraries.

target_link_libraries( # Specifies the target library.
        nativeffmpeg

        -Wl,--start-group
        avcodec-58
        avdevice-58
        avfilter-7
        avformat-58
        postproc-55
        swresample-3
        swscale-5
        avutil-56
        -Wl,--end-group

        android
        OpenSLES

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})

#FIND_PACKAGE(OpenMP REQUIRED)
#if (OPENMP_FOUND)
#    message("OPENMP FOUND ${OpenMP_CXX_FLAGS}")
#    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
#    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
#    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
#endif ()