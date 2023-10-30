cmake_minimum_required(VERSION 3.20.0)
project(SimShell VERSION 0.1.0 LANGUAGES C CXX)
add_definitions(-DCMAKE_EXPORT_COMPILE_COMMANDS=ON)
include(CTest)
enable_testing()
include_directories("${CMAKE_CURRENT_SOURCE_DIR}/include")

add_executable(SimShell main.cpp)
target_link_libraries(SimShell readline)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
