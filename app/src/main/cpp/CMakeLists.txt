# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.18.1)

set( LIB_FMOD ${FMOD_API_ROOT}/core/lib/${ANDROID_ABI}/libfmod${FMOD_LIB_SUFFIX}.so )
set( LIB_FMODSTUDIO ${FMOD_API_ROOT}/studio/lib/${ANDROID_ABI}/libfmodstudio${FMOD_LIB_SUFFIX}.so )

# Declares and names the project.

project("fmodtestapp")

# Creates and names a library, sets it as either STATIC
# or SHARED, and provides the relative paths to its source code.
# You can define multiple libraries, and CMake builds them for you.
# Gradle automatically packages shared libraries with your APK.

add_library( fmod SHARED IMPORTED )
set_target_properties(
        fmod PROPERTIES
        IMPORTED_LOCATION ${LIB_FMOD}
        INTERFACE_INCLUDE_DIRECTORIES ${FMOD_API_ROOT}/core/inc
)



add_library( fmodstudio SHARED IMPORTED )
set_target_properties(
        fmodstudio PROPERTIES
        IMPORTED_LOCATION ${LIB_FMODSTUDIO}
        INTERFACE_INCLUDE_DIRECTORIES ${FMOD_API_ROOT}/studio/inc
)






add_library( # Sets the name of the library.
        fmodtestapp

        # Sets the library as a shared library.
        SHARED

        # Provides a relative path to your source file(s).
        load_banks.cpp
        common.cpp
        common_platform.cpp)

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
        fmodtestapp

        fmod
        fmodstudio

        # Links the target library to the log library
        # included in the NDK.
        ${log-lib})