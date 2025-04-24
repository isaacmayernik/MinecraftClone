cmake_minimum_required(VERSION 3.10)
project(minecraft_clone)  # Matches your folder name

# Set paths
set(PROJECT_SOURCE_DIR "${CMAKE_SOURCE_DIR}/minecraft_clone")
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/GLFW")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")
set(GLEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/GL")
set(GLEW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glew32.lib")

# Add executable
add_executable(minecraft_clone 
    "${PROJECT_SOURCE_DIR}/src/main.c"
    "${PROJECT_SOURCE_DIR}/src/camera.c"
) 

# Include directories
target_include_directories(minecraft_clone PRIVATE
    "${PROJECT_SOURCE_DIR}/include"
    "${PROJECT_SOURCE_DIR}/src"
    ${GLFW_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(minecraft_clone
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    opengl32.lib
)

# Copy DLL to build folder (for GLEW)
add_custom_command(TARGET minecraft_clone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${PROJECT_SOURCE_DIR}/glew32.dll"
    "$<TARGET_FILE_DIR:minecraft_clone>"
)