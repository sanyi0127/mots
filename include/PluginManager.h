// include/PluginManager.h
#ifndef PLUGINMANAGER_H
#define PLUGINMANAGER_H

#include "IPlugin.h"
#include "Event.h"
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>

#if defined(_WIN32)
#include <windows.h>
typedef HMODULE LibHandle;
#else
#include <dlfcn.h>
typedef void* LibHandle;
#endif

struct PluginInfo {
    std::string path;
    LibHandle handle;
    std::unique_ptr<IPlugin> instance;
};

class PluginManager {
public:
    PluginManager();
    ~PluginManager();

    // 返回值改为 bool，表示是否成功注册事件
    bool loadPlugin(const std::string& path);
    // 卸载特定插件
    bool unloadPlugin(const std::string& pluginName);
    void unloadAll();

    // 返回 bool，表示是否成功注册事件
    bool registerPluginEvent(const std::string& pluginName, const std::string& eventName, EventCallback callback);
    void triggerPluginEvent(const std::string& eventName, const std::string& eventData); // 移除了 pluginName 参数，因为事件是全局的

private:
    std::vector<PluginInfo> plugins_;
    EventManager eventManager_;

    LibHandle loadLibrary(const std::string& path);
    void unloadLibrary(LibHandle handle);
    CreatePluginFunc getCreatePluginFunc(LibHandle handle);

    // 辅助函数：检查插件是否已加载
    bool isPluginLoaded(const std::string& pluginName) const;
};

#endif // PLUGINMANAGER_H