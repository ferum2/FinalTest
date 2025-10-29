cmake_minimum_required(VERSION 3.20)
add_executable(FinalTestProject main.cpp)
project(FinalTestProject)
find_package(SQLite3 REQUIRED)
set(SQLite3_DIR "/opt/homebrew/opt/sqlite/lib/cmake/sqlite3")
set(CMAKE_CXX_STANDARD 17)
include_directories(/opt/homebrew/Cellar/sqlite/3.50.4/include/sqlite3.h)
link_directories(/opt/homebrew/Cellar/sqlite/3.50.4/include/sqlite3.h)
find_library(SQLITE3_LIBRARY NAMES sqlite3 PATHS /opt/homebrew/Cellar/sqlite/3.50.4/include/sqlite3.h)
target_link_libraries(FinalTestProject PRIVATE ${SQLITE3_LIBRARY})
# Поиск SQLite3


# Добавление исполняемого файла


# Линковка с SQLite3
