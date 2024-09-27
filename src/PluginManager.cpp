// src/PluginManager.cpp
#include "PluginManager.h"
#include <iostream>

PluginManager::PluginManager() {}

PluginManager::~PluginManager() {
    unloadAll();
}

bool PluginManager::isPluginLoaded(const std::string& pluginName) const {
    for (const auto& pluginInfo : plugins_) {
        if (pluginInfo.instance && pluginInfo.instance->getName() == pluginName) {
            return true;
        }
    }
    return false;
}

bool PluginManager::loadPlugin(const std::string& path) {
    // 检查插件路径是否已经加载
    for (const auto& pluginInfo : plugins_) {
        if (pluginInfo.path == path) {
            std::cerr << "Plugin already loaded: " << path << std::endl;
            return false;
        }
    }

    LibHandle handle = loadLibrary(path);
    if (!handle) {
        std::cerr << "Failed to load library: " << path << std::endl;
        return false;
    }

    CreatePluginFunc createFunc = getCreatePluginFunc(handle);
    if (!createFunc) {
        std::cerr << "Failed to find CreatePlugin function in: " << path << std::endl;
        unloadLibrary(handle);
        return false;
    }

    IPlugin* plugin = createFunc();
    if (!plugin) {
        std::cerr << "Failed to create plugin instance from: " << path << std::endl;
        unloadLibrary(handle);
        return false;
    }

    if (!plugin->initialize()) {
        std::cerr << "Failed to initialize plugin: " << path << std::endl;
        plugin->shutdown();
        unloadLibrary(handle);
        return false;
    }

    std::string pluginName = plugin->getName();

    // 检查插件名称是否已经存在
    if (isPluginLoaded(pluginName)) {
        std::cerr << "Plugin with name '" << pluginName << "' is already loaded." << std::endl;
        plugin->shutdown();
        unloadLibrary(handle);
        return false;
    }

    PluginInfo info;
    info.path = path;
    info.handle = handle;
    info.instance.reset(plugin);
    plugins_.emplace_back(std::move(info));

    std::cout << "Successfully loaded plugin: " << plugin->getName() << std::endl;
    return true;
}

bool PluginManager::unloadPlugin(const std::string& pluginName) {
    for (auto it = plugins_.begin(); it != plugins_.end(); ++it) {
        if (it->instance && it->instance->getName() == pluginName) {
            // 注销所有与该插件关联的事件回调
            eventManager_.unregisterPluginCallbacks(pluginName);
    
            it->instance->shutdown();
            it->instance.reset();
    
            unloadLibrary(it->handle);
            std::cout << "Unloaded plugin: " << it->path << std::endl;
    
            plugins_.erase(it);
            return true;
        }
    }
    std::cerr << "Plugin not found: " << pluginName << std::endl;
    return false;
}

void PluginManager::unloadAll() {
    for (auto& pluginInfo : plugins_) {
        if (pluginInfo.instance) {
            // 注销所有与该插件关联的事件回调
            eventManager_.unregisterPluginCallbacks(pluginInfo.instance->getName());

            pluginInfo.instance->shutdown();
            pluginInfo.instance.reset();
        }
        unloadLibrary(pluginInfo.handle);
        std::cout << "Unloaded plugin: " << pluginInfo.path << std::endl;
    }
    plugins_.clear();
}

bool PluginManager::registerPluginEvent(const std::string& pluginName, const std::string& eventName, EventCallback callback) {
    if (!isPluginLoaded(pluginName)) {
        std::cerr << "Cannot register event. Plugin '" << pluginName << "' is not loaded." << std::endl;
        return false;
    }
    eventManager_.registerEvent(pluginName, eventName, callback);
    return true;
}

void PluginManager::triggerPluginEvent(const std::string& eventName, const std::string& eventData) {
    eventManager_.triggerEvent(eventName, eventData);
}

LibHandle PluginManager::loadLibrary(const std::string& path) {
#if defined(_WIN32)
    return LoadLibraryA(path.c_str());
#else
    return dlopen(path.c_str(), RTLD_LAZY);
#endif
}

void PluginManager::unloadLibrary(LibHandle handle) {
    if (!handle) return;
#if defined(_WIN32)
    FreeLibrary(handle);
#else
    dlclose(handle);
#endif
}

CreatePluginFunc PluginManager::getCreatePluginFunc(LibHandle handle) {
#if defined(_WIN32)
    FARPROC func = GetProcAddress(handle, "CreatePlugin");
    if (!func) {
        std::cerr << "GetProcAddress failed with error: " << GetLastError() << std::endl;
    }
    return reinterpret_cast<CreatePluginFunc>(func);
#else
    dlerror(); // 清除任何现有错误
    CreatePluginFunc func = reinterpret_cast<CreatePluginFunc>(dlsym(handle, "CreatePlugin"));
    const char* dlsym_error = dlerror();
    if (dlsym_error) {
        std::cerr << "dlsym failed: " << dlsym_error << std::endl;
        return nullptr;
    }
    return func;
#endif
}