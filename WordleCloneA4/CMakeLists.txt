cmake_minimum_required(VERSION 3.19)
project(WordleCloneA4 LANGUAGES CXX)

find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets)

qt_standard_project_setup()

qt_add_executable(WordleCloneA4
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
    keyboard.h
    keyboard.cpp
    wordlegrid.cpp
    wordlegrid.h
)

target_link_libraries(WordleCloneA4
    PRIVATE
        Qt::Core
        Qt::Widgets
)

install(TARGETS WordleCloneA4
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET WordleCloneA4
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})

file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/words"
     DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")