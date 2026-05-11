cmake_minimum_required(VERSION 3.10)
project(SuperPotooWorld C)

set(CMAKE_C_STANDARD 11)

# Dossiers d'inclusion
include_directories(
    include
    include/Input
    include/Scene
    include/Utils
    include/GameObject
    include/GameObject/Camera
    include/GameObject/Collectable
    include/GameObject/Enemy
    include/GameObject/SceneManager
    include/GameObject/Terrain
    include/GameObject/UI
    include/GameObject/UI/LevelUI
    include/GameObject/UI/TitleUI
    external/OOC
    external/PlatformerEngine
    external/RenderingEngine
)

# Sources
file(GLOB_RECURSE SOURCES 
    "src/*.c"
    "external/OOC/*.c"
)

# Find SDL2
find_package(SDL2 REQUIRED)
find_package(SDL2_image REQUIRED)
find_package(SDL2_mixer REQUIRED)
find_package(SDL2_ttf REQUIRED)

# Find OpenMP
find_package(OpenMP REQUIRED)

# Executable
add_executable(SuperPotooWorld ${SOURCES})

# OpenMP
if(OpenMP_C_FOUND)
    target_link_libraries(SuperPotooWorld PUBLIC OpenMP::OpenMP_C)
endif()

# Cibles SDL2 (Compatibilité vcpkg / apt)
if(TARGET SDL2::SDL2)
    target_link_libraries(SuperPotooWorld PRIVATE SDL2::SDL2)
else()
    target_link_libraries(SuperPotooWorld PRIVATE ${SDL2_LIBRARIES})
endif()

if(TARGET SDL2_image::SDL2_image)
    target_link_libraries(SuperPotooWorld PRIVATE SDL2_image::SDL2_image)
else()
    target_link_libraries(SuperPotooWorld PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

if(TARGET SDL2_mixer::SDL2_mixer)
    target_link_libraries(SuperPotooWorld PRIVATE SDL2_mixer::SDL2_mixer)
else()
    target_link_libraries(SuperPotooWorld PRIVATE ${SDL2_MIXER_LIBRARIES})
endif()

if(TARGET SDL2_ttf::SDL2_ttf)
    target_link_libraries(SuperPotooWorld PRIVATE SDL2_ttf::SDL2_ttf)
else()
    target_link_libraries(SuperPotooWorld PRIVATE ${SDL2_TTF_LIBRARIES})
endif()

# Bibliothèques externes (PE et RE)
# Note: Ces bibliothèques doivent être compilées pour la plateforme cible.
if(WIN32)
    target_link_libraries(SuperPotooWorld PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/external/PlatformerEngine/PE.lib
        ${CMAKE_CURRENT_SOURCE_DIR}/external/RenderingEngine/RE.lib
    )
    if(TARGET SDL2::SDL2main)
        target_link_libraries(SuperPotooWorld PRIVATE SDL2::SDL2main)
    endif()
else()
    target_link_libraries(SuperPotooWorld PRIVATE 
        ${CMAKE_CURRENT_SOURCE_DIR}/external/PlatformerEngine/libPE.a
        ${CMAKE_CURRENT_SOURCE_DIR}/external/RenderingEngine/libRE.a
        m
    )
endif()

# Copie des Assets vers le dossier de build
add_custom_command(TARGET SuperPotooWorld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    ${CMAKE_SOURCE_DIR}/Assets
    $<TARGET_FILE_DIR:SuperPotooWorld>/Assets
)

# Copie des musiques (qui étaient dans src/)
add_custom_command(TARGET SuperPotooWorld POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    ${CMAKE_SOURCE_DIR}/src/MainTheme.mp3
    ${CMAKE_SOURCE_DIR}/src/Boss.mp3
    $<TARGET_FILE_DIR:SuperPotooWorld>/
)
