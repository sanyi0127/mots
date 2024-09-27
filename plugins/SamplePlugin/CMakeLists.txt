# plugins/SamplePlugin/CMakeLists.txt
cmake_minimum_required(VERSION 3.5)
project(SamplePlugin)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 定义库名称和源文件
add_library(SamplePlugin SHARED SamplePlugin.cpp)

# 包含头文件路径
target_include_directories(SamplePlugin PRIVATE ../../include)

# 链接核心库（如果插件需要访问框架的内部）
target_link_libraries(SamplePlugin PRIVATE MotsFramework)

# 对于 Windows，确保使用 .dll 而不是 lib 前缀
if(WIN32)
    set_target_properties(SamplePlugin PROPERTIES PREFIX "")
endif()

# 对于 macOS，设置共享库的扩展名并配置 rpath
if(APPLE)
    set_target_properties(SamplePlugin PROPERTIES SUFFIX ".dylib")
    set_target_properties(SamplePlugin PROPERTIES
        BUILD_RPATH "@loader_path"
    )
endif()

# 对于类 Unix 系统，设置 rpath
if(UNIX AND NOT APPLE)
    set_target_properties(SamplePlugin PROPERTIES
        BUILD_RPATH "$ORIGIN"
    )
endif()

# 复制插件到 apphome/bin
add_custom_command(TARGET SamplePlugin POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy
    $<TARGET_FILE:SamplePlugin>
    "${APP_HOME_BIN}"
)