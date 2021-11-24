cmake_minimum_required(VERSION 2.8.11)
project(thermogui)

set(PRJ_NAME thermogui)

add_definitions(-D__linux__)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pipe -Wall -g -O0")
set(TARGET_LIBS pthread rt)
set(CMAKE_SYSROOT $ENV{SYSROOT})
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${CMAKE_SYSROOT}/lib/arm-linux-gnueabihf")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-rpath-link,${CMAKE_SYSROOT}/usr/lib/arm-linux-gnueabihf")

# Project level flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fexceptions")

set(QT_DIR ${CMAKE_SYSROOT}/usr)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

add_subdirectory(src/logger)

find_package(Qt5 COMPONENTS Qml Widgets Quick REQUIRED
	PATHS ${QT_DIR}
	NO_DEFAULT_PATH
)

set(SOURCES
	src/main.cpp
	src/mqtt.cpp
	src/config.cpp
	src/messenger.cpp
	src/mcThermostat.cpp
	src/mcWpaIf.cpp
	resources.qrc
)

add_executable(${PRJ_NAME} ${SOURCES})

target_link_libraries(${PRJ_NAME}
	logger
	mosquitto
)

qt5_use_modules(${PRJ_NAME}
	Widgets
	Qml
	Quick
)
