
find_package(Qt5Core REQUIRED)
find_package(Qt5Quick REQUIRED)
find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

include_directories(${Qt5Widgets_INCLUDES})

set (SOURCES main.cpp thread.cpp)

set (RESOURCES dashboard.qrc)

set (HEADERS thread.h)

add_executable(dboard ${SOURCES} ${RESOURCES} ${HEADERS})

target_link_libraries(dboard myaio Qt5::Widgets Qt5::Quick Qt5::Core)
