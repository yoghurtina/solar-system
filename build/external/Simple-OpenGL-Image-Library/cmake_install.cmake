# Install script for directory: /home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "0")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/home/ioanna/Desktop/reportsGraphics/SolarSystem/build/external/Simple-OpenGL-Image-Library/libSOIL.a")
endif()

if("x${CMAKE_INSTALL_COMPONENT}x" STREQUAL "xUnspecifiedx" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/SOIL" TYPE FILE FILES
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/SOIL.h"
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/image_DXT.h"
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/image_helper.h"
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/stb_image_aug.h"
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug.h"
    "/home/ioanna/Desktop/reportsGraphics/SolarSystem/external/Simple-OpenGL-Image-Library/include/stbi_DDS_aug_c.h"
    )
endif()

