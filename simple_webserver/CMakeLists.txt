cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
project(simple_webserver LANGUAGES CXX)

FILE(GLOB sources CONFIGURE_DEPEND ./*.h ./*.cc)
add_executable(${PROJECT_NAME} ${sources})
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
message(STATUS ${PROJECT_NAME})