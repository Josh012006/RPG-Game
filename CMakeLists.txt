cmake_minimum_required(VERSION 3.16)
project(Oathbound LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(OATHBOUND_BUILD_GAME  "Build the SFML game"  ON)
option(OATHBOUND_BUILD_TESTS "Build the unit tests" ON)

# Release by default (as before), but no longer forced: -DCMAKE_BUILD_TYPE=Debug works.
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

# Binaries go to bin/ (the game is run from the project root, see README)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY         ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG   ${CMAKE_SOURCE_DIR}/bin)

# ------------------------------------------------------------------------------
# nlohmann/json: system package if available, bundled copy otherwise
# ------------------------------------------------------------------------------
add_library(oathbound_json INTERFACE)
find_package(nlohmann_json 3.2.0 QUIET)
if(nlohmann_json_FOUND)
    target_link_libraries(oathbound_json INTERFACE nlohmann_json::nlohmann_json)
else()
    target_include_directories(oathbound_json SYSTEM INTERFACE ${CMAKE_SOURCE_DIR}/third_party/include)
endif()

# ------------------------------------------------------------------------------
# oathbound_core: game rules, data and saves. NO SFML here, so it builds and is
# tested without a window or a graphics driver.
# ------------------------------------------------------------------------------
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/core/*.cpp)
add_library(oathbound_core STATIC ${CORE_SOURCES})
target_include_directories(oathbound_core PUBLIC ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(oathbound_core PUBLIC oathbound_json)
if(MSVC)
    target_compile_options(oathbound_core PRIVATE /W4)
else()
    target_compile_options(oathbound_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

# ------------------------------------------------------------------------------
# The game: engine (window, states, assets) + states (screens)
# ------------------------------------------------------------------------------
if(OATHBOUND_BUILD_GAME)
    file(GLOB_RECURSE GAME_SOURCES CONFIGURE_DEPENDS src/engine/*.cpp src/states/*.cpp)
    add_executable(oathbound src/main.cpp ${GAME_SOURCES})
    target_link_libraries(oathbound PRIVATE oathbound_core)

    if(WIN32 AND EXISTS ${CMAKE_SOURCE_DIR}/lib)
        # Bundled SFML (headers in third_party/include/SFML, import libs and DLLs in lib/ and bin/)
        target_include_directories(oathbound SYSTEM PRIVATE ${CMAKE_SOURCE_DIR}/third_party/include)
        target_link_directories(oathbound PRIVATE ${CMAKE_SOURCE_DIR}/lib)
        target_link_libraries(oathbound PRIVATE sfml-graphics sfml-window sfml-system opengl32)
    else()
        find_package(SFML 2.6 COMPONENTS graphics window system REQUIRED)
        target_link_libraries(oathbound PRIVATE sfml-graphics sfml-window sfml-system)
    endif()
endif()

# ------------------------------------------------------------------------------
# Tests (ctest)
# ------------------------------------------------------------------------------
if(OATHBOUND_BUILD_TESTS)
    enable_testing()
    file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
    add_executable(oathbound_tests ${TEST_SOURCES})
    target_include_directories(oathbound_tests PRIVATE ${CMAKE_SOURCE_DIR}/tests)
    target_link_libraries(oathbound_tests PRIVATE oathbound_core)
    target_compile_definitions(oathbound_tests PRIVATE
        "OATHBOUND_DATA_DIR=\"${CMAKE_SOURCE_DIR}/data\""
        "OATHBOUND_FIXTURES_DIR=\"${CMAKE_SOURCE_DIR}/tests/fixtures\"")
    add_test(NAME oathbound_tests COMMAND oathbound_tests)
endif()
