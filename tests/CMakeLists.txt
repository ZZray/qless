# rayzhang qss_theme
# 2023/12/05
cmake_minimum_required(VERSION 3.16)
project(testRayQssTheme LANGUAGES CXX)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()

set(SOURCE_FILES 
    src/main.cpp
    src/mainwindow.cpp
    src/mainwindow.h
    src/mainwindow.ui
)

add_executable(${PROJECT_NAME}  ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(${PROJECT_NAME} PRIVATE
    Qt6::Core
    Qt6::Gui
    Qt6::Widgets
    libray-qless
)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
# 查找Qt的windeployqt可执行文件
# get_target_property(_qmake_executable Qt6::qmake IMPORTED_LOCATION)
# get_filename_component(_qt_bin_dir "${_qmake_executable}" DIRECTORY)
# find_program(WINDEPLOYQT_EXECUTABLE windeployqt HINTS "${_qt_bin_dir}")

# 添加自定义命令来运行windeployqt
# add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
#     COMMAND "${WINDEPLOYQT_EXECUTABLE}"
#     "$<TARGET_FILE_DIR:${PROJECT_NAME}>/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
# )