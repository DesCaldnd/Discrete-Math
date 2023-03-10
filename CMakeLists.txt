cmake_minimum_required(VERSION 3.23)
project(DiscreteMath)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH "C:/Qt/6.4.2/mingw_64/lib/cmake")
set(THREADS_PREFER_PTHREAD_FLAG ON)

if (MSVC)
    set(CMAKE_CXX_FLAGS "/Zc:__cplusplus")
    set(CMAKE_PREFIX_PATH "C:/Qt/6.4.2/msvc2019_64/lib/cmake")
endif ()


#set(QXlsxQt6_DIR "C://Program Files (x86)/QXlsx/lib/cmake/QXlsxQt6")

find_package(Qt6 COMPONENTS
        Core
        Gui
        Widgets
        REQUIRED)

find_package(QXlsxQt6 REQUIRED)

find_package(Threads REQUIRED)

set(RELEASE_EXECUTABLE "")

if(CMAKE_BUILD_TYPE MATCHES "Release")
    set(RELEASE_EXECUTABLE WIN32)
endif()

set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/appico.rc")
add_executable(DiscreteMath ${RELEASE_EXECUTABLE} main.cpp Source/mainwindow.cpp Headers/mainwindow.h
        Source/mainwindow.ui Source/Operation.cpp Headers/Operation.h Source/ExpressionSymbol.cpp
        Headers/ExpressionSymbol.h Source/Variable.cpp Headers/Variable.h resource.qrc ${APP_ICON_RESOURCE_WINDOWS}
        Source/Calculator.cpp Headers/Calculator.h Source/developer.cpp Headers/developer.h Source/developer.ui)
target_link_libraries(DiscreteMath
        Qt::Core
        Qt::Gui
        Qt::Widgets
        QXlsx::QXlsx
        Threads::Threads
        )


if (WIN32)
    set(DEBUG_SUFFIX)
    if (MSVC AND CMAKE_BUILD_TYPE MATCHES "Debug")
        set(DEBUG_SUFFIX "d")
    endif ()
    set(QT_INSTALL_PATH "${CMAKE_PREFIX_PATH}")
    if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
        set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        if (NOT EXISTS "${QT_INSTALL_PATH}/bin")
            set(QT_INSTALL_PATH "${QT_INSTALL_PATH}/..")
        endif ()
    endif ()
    if (EXISTS "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E make_directory
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/plugins/platforms/qwindows${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>/plugins/platforms/")
    endif ()
    foreach (QT_LIB Core Gui Widgets)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                COMMAND ${CMAKE_COMMAND} -E copy
                "${QT_INSTALL_PATH}/bin/Qt6${QT_LIB}${DEBUG_SUFFIX}.dll"
                "$<TARGET_FILE_DIR:${PROJECT_NAME}>")
    endforeach (QT_LIB)
endif ()
