cmake_minimum_required(VERSION 3.1.0)

project(HistopathologicalAnalysis)


set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)


find_package(Qt5 COMPONENTS Widgets REQUIRED)
find_package(VTK REQUIRED)




#add_executable(${PROJECT_NAME} "main.cpp")

add_executable(${PROJECT_NAME}
    "main.cpp"
    "mainwindow.ui"
    "mainwindow.cpp"
    )

target_link_libraries(${PROJECT_NAME} Qt5::Widgets)
target_link_libraries(${PROJECT_NAME} ${VTK_LIBRARIES})

