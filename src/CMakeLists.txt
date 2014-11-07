cmake_minimum_required (VERSION 3.0.0)
project (fract)

IF("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	SET(MSVC 1)
ENDIF ()

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS}" )
include_directories(
	./
)
	
IF(MSVC)
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} /W3 /WX /O2 /MT" )

	include_directories(
		./lib/glfw-3.0.4.bin.WIN64/include/
		./lib/
		./lib/jsoncpp/include/
	)

	link_directories(
		./lib/glfw-3.0.4.bin.WIN64/lib-msvc120/
		./lib/jsoncpp/lib/
	)
ELSE ()
	SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -Werror -O3" )

	include_directories(
		/opt/local/include/
	)

	link_directories(
		/opt/local/lib/
	)
ENDIF ()

SET(EXTRA_LIBS)

IF(APPLE)
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

  SET(EXTRA_LIBS ${EXTRA_LIBS}
   	${COCOA_LIBRARY}
   	${OPENGL_LIBRARY}
   	${IOKIT_LIBRARY}
   	${COREVIDEO_LIBRARY}
   	${CORESERVICES_LIBRARY})
ENDIF (APPLE)

add_executable(fract
	application/experimentation-application.cpp
	gl-util/gl-common.cpp
	gl-util/framebuffer.cpp
	gl-util/quad-renderer.cpp
	gl-util/shader.cpp
	gl-util/texture2d.cpp
	raytracer/raytracer.cpp
	raytracer/raytracing-engine.cpp
	raytracer/renderer.cpp
	resources/config.cpp
	resources/shader-preprocessor.cpp
	resources/shader-provider.cpp
	util/camera.cpp
	util/exceptions.cpp
	util/file-watcher.cpp
	util/json.cpp
	util/logging.cpp
	util/mat.cpp
	util/stopwatch.cpp
	util/string-util.cpp
)

if(MSVC)
	target_link_libraries(fract
		glfw3
		lib_json
		${EXTRA_LIBS}
	)
ELSE ()
	target_link_libraries(fract
		glfw
		jsoncpp
		${EXTRA_LIBS}
	)
ENDIF ()
