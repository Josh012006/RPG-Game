cmake_minimum_required(VERSION 3.16)
project(MyRPG)

# Standard C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Récupération automatique des fichiers source
file(GLOB_RECURSE SRC_FILES CONFIGURE_DEPENDS src/*.cpp)

# Création de l'exécutable
add_executable(main ${SRC_FILES})

# Include (headers)
target_include_directories(main PRIVATE include)

# ==============================
# SFML (Windows manuel)
# ==============================
if (WIN32)

    target_include_directories(main PRIVATE include)
    target_link_directories(main PRIVATE lib)

    target_link_libraries(main
            sfml-graphics
            sfml-window
            sfml-system
            opengl32
    )

endif()

# ==============================
# SFML (Linux / macOS via pkg-config)
# ==============================
if (UNIX AND NOT APPLE)

    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SFML REQUIRED sfml-graphics sfml-window sfml-system)

    target_include_directories(main PRIVATE ${SFML_INCLUDE_DIRS})
    target_link_libraries(main ${SFML_LIBRARIES})

endif()