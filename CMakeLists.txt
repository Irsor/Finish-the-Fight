cmake_minimum_required(VERSION 3.30)
project(Finish-the-Fight)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(FetchContent)

# Vulkan SDK
find_package(Vulkan REQUIRED)

# Vulkan‑Hpp
FetchContent_Declare(
        vulkan-hpp
        GIT_REPOSITORY https://github.com/KhronosGroup/Vulkan-Hpp.git
        GIT_TAG        v1.4.312
)

FetchContent_MakeAvailable(vulkan-hpp)

# glfw
FetchContent_Declare(
        glfw
        GIT_REPOSITORY https://github.com/glfw/glfw.git
        GIT_TAG        3.4
)

FetchContent_MakeAvailable(glfw)

# glm
FetchContent_Declare(
        glm
        GIT_REPOSITORY https://github.com/g-truc/glm.git
        GIT_TAG        1.0.1
)

FetchContent_MakeAvailable(glm)

add_executable(${PROJECT_NAME} main.cpp
    src/App.cpp
    src/PhysicalDevice.cpp
    src/Queue.cpp
    src/Window.cpp
    src/Swapchain.cpp
    src/Pipeline.cpp
    src/Texture.cpp
    src/Utils/FileUtils.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/include
        ${vulkan-hpp_SOURCE_DIR}/include
        ${glm_SOURCE_DIR}
)

target_link_libraries(${PROJECT_NAME} PRIVATE
        Vulkan::Vulkan
        glfw
)
