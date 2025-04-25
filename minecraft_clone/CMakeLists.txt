cmake_minimum_required(VERSION 3.10)
set(CMAKE_GENERATOR_TOOLSET "v143" CACHE STRING "Force VS 2022 toolchain")

project(minecraft_clone)

# Set paths
set(GLFW_INCLUDE_DIR "include/GLFW")
set(GLFW_LIBRARY "lib/glfw3.lib")
set(GLEW_INCLUDE_DIR "include/GL")
set(GLEW_LIBRARY "lib/glew32.lib")

# Add all source files
add_executable(minecraft_clone
    src/main.c
    src/camera.c
)

# Include directories
target_include_directories(minecraft_clone PRIVATE
    include
    src
    ${GLFW_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
)

# Link libraries
target_link_libraries(minecraft_clone
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    opengl32.lib
)

add_custom_command(TARGET minecraft_clone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "glew32.dll"
    "$<TARGET_FILE_DIR:minecraft_clone>"
)