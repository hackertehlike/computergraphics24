# Install script for directory: /Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf

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
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
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

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set path to fallback-tool for dependency-resolution.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/Library/Developer/CommandLineTools/usr/bin/objdump")
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/lib" TYPE STATIC_LIBRARY FILES "/Users/aysegulbarlas/codestuff/cg/nori-base-24/build/ext_build/openexr/OpenEXR/IlmImf/libIlmImf-2_5.a")
  if(EXISTS "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIlmImf-2_5.a" AND
     NOT IS_SYMLINK "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIlmImf-2_5.a")
    execute_process(COMMAND "/Library/Developer/CommandLineTools/usr/bin/ranlib" "$ENV{DESTDIR}${CMAKE_INSTALL_PREFIX}/lib/libIlmImf-2_5.a")
  endif()
endif()

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/OpenEXR" TYPE FILE FILES
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfForward.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfExport.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfBoxAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfCRgbaFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfChannelList.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfChannelListAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfCompressionAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDoubleAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfFloatAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfFrameBuffer.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfHeader.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfIO.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfIntAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfLineOrderAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfMatrixAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfOpaqueAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfRgbaFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfStringAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfVecAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfHuf.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfWav.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfLut.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfArray.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfCompression.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfLineOrder.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfName.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfPixelType.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfVersion.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfXdr.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfConvert.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfPreviewImage.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfPreviewImageAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfChromaticities.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfChromaticitiesAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfKeyCode.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfKeyCodeAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTimeCode.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTimeCodeAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfRational.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfRationalAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfFramesPerSecond.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfStandardAttributes.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfStdIO.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfEnvmap.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfEnvmapAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfInt64.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfRgba.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTileDescription.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTileDescriptionAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTiledInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTiledOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTiledRgbaFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfRgbaYca.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTestFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfThreading.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfB44Compressor.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfStringVectorAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfMultiView.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfAcesFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfMultiPartOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfGenericOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfMultiPartInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfGenericInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfPartType.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfPartHelper.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfOutputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTiledOutputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfInputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfTiledInputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepScanLineOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepScanLineOutputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepScanLineInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepScanLineInputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepTiledInputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepTiledInputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepTiledOutputFile.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepTiledOutputPart.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepFrameBuffer.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepCompositing.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfCompositeDeepScanLine.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfNamespace.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepImageState.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfDeepImageStateAttribute.h"
    "/Users/aysegulbarlas/codestuff/cg/nori-base-24/ext/openexr/OpenEXR/IlmImf/ImfFloatVectorAttribute.h"
    )
endif()

