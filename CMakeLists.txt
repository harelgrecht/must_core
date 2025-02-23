cmake_minimum_required(VERSION 3.14)
project(NetworkManagerProject)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set the output directories
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# Integrate the nlohmann JSON library using FetchContent
include(FetchContent)
FetchContent_Declare(
  nlohmann_json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG v3.11.3  # You can adjust the tag/version as needed
)
FetchContent_MakeAvailable(nlohmann_json)

# Recursively find all .cpp files in module*/src directories
file(GLOB_RECURSE MODULE_SOURCES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*/*.cpp"
)

# Optionally include a top-level main.cpp if present
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
    list(APPEND MODULE_SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/main.cpp")
endif()

# Create the executable target from the gathered sources
add_executable(${PROJECT_NAME} ${MODULE_SOURCES})

# Automatically collect include directories from module*/include folders
file(GLOB MODULE_INCLUDE_DIRS CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*/*.hpp"
)
list(REMOVE_DUPLICATES MODULE_INCLUDE_DIRS)
target_include_directories(${PROJECT_NAME} PUBLIC ${MODULE_INCLUDE_DIRS})

# Link the nlohmann JSON library (nlohmann_json is header-only, but linking the target is recommended)
target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json)
