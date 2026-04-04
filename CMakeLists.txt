cmake_minimum_required(VERSION 3.16)
project(Oathbound)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# ==============================
# Build en Release uniquement et sortie dans bin
# ==============================
set(CMAKE_CONFIGURATION_TYPES Release CACHE STRING "" FORCE)
set(CMAKE_BUILD_TYPE Release CACHE STRING "Release build only" FORCE)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_SOURCE_DIR}/bin)

# ==============================
# Récupération des fichiers source
# ==============================
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# Création de l'exécutable
add_executable(main ${SRC_FILES})

# Include (headers)
target_include_directories(main PRIVATE include)

# ==============================
# SFML Windows
# ==============================
if (WIN32)
    target_include_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/include)
    target_link_directories(main PRIVATE ${CMAKE_SOURCE_DIR}/lib)

    target_link_libraries(main
            sfml-graphics
            sfml-window
            sfml-system
            opengl32
    )
endif()

# ==============================
# SFML Linux / macOS
# ==============================
if (UNIX)
    find_package(SFML 2.6.2 COMPONENTS graphics window system REQUIRED)
    target_include_directories(main PRIVATE ${SFML_INCLUDE_DIR})
    target_link_libraries(main ${SFML_LIBRARIES})
endif()