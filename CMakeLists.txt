# ---------------------------------------------------------------------------------
# Projects Settings
# ---------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.3)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)


MACRO(SUBDIRLIST result curdir)
    FILE(GLOB children RELATIVE ${curdir} ${curdir}/*)
    SET(dirlist "")
    FOREACH(child ${children})
        IF(IS_DIRECTORY ${curdir}/${child})
            LIST(APPEND dirlist ${child})
        ENDIF()
    ENDFOREACH()
    SET(${result} ${dirlist})
ENDMACRO()

    set(EXTERNAL_LIBRARIES_SOURCE_PATH ${CMAKE_SOURCE_DIR}/common/submodules)
set(EXTERNAL_LIBRARIES_BUILD_PATH ${CMAKE_BINARY_DIR}/common/submodules)

# ---------------------------------------------------------------------------------
# Cmake projects
# ---------------------------------------------------------------------------------
project(GrassProject)

# static libraries
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad)
add_subdirectory(${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui)


set(glfwDir ${EXTERNAL_LIBRARIES_BUILD_PATH}/glfw/src CACHE PATH "Location of the glfw lib")
set(gladDir ${EXTERNAL_LIBRARIES_BUILD_PATH}/glad/src CACHE PATH "Location of the glad lib")
set(imguiDir ${EXTERNAL_LIBRARIES_BUILD_PATH}/imgui/src CACHE PATH "Location of the imgui lib")
link_directories(${gladDir} ${glfwDir} ${imguiDir})

include_directories(
        include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glad/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glfw//include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/imgui/include
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/glm
        ${EXTERNAL_LIBRARIES_SOURCE_PATH}/STBImage
        )

file(GLOB shaders "assets/shaders/*")


# project
add_subdirectory(${CMAKE_SOURCE_DIR}/grass_project)

set_target_properties(
    grass_project PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}")
