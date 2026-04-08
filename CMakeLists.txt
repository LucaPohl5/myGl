cmake_minimum_required(VERSION 3.10)

project(myGl)


#import glfw
add_library(glfw STATIC IMPORTED)
set_target_properties(glfw PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/dependencies/library/libglfw.3.4.dylib
)
target_include_directories(glfw
        INTERFACE
        ${CMAKE_SOURCE_DIR}/dependencies/include
)

#import assimp
add_library(assimp STATIC IMPORTED)
set_target_properties(assimp PROPERTIES
        IMPORTED_LOCATION
        ${CMAKE_SOURCE_DIR}/dependencies/library/libassimp.6.0.4.dylib
        ${CMAKE_SOURCE_DIR}/dependencies/library/libassimp.6.dylib
        ${CMAKE_SOURCE_DIR}/dependencies/library/libassimp.dylib
)
target_include_directories(assimp
        INTERFACE
        ${CMAKE_SOURCE_DIR}/dependencies/include)



#create exe for tests
#add_executable(myGlTest
#        src/main.cpp

#add lib for release
add_library(myGl
        src/Window.h
        src/Window.cpp
        src/Camera.h
        src/Camera.cpp
        src/glad.c
        src/Shader.h
        src/Shader.cpp
        src/myGl.h
        src/EventHandler.cpp
        src/EventHandler.h
        src/Model.cpp
        src/Model.h
        src/Renderer.cpp
        src/Renderer.h
)

if(MSVC)
    target_compile_options(myGl PRIVATE /W4 /WX)
else()
    target_compile_options(myGl PRIVATE -Wall -Wextra -Wpedantic -v)
endif()


set_target_properties(myGl PROPERTIES
        CXX_STANDARD 17
        CXX_STANDARD_REQUIRED YES
        CXX_EXTENSIONS NO)


#link libraries
target_link_libraries(myGl
        PRIVATE glfw
        PRIVATE assimp
)