cmake_minimum_required(VERSION 3.0.0)
project(mochengtest)
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)
target_include_directories(native_app_glue PUBLIC
        ${ANDROID_NDK}/sources/android/native_app_glue)

find_library(log-lib
        log)

#set(MoCheng3D ${CMAKE_CURRENT_SOURCE_DIR}/../MoCheng3D)

set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS}    -u ANativeActivity_onCreate ")

add_library(mochengtest SHARED src/main/cpp/moche.cpp)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++20 -DVK_USE_PLATFORM_ANDROID_KHR")
#-DVK_NO_PROTOTYPES
#include(${MoCheng3D}/cmake/FindGLFW3.cmake)
#include(${MoCheng3D}/cmake/FindVulkan.cmake)


# imgui
#file(GLOB_RECURSE IMGUI D:/MoCheng/nativecpp/app/../MoCheng3D/imgui/ *.cpp)

#add_library(imgui ${IMGUI})

#target_link_libraries(imgui glfw Vulkan)

# source
#file(GLOB_RECURSE CPPCODE ${MoCheng3D}/src/*.cpp)
#message(AUTHOR_WARNING ${CPPCODE})
#message(AUTHOR_WARNING "here")
#add_library(cppcode ${CPPCODE})

#target_link_libraries(cppcode imgui)


#file(GLOB SRC "${MoCheng3D}/test/*.cpp")

#add_library(src ${SRC})
#include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../MoCheng3D/include ${MoCheng3D} ${CMAKE_CURRENT_SOURCE_DIR} ${ANDROID_NDK}/sources/android/native_app_glue)
#target_include_directories(cppcode PUBLIC
#        ${MoCheng3D}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/Include ${CMAKE_CURRENT_SOURCE_DIR} ${ANDROID_NDK}/sources/android/native_app_glue)
find_package(Vulkan REQUIRED)


target_link_libraries(mochengtest

        Vulkan::Vulkan
        native_app_glue
        android
        ${log-lib})