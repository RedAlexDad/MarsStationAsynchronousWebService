cmake_minimum_required(VERSION 3.26)
project(MarsStationAsynchronousWebService)

set(CMAKE_CXX_STANDARD 14)

# Ищем библиотеки
find_package(PkgConfig REQUIRED)
pkg_search_module(JSONCPP REQUIRED jsoncpp)

include_directories(${JSONCPP_INCLUDE_DIRS})

# Добавляем исполняемый файл
add_executable(
        MarsStationAsynchronousWebService
        Asynchronous.cpp
        Asynchronous.h
        main.cpp
)

# Подключаем заголовочные файлы
target_include_directories(
        MarsStationAsynchronousWebService
        PRIVATE
        ${nlohmann_json_INCLUDE_DIRS}
        ${CURL_INCLUDE_DIRS}
        ${jsoncpp_INCLUDE_DIRS}
        ${TABULATE_INCLUDE_DIR}
)

# Линкуем программу с библиотеками
target_link_libraries(
        MarsStationAsynchronousWebService
        PRIVATE ${JSONCPP_LIBRARIES}
        curl
)
