cmake_minimum_required(VERSION 3.1.0)

project(HistopathologicalAnalysis)

set(CMAKE_CXX_STANDARD 20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(VTK REQUIRED)
find_package(ITK REQUIRED)
find_package(dlib REQUIRED)

include(${ITK_USE_FILE})




file(GLOB core
    "core/*.h"
   "core/*.cpp"
)


file(GLOB util
    "util/*.h"
   "util/*.cpp"
)

file(GLOB dataStructures
    "dataStructures/*.h"
    "dataStructures/*.cpp"
)

add_executable(${PROJECT_NAME}
    "main.cpp"
    "mainwindow.ui"
    "mainwindow.cpp"
    "mainwindow.h"

    ${core}
    ${util}
    ${dataStructures}
    )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})
target_link_libraries(${PROJECT_NAME} ${ITK_LIBRARIES})
#target_link_libraries(${PROJECT_NAME} ${dlib_LIBRARIES}) #deprecated
target_link_libraries(${PROJECT_NAME} dlib::dlib)
