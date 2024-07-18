## Set target project
file(GLOB common_src "*.h" "*.cpp")
file(GLOB grass_simulation_src "grass_simulation/*.h" "grass_simulation/*.cpp")
file(GLOB rendering_src "rendering/*.h" "rendering/*.cpp")
add_executable(grass_project 
    ${common_src} 
    ${shaders}
    ${grass_simulation_src}
    ${rendering_src})

get_target_property(MY_PROJECT_SOURCES grass_project SOURCES)
# Remove all groups
source_group(" " FILES ${MY_PROJECT_SOURCES})

# Put things in fitting groups
source_group(GrassSimulation FILES ${grass_simulation_src})
source_group(Rendering FILES ${rendering_src})
source_group(Shaders FILES ${shaders})

## set link libraries
set(libraries glad glfw imgui)

target_link_libraries(grass_project ${libraries})
## add local source directory to include paths
target_include_directories(grass_project PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# Set defines
target_compile_definitions(grass_project PUBLIC "_CRT_SECURE_NO_WARNINGS")

## copy shaders to build folder
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/blades.vert DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/blades.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR}
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/patch.vert DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/patch.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/skybox.vert DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/skybox.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/billboard.vert DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/billboard.frag DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/skybox DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/skybox2 DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
#file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/images DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

