// include/IPlugin.h
#ifndef IPLUGIN_H
#define IPLUGIN_H

#include <string>
#include <vector>
#include <functional>

// 定义事件类型
using EventCallback = std::function<void(const std::string&)>;

// 导出宏定义
#if defined(_WIN32) || defined(_WIN64)
    #define PLUGIN_API __declspec(dllexport)
#else
    #define PLUGIN_API
#endif

class IPlugin {
public:
    virtual ~IPlugin() = default;

    // 插件初始化
    virtual bool initialize() = 0;

    // 插件卸载
    virtual void shutdown() = 0;

    // 获取插件名称
    virtual std::string getName() const = 0;

    // 注册事件回调
    virtual void registerEvent(const std::string& eventName, EventCallback callback) = 0;

    // 触发事件
    virtual void triggerEvent(const std::string& eventName, const std::string& eventData) = 0;
};

typedef IPlugin* (*CreatePluginFunc)();

#endif // IPLUGIN_H