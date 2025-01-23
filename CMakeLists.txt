cmake_minimum_required(VERSION 3.20)
project(FlappyBird)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Установка путей к SDL2 и дополнительным библиотекам
set(SDL2_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2")
set(SDL2_IMAGE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_image")
set(SDL2_TTF_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_ttf")
set(SDL2_MIXER_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL2_mixer")

# Добавление путей для заголовочных файлов
include_directories(
        ${SDL2_DIR}/x86_64-w64-mingw32/include/SDL2
        ${SDL2_IMAGE_DIR}/x86_64-w64-mingw32/include/SDL2
        ${SDL2_TTF_DIR}/x86_64-w64-mingw32/include/SDL2
        ${SDL2_MIXER_DIR}/x86_64-w64-mingw32/include/SDL2
        ${CMAKE_SOURCE_DIR}/src
)

# Добавление путей для библиотек
link_directories(
        ${SDL2_DIR}/x86_64-w64-mingw32/lib
        ${SDL2_IMAGE_DIR}/x86_64-w64-mingw32/lib
        ${SDL2_TTF_DIR}/x86_64-w64-mingw32/lib
        ${SDL2_MIXER_DIR}/x86_64-w64-mingw32/lib
)

# Добавление исходных файлов
set(SOURCES
        src/main.cpp
        src/Game.cpp
)

set(HEADERS
        src/Game.h
)

# Создание исполняемого файла
add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS})

# Линковка библиотек
target_link_libraries(${PROJECT_NAME}
        mingw32
        SDL2main
        SDL2
        SDL2_image
        SDL2_ttf
        SDL2_mixer
)

# Копирование DLL файлов
if(WIN32)
    foreach(DLL
            "${SDL2_DIR}/x86_64-w64-mingw32/bin/SDL2.dll"
            "${SDL2_IMAGE_DIR}/x86_64-w64-mingw32/bin/SDL2_image.dll"
            "${SDL2_TTF_DIR}/x86_64-w64-mingw32/bin/SDL2_ttf.dll"
            "${SDL2_MIXER_DIR}/x86_64-w64-mingw32/bin/SDL2_mixer.dll"
    )
        if(EXISTS "${DLL}")
            file(COPY "${DLL}" DESTINATION "${CMAKE_BINARY_DIR}")
        else()
            message(WARNING "DLL not found: ${DLL}")
        endif()
    endforeach()
endif()

# Копирование ресурсов
if(EXISTS "${CMAKE_SOURCE_DIR}/assets")
    file(COPY "${CMAKE_SOURCE_DIR}/assets" DESTINATION "${CMAKE_BINARY_DIR}")
endif()