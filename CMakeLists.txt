cmake_minimum_required(VERSION 3.16)

project(legilimens VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# Add include directories
include_directories(
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    ${CMAKE_CURRENT_SOURCE_DIR}/src/model
    ${CMAKE_CURRENT_SOURCE_DIR}/src/controller
    ${CMAKE_CURRENT_SOURCE_DIR}/src/view
    ${CMAKE_CURRENT_SOURCE_DIR}/src/common
)

set(PROJECT_SOURCES
        src/main.cpp
        src/view/mainwindow.cpp
        src/view/mainwindow.h
        src/view/mainwindow.ui
        src/common/basewidgetmanager.cpp
        src/common/basewidgetmanager.h
        src/view/widgets/sequencewidgetmanager.cpp
        src/view/widgets/sequencewidgetmanager.h
        src/view/widgets/streamswidgetmanager.cpp
        src/view/widgets/streamswidgetmanager.h
        src/view/widgets/slicewidgetmanager.cpp
        src/view/widgets/slicewidgetmanager.h
        src/view/widgets/hexwidgetmanager.cpp
        src/view/widgets/hexwidgetmanager.h
        src/view/widgets/macroblockwidgetmanager.cpp
        src/view/widgets/macroblockwidgetmanager.h
        src/model/model.cpp
        src/model/model.h
        src/model/streamtreemodel.cpp
        src/model/streamtreemodel.h
        src/controller/controller.cpp
        src/controller/controller.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(legilimens
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET legilimens APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(legilimens SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(legilimens
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(legilimens PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.legilimens)
endif()
set_target_properties(legilimens PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS legilimens
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(legilimens)
endif()
