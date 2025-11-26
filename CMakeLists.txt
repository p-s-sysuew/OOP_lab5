cmake_minimum_required(VERSION 3.10)
project(lab_5 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Предупреждения
if (MSVC)
    add_compile_options(/W4)
else()
    add_compile_options(-Wall -Wextra -pedantic)
endif()

# Библиотека
add_library(${CMAKE_PROJECT_NAME}_lib
    src/functions.cpp
)

# Указываем папку с заголовками
target_include_directories(${CMAKE_PROJECT_NAME}_lib PUBLIC
    ${PROJECT_SOURCE_DIR}/include
)

# Исполняемый файл
add_executable(${CMAKE_PROJECT_NAME}_exe main.cpp)

# Линкуем
target_link_libraries(${CMAKE_PROJECT_NAME}_exe PRIVATE
    ${CMAKE_PROJECT_NAME}_lib
)
