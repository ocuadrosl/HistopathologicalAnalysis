cmake_minimum_required(VERSION 3.1.0)

project(HistopathologicalAnalysis)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(VTK REQUIRED)
find_package(ITK REQUIRED)

include(${ITK_USE_FILE})

#include_directories("core")


file(GLOB core
    "core/*.h"
   "core/*.cpp"
)

file(GLOB util
    "util/*.h"
   "util/*.cpp"
)



add_executable(${PROJECT_NAME}
    "main.cpp"
    "mainwindow.ui"
    "mainwindow.cpp"
    ${core}
    ${util}
    )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})

