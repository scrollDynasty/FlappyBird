cmake_minimum_required(VERSION 3.25)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)

# Пути к SDL2 и SDL2_ttf
set(SDL2_PATH "${CMAKE_SOURCE_DIR}/SDL2/x86_64-w64-mingw32")
set(SDL2_TTF_PATH "${CMAKE_SOURCE_DIR}/SDL2_ttf/x86_64-w64-mingw32")

# Включаем заголовочные файлы
include_directories(
        ${SDL2_PATH}/include/SDL2
        ${SDL2_TTF_PATH}/include/SDL2
        ${PROJECT_SOURCE_DIR}/src
)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/Game.cpp
)

# Подключаем библиотеки
target_link_libraries(${PROJECT_NAME}
        mingw32
        ${SDL2_PATH}/lib/libSDL2main.a
        ${SDL2_PATH}/lib/libSDL2.dll.a
        ${SDL2_PATH}/lib/libSDL2_image.dll.a
        ${SDL2_TTF_PATH}/lib/libSDL2_ttf.dll.a
)

# Копируем необходимые DLL файлы (убрали zlib1.dll)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
        ${SDL2_PATH}/bin/SDL2.dll
        ${SDL2_PATH}/bin/SDL2_image.dll
        ${SDL2_TTF_PATH}/bin/SDL2_ttf.dll
        ${CMAKE_BINARY_DIR}
)

# Копируем папку assets
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/assets
        ${CMAKE_BINARY_DIR}/assets
)