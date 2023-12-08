# rayzhang qss_theme
# 2023/12/05
project(libray-qtheme LANGUAGES CXX)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt6 REQUIRED COMPONENTS Core Gui Widgets)

qt_standard_project_setup()


set(SOURCE_FILES 
    src/qtheme.cpp
	src/qtheme.h
)
add_library(${PROJECT_NAME} SHARED ${SOURCE_FILES})
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets libray-qless)

# debug 输出带d的库
set_target_properties(${PROJECT_NAME} PROPERTIES DEBUG_POSTFIX d)

# 预处理器/预定义宏
target_compile_definitions(${PROJECT_NAME} PRIVATE LIBQLESS_API_EXPORT)
source_group(TREE ${CMAKE_CURRENT_SOURCE_DIR} FILES ${SOURCE_FILES})
