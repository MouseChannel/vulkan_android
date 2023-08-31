cmake_minimum_required(VERSION 3.0.0)
project(mochengtest)
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

find_library(log-lib
        log)
 #--
find_package(Vulkan REQUIRED)

file(GLOB_RECURSE MoCheng3D ${CMAKE_CURRENT_SOURCE_DIR}/src/main/src *.cpp)
add_library(mocheng3d ${MoCheng3D})
target_link_libraries(mocheng3d Vulkan::Vulkan)



#--



set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}    -u ANativeActivity_onCreate ")

add_library(mochengtest SHARED ${CMAKE_CURRENT_SOURCE_DIR}/src/main/cpp/moche.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -DVK_USE_PLATFORM_ANDROID_KHR")
#-DVK_NO_PROTOTYPES

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${ANDROID_NDK}/sources/android/native_app_glue
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/include
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/include/Tool
        ${CMAKE_CURRENT_SOURCE_DIR}/src/main/include/vulkan_header
        )



target_link_libraries(mochengtest
        mocheng3d
        native_app_glue
        android
        ${log-lib})