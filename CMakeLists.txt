cmake_minimum_required(VERSION 3.12)
project(GameProject1 CXX)

# Set the SDL2_DIR variable to the directory containing the config file
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/lib/cmake/SDL2")

set(SDL2_IMAGE_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/include")
set(SDL2_IMAGE_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/lib/libSDL2_image.dll.a")
# Manually set up SDL2_ttf
set(SDL2_TTF_INCLUDE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_ttf/include")
set(SDL2_TTF_LIBRARY "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_ttf/lib/libSDL2_ttf.dll.a")

find_package(SDL2 REQUIRED)

add_executable(SDL2Demo 
src/main.cpp
src/basicEntity.cpp
src/renderPipeline.cpp
src/environmentObject.cpp
src/advancedObjects.cpp
src/uiObjects.cpp
src/enemySpawner.cpp
) 

set_target_properties(SDL2Demo PROPERTIES WIN32_EXECUTABLE FALSE)
target_include_directories(SDL2Demo PRIVATE 
    ${SDL2_IMAGE_INCLUDE_DIR} 
    ${SDL2_TTF_INCLUDE_DIR}
)
target_link_options(SDL2Demo PRIVATE -mconsole)
target_link_libraries(SDL2Demo PRIVATE 
    mingw32 
    SDL2::SDL2main 
    ${SDL2_IMAGE_LIBRARY}  # Extensions first
    ${SDL2_TTF_LIBRARY} 
    SDL2::SDL2             # Base library last
)

# Copy the assets folder to the build directory automatically
add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_SOURCE_DIR}/assets"
    "$<TARGET_FILE_DIR:SDL2Demo>/assets"
)

# Copy SDL2 and extension DLLs to the output directory
add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2/bin/SDL2.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/SDL2.dll"
)

add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/SDL2_image.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/SDL2_image.dll"
)
add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_ttf/bin/SDL2_ttf.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/SDL2_ttf.dll"
)

# Copy SDL2_image dependency DLLs to the output directory
add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/libpng16-16.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/libpng16-16.dll"
)

add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/zlib1.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/zlib1.dll"
)

add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/libjpeg-9.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/libjpeg-9.dll"
)

add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/libtiff-5.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/libtiff-5.dll"
)

add_custom_command(TARGET SDL2Demo POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_CURRENT_SOURCE_DIR}/libs/SDL2_image/bin/libwebp-7.dll"
    "$<TARGET_FILE_DIR:SDL2Demo>/libwebp-7.dll"
)