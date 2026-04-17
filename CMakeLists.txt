cmake_minimum_required(VERSION 3.10)

project(Audio VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)

include_directories(${PROJECT_SOURCE_DIR}/src)
include_directories(${PROJECT_SOURCE_DIR}/3rd/fftw-3.3.5)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glew-2.2.0/include)
include_directories(${PROJECT_SOURCE_DIR}/3rd/glfw-3.3.8)
include_directories(${PROJECT_SOURCE_DIR}/3rd/imgui-docking)
include_directories(${PROJECT_SOURCE_DIR}/3rd/OpenAL/include)

file(GLOB IMGUI_SRCS CONFIGURE_DEPENDS 
    ${PROJECT_SOURCE_DIR}/3rd/imgui-docking/*.cpp
    ${PROJECT_SOURCE_DIR}/3rd/imgui-docking/backends/*.cpp
)

file(GLOB SRCS CONFIGURE_DEPENDS 
    ${PROJECT_SOURCE_DIR}/src/Common/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Data/*.cpp
    ${PROJECT_SOURCE_DIR}/src/GUI/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Math/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Player/*.cpp
    ${PROJECT_SOURCE_DIR}/src/Render/*.cpp
    ${PROJECT_SOURCE_DIR}/src/*.cpp
)

add_library(opengl STATIC IMPORTED)
add_library(fftw STATIC IMPORTED)
add_library(glew STATIC IMPORTED)
add_library(glfw STATIC IMPORTED)
add_library(openal STATIC IMPORTED)

if (CMAKE_SYSTEM_NAME MATCHES "Windows")
    set_target_properties(opengl PROPERTIES IMPORTED_LOCATION opengl32.lib)    
    set_target_properties(fftw PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/3rd/fftw-3.3.5/libfftw3-3.lib)    
    set_target_properties(glew PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/3rd/glew-2.2.0/lib/glew32.lib)    
    set_target_properties(glfw PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/3rd/glfw-3.3.8/glfw3dll.lib)    
    set_target_properties(openal PROPERTIES IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/3rd/OpenAL/libs/Win64/OpenAL32.lib)
elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
    
endif()

add_library(imgui STATIC ${IMGUI_SRCS})
target_link_libraries(imgui PUBLIC opengl glew glfw)

add_executable(Audio ${SRCS})
target_compile_definitions(Audio PUBLIC SHADER_DIR=${PROJECT_SOURCE_DIR})
target_link_libraries(Audio fftw openal imgui)
