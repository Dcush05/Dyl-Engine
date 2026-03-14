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

add_executable(Dyl_Engine main.c)
set(ENGINE_LIB_PATH "../../Engine/build/$<CONFIG>/Dyl_Engine")
target_link_libraries(Dyl_Engine PRIVATE "${ENGINE_LIB_PATH}")
target_include_directories(Dyl_Engine PRIVATE 
	"${CMAKE_SOURCE_DIR}/../include" 
	"${CMAKE_SOURCE_DIR}/../include/freetype"
	"${CMAKE_SOURCE_DIR}/../Engine/src")

add_custom_command(TARGET Dyl_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
    "../../Engine/build/$<CONFIG>/Dyl_Engine.dll"
    $<TARGET_FILE_DIR:Dyl_Engine>
)
add_custom_command(TARGET Dyl_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"../../bin/SDL3.dll"
    $<TARGET_FILE_DIR:Dyl_Engine>
)
add_custom_command(TARGET Dyl_Engine POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
	"../../bin/brotlicommon.dll"
    $<TARGET_FILE_DIR:Dyl_Engine>
)




