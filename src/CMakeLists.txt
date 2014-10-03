cmake_minimum_required (VERSION 3.0.0)
project (fract)

SET( CMAKE_CXX_FLAGS  "${CMAKE_CXX_FLAGS} -std=c++11 -Wall -O3" )

include_directories(.)

add_executable(fract
	application/experimentation-application.cpp
	cpu-raytracers/test-raytracers.cpp
	gl-util/quad-renderer.cpp
	gl-util/shader.cpp
	gl-util/texture2d.cpp
	raytracer/raytracing-engine.cpp
	raytracer/renderer.cpp
	util/camera.cpp
	util/mat.cpp
	util/stopwatch.cpp
)
