cmake_minimum_required(VERSION 3.23)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_C_STANDARD_REQUIRED True)

if(MSVC)
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Release>:Release>")
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
    add_compile_options(/arch:AVX2) # Ensure SIMD optimizations
    add_compile_options(/arch:DDEBUG)
    add_compile_options("$<$<CONFIG:Release>:/O2>")

endif()


set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE) # Link time optimization

project(Dyl_Engine)
file(GLOB_RECURSE MY_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.c")
add_compile_definitions(USING_SDL)
add_library("${CMAKE_PROJECT_NAME}" SHARED ${MY_SOURCES})






target_compile_definitions(Dyl_Engine PRIVATE ENGINE_EXPORTS)



set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
# Import libraries (.lib) go to lib/
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

target_include_directories("${CMAKE_PROJECT_NAME}" PRIVATE
    "${CMAKE_CURRENT_SOURCE_DIR}/../include"
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/freetype"

)

# Link SDL3 and other libraries
target_link_libraries("${CMAKE_PROJECT_NAME}" PRIVATE
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/SDL3.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/freetype.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/brotlicommon.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/brotlidec.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/bz2.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/libpng16.lib"
	"${CMAKE_CURRENT_SOURCE_DIR}/../libs/zlib.lib"
)
