cmake_minimum_required(VERSION 3.5)

project(WangPanClient LANGUAGES CXX)

set(CMAKE_BUILD_TYPE "Debug")
set(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb -pg")
set(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(QT_DIR "/home/qq469015470/Qt/5.15.1/gcc_64/lib/cmake/Qt5/")
#set(Qt5_DIR "/home/qq469015470/Qt/5.15.1/gcc_64/lib/cmake/Qt5/")

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check http://doc.qt.io/qt-5/deployment-android.html for more information.
# They need to be set before the find_package(Qt5 ...) call.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Network REQUIRED)

if(ANDROID)
  add_library(${PROJECT_NAME} SHARED
    main.cpp
    loginwindow.cpp
    loginwindow.h
    mainwindow.cpp
    mainwindow.h
    DaoHang.h
    Daohang.cpp
    FileView.h
    FileView.cpp
    ProgressView.h
    ProgressView.cpp
    CreateDirectoryDialog.h
    CreateDirectoryDialog.cpp
  )
else()
  add_executable(${PROJECT_NAME}
    main.cpp
    loginwindow.cpp
    loginwindow.h
    mainwindow.cpp
    mainwindow.h
    DaoHang.h
    DaoHang.cpp
    FileView.h
    FileView.cpp
    ProgressView.h
    ProgressView.cpp
    CreateDirectoryDialog.h
    CreateDirectoryDialog.cpp
  )
endif()

target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(${PROJECT_NAME} PRIVATE pthread)
