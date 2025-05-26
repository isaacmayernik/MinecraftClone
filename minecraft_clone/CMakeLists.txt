cmake_minimum_required(VERSION 3.10)
project(minecraft_clone) 

# Set paths
set(GLFW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include")
set(GLFW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glfw3.lib")
set(GLEW_INCLUDE_DIR "${CMAKE_SOURCE_DIR}/include/GL")
set(GLEW_LIBRARY "${CMAKE_SOURCE_DIR}/lib/glew32.lib")

# Add executable
file(GLOB SOURCES "src/*.c")
add_executable(minecraft_clone ${SOURCES})

# Include directories
target_include_directories(minecraft_clone PRIVATE
    ${GLFW_INCLUDE_DIR}
    ${GLEW_INCLUDE_DIR}
    ${CMAKE_SOURCE_DIR}/include
)
add_definitions(-DCGLM_HEADER_ONLY)

# Link libraries
target_link_libraries(minecraft_clone
    ${GLFW_LIBRARY}
    ${GLEW_LIBRARY}
    opengl32.lib
)

# Copy DLL to build folder
add_custom_command(TARGET minecraft_clone POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/glew32.dll"
    "$<TARGET_FILE_DIR:minecraft_clone>"
)