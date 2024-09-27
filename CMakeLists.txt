# CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(MotsContainerFramework)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义全局输出目录
set(APP_HOME_BIN "${CMAKE_SOURCE_DIR}/apphome/bin")
file(MAKE_DIRECTORY ${APP_HOME_BIN})  # 确保目录存在

# 设置所有目标的输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${APP_HOME_BIN})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${APP_HOME_BIN})
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${APP_HOME_BIN})

# 定义核心库
add_library(MotsFramework STATIC
    src/PluginManager.cpp
    src/Event.cpp
)

# 包含头文件路径
target_include_directories(MotsFramework PUBLIC include)

# 配置插件
add_subdirectory(plugins/SamplePlugin)
add_subdirectory(plugins/AnotherPlugin) # 新增
add_subdirectory(plugins/DuplicateNamePlugin) # 新增

# 主程序
add_executable(MainApp src/main.cpp)

# 链接核心库
target_link_libraries(MainApp PRIVATE MotsFramework)

# 链接动态库路径（类 Unix 系统需要链接 dl 库）
if (UNIX AND NOT APPLE)
    target_link_libraries(MainApp PRIVATE dl)
endif()

# 设置调试器工作目录，使其指向 apphome/bin
set_target_properties(MainApp PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${APP_HOME_BIN}"
)

# 添加单元测试子目录
add_subdirectory(tests)

# 添加编译选项以生成覆盖率信息
if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    option(COVERAGE "Enable coverage reporting" ON)
else()
    option(COVERAGE "Enable coverage reporting" OFF)
endif()

if(COVERAGE)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(--coverage -O0 -g)
        link_libraries(--coverage)
    endif()
endif()