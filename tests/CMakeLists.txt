# tests/CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(MotsContainerFrameworkTests)

# 启用测试功能
enable_testing()

# 添加测试源文件
file(GLOB TEST_SOURCES "*.cpp")

# 创建单元测试可执行文件
add_executable(UnitTests ${TEST_SOURCES})

# 包含头文件路径
target_include_directories(UnitTests PRIVATE ../include)

# 链接核心库
target_link_libraries(UnitTests PRIVATE MotsFramework)

# 链接动态库路径（类 Unix 系统需要链接 dl 库）
if (UNIX AND NOT APPLE)
    target_link_libraries(UnitTests PRIVATE dl)
endif()

# 设置运行时寻找插件的路径（输出到 apphome/bin）
set_target_properties(UnitTests PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${APP_HOME_BIN}"
)

# 复制插件到测试可执行文件同目录
add_custom_command(TARGET UnitTests POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:SamplePlugin>
    "${APP_HOME_BIN}"
)

# 设置调试器工作目录，使其指向 apphome/bin
set_target_properties(UnitTests PROPERTIES
    VS_DEBUGGER_WORKING_DIRECTORY "${APP_HOME_BIN}"
)

# 添加测试到 CTest
add_test(NAME UnitTests COMMAND UnitTests)