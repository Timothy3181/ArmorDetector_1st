CMAKE_MINIMUM_REQUIRED(VERSION 3.10)
project(armor_detector)

find_package(OpenCV REQUIRED)

include_directories(${OpenCV_INCLUDE_DIRS})
include_directories(${PROJECT_SOURCE_DIR}/include)

add_library(armor_detector SHARED ${PROJECT_SOURCE_DIR}/include/armor_detector.cpp)

add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)

target_link_libraries(main armor_detector ${OpenCV_LIBS})

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/file/blue.avi DESTINATION ${CMAKE_CURRENT_BINARY_DIR})