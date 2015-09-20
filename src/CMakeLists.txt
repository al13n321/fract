cmake_minimum_required (VERSION 3.0.0)
#cmake_policy(SET CMP0054 NEW)
project (fract)

# path to OculusVR SDK; comment to disable VR support
# currently not supported on Mac OS.
SET (OVR_SDK "C:/Program Files/OculusSDK/")

SET (EXTRA_LIBS)
SET (EXTRA_SRCS)

IF (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
  SET (MSVC 1)
ENDIF ()

IF (EXISTS "${OVR_SDK}")
  SET (USE_OVR 1)
  message("Have OVR support")

  add_definitions(-DUSE_OVR)

  include_directories(
    "${OVR_SDK}/LibOVR/Src/"
    "${OVR_SDK}/LibOVR/Include/"
  )

  link_directories(
    "${OVR_SDK}/LibOVR/Lib/x64/VS2013"
  )

  IF (MSVC)
    SET (EXTRA_LIBS ${EXTRA_LIBS} libovr64 winmm ws2_32)
  ENDIF ()

  SET (EXTRA_SRCS ${EXTRA_SRCS} application/ovr-controller.cpp)
ELSE ()
  message("No OVR support")
ENDIF ()

include_directories(
  ./
  ./lib/gl3w/include/
  ./lib/jsoncpp/include/
)

IF (MSVC)
  SET (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3 /WX /O2 /MT /D _CRT_SECURE_NO_WARNINGS")
  SET (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")

  include_directories(
    ./lib/glfw-3.0.4.bin.WIN64/include/
  )

  link_directories(
    ./lib/glfw-3.0.4.bin.WIN64/lib-msvc120/
  )
ELSE ()
  SET (CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -O3")

  include_directories(
    /opt/local/include/
  )

  link_directories(
    /opt/local/lib/
  )
ENDIF ()

IF (APPLE)
  FIND_LIBRARY(COCOA_LIBRARY Cocoa)
  FIND_LIBRARY(OPENGL_LIBRARY OpenGL)
  FIND_LIBRARY(IOKIT_LIBRARY IOKit)
  FIND_LIBRARY(COREVIDEO_LIBRARY CoreVideo)
  FIND_LIBRARY(CORESERVICES_LIBRARY CoreServices)

  MARK_AS_ADVANCED (COCOA_LIBRARY
                    OPENGL_LIBRARY
                    IOKIT_LIBRARY
                    COREVIDEO_LIBRARY
                    CORESERVICES_LIBRARY)

  SET (EXTRA_LIBS ${EXTRA_LIBS}
     ${COCOA_LIBRARY}
     ${OPENGL_LIBRARY}
     ${IOKIT_LIBRARY}
     ${COREVIDEO_LIBRARY}
     ${CORESERVICES_LIBRARY})
ENDIF (APPLE)

add_executable(fract
  application/experimentation-application.cpp
  #application/monitest.cpp
  #application/vrtest.cpp
  application/normal-controller.cpp
  application/experimental-cpu-raytracer.cpp
  gl-util/framebuffer.cpp
  gl-util/gl-common.cpp
  gl-util/glfw-util.cpp
  gl-util/quad-renderer.cpp
  gl-util/shader.cpp
  gl-util/texture2d.cpp
  raytracer/raytracer.cpp
  raytracer/renderer.cpp
  resources/camera.cpp
  resources/config.cpp
  resources/shader-preprocessor.cpp
  resources/shader-provider.cpp
  util/debug.cpp
  util/exceptions.cpp
  util/file-watcher.cpp
  util/json.cpp
  util/logging.cpp
  util/mat.cpp
  util/stopwatch.cpp
  util/string-util.cpp
  lib/jsoncpp/src/jsoncpp.cpp
  lib/gl3w/src/gl3w.c
  ${EXTRA_SRCS}
)

IF (MSVC)
  target_link_libraries(fract
    glfw3
    OpenGL32
    ${EXTRA_LIBS}
  )
ELSE ()
  target_link_libraries(fract
    glfw
    ${EXTRA_LIBS}
  )
ENDIF ()
