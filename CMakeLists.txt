cmake_minimum_required(VERSION 3.14)
project(MustV2Software)

# Require C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ----------------------------------------------------------------------------
# 1. Since you're offline and already have libraries in third_party,
#    add the third_party directory to the include paths.
# ----------------------------------------------------------------------------
set(LOCAL_THIRD_PARTY_DIR "${CMAKE_CURRENT_SOURCE_DIR}/third_party")
 
# ----------------------------------------------------------------------------
# 2. Collect all .cpp files in the project (recursively)
#    Exclude files from external dependencies (e.g. _deps) and files like CMakeCXXCompilerId.cpp
# ----------------------------------------------------------------------------
file(GLOB_RECURSE ALL_SOURCE_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)
# Exclude any file that has "_deps" in its path.
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*_deps/.*")
# Exclude the CMakeCXXCompilerId.cpp file.
list(FILTER ALL_SOURCE_FILES EXCLUDE REGEX ".*CMakeCXXCompilerId\\.cpp")
set(SOURCE_FILES ${ALL_SOURCE_FILES})

# ----------------------------------------------------------------------------
# 3. Define an executable from the collected .cpp files
# ----------------------------------------------------------------------------
add_executable(${PROJECT_NAME} ${SOURCE_FILES})
find_package(Threads REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Threads::Threads)

# ----------------------------------------------------------------------------
# 4. Link libraries if needed
#    Since nlohmann/json is header-only, you don't need to link it.
# ----------------------------------------------------------------------------

# ----------------------------------------------------------------------------
# 5. Collect all .hpp files, extract their directories, and add to include path
#    Exclude files from external dependencies.
# ----------------------------------------------------------------------------
file(GLOB_RECURSE ALL_HEADER_FILES CONFIGURE_DEPENDS
    "${CMAKE_CURRENT_SOURCE_DIR}/*.hpp"
)
list(FILTER ALL_HEADER_FILES EXCLUDE REGEX ".*_deps/.*")

set(HEADER_DIRS "")
foreach(hdr ${ALL_HEADER_FILES})
    get_filename_component(hdr_dir "${hdr}" DIRECTORY)
    list(APPEND HEADER_DIRS "${hdr_dir}")
endforeach()
list(REMOVE_DUPLICATES HEADER_DIRS)

target_include_directories(${PROJECT_NAME} PRIVATE ${HEADER_DIRS})

# ----------------------------------------------------------------------------
# 6. Also add the local nlohmann directory so that #include <nlohmann/json.hpp> works.
# ----------------------------------------------------------------------------
target_include_directories(${PROJECT_NAME} PRIVATE ${LOCAL_THIRD_PARTY_DIR})

# ----------------------------------------------------------------------------
# (Optional) Specify a custom output directory for your executable
# ----------------------------------------------------------------------------
# set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

# ----------------------------------------------------------------------------
# 7. Copy external JSON files to the output directory
# ----------------------------------------------------------------------------
set(JSON_FILES
    "${CMAKE_CURRENT_SOURCE_DIR}/user_setting.json"
    "${CMAKE_CURRENT_SOURCE_DIR}/self_search_ips.json"
)
foreach(json_file ${JSON_FILES})
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${json_file}
            $<TARGET_FILE_DIR:${PROJECT_NAME}>
    )
endforeach()
