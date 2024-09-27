// src/main.cpp
#include "PluginManager.h"
#include <iostream>
#include <filesystem>

int main() {
    PluginManager manager;

    // 使用 apphome/bin 目录下的插件路径
    std::filesystem::path exePath = std::filesystem::current_path();
    std::string pluginPath;

#if defined(_WIN32)
    pluginPath = (exePath / "SamplePlugin.dll").string();
#elif defined(__APPLE__)
    pluginPath = (exePath / "libSamplePlugin.dylib").string();
#else
    pluginPath = (exePath / "libSamplePlugin.so").string();
#endif

    if (!manager.loadPlugin(pluginPath)) {
        std::cerr << "Failed to load plugin from: " << pluginPath << std::endl;
        return 1;
    }

    // 注册事件回调
    manager.registerPluginEvent("SamplePlugin", "OnDataReceived", [](const std::string& data) {
        std::cout << "[Event] OnDataReceived: " << data << std::endl;
    });

    // 触发事件
    manager.triggerPluginEvent("OnDataReceived", "Hello, Plugin!");

    // 程序结束时，插件管理器将自动卸载所有插件
    return 0;
}