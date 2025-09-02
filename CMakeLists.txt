
cmake_minimum_required(VERSION 3.14)
project(OcrBridgeWinRT)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories("${CMAKE_SOURCE_DIR}/src")

add_executable(OcrBridgeWinRT src/main.cpp)

# Link WindowsAppSDK

include_directories("${CMAKE_PREFIX_PATH}/include")
link_directories("${CMAKE_PREFIX_PATH}/lib")

add_executable(OcrBridgeWinRT src/main.cpp)
target_link_libraries(OcrBridgeWinRT WindowsApp)

target_link_libraries(OcrBridgeWinRT Microsoft.WindowsAppSDK::WindowsApp)
