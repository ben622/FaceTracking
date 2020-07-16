# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.4.1)

set(PRELIB_ROOT ${CMAKE_SOURCE_DIR})

message(${PRELIB_ROOT})

INCLUDE_DIRECTORIES(${PRELIB_ROOT}/include)

ADD_LIBRARY(opencv
        SHARED
        IMPORTED)

SET_TARGET_PROPERTIES(opencv
        PROPERTIES
        IMPORTED_LOCATION
        ${PRELIB_ROOT}/libs/${ANDROID_ABI}/libopencv_java3.so)



add_library( # Sets the name of the library.
            facetracking
             # Sets the library as a shared library.
             SHARED
             # Provides a relative path to your source file(s).
            ${PRELIB_ROOT}/native-lib.cpp)


find_library( # Sets the name of the path variable.
              log-lib
              # Specifies the name of the NDK library that
              # you want CMake to locate.
              log )

target_link_libraries( # Specifies the target library.
                        facetracking
                       ${log-lib}
                        opencv
                        android)