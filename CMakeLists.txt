
cmake_minimum_required(VERSION 3.14)
project(OcrBridgeWinRT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${CMAKE_SOURCE_DIR}/src")
file(GLOB SOURCES "src/*.cpp")

add_executable(OcrBridgeWinRT ${SOURCES})
target_link_libraries(OcrBridgeWinRT WindowsApp)
