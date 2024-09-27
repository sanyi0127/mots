// include/Event.h
#ifndef EVENT_H
#define EVENT_H

#include <string>
#include <unordered_map>
#include <vector>
#include <functional>
#include <mutex>
#include <algorithm>
#include <iostream>

using EventCallback = std::function<void(const std::string&)>;

struct CallbackInfo {
    std::string pluginName;
    EventCallback callback;
};

class EventManager {
public:
    // 注册事件，并关联插件名称
    void registerEvent(const std::string& pluginName, const std::string& eventName, EventCallback callback) {
        std::lock_guard<std::mutex> lock(mutex_);
        callbacks_[eventName].emplace_back(CallbackInfo{ pluginName, callback });
    }

    // 注销与插件名称相关的所有事件回调
    void unregisterPluginCallbacks(const std::string& pluginName) {
        std::lock_guard<std::mutex> lock(mutex_);
        for (auto& [eventName, cbList] : callbacks_) {
            cbList.erase(
                std::remove_if(cbList.begin(), cbList.end(),
                    [&](const CallbackInfo& info) {
                        return info.pluginName == pluginName;
                    }),
                cbList.end()
            );
        }
    }

    // 触发事件
    void triggerEvent(const std::string& eventName, const std::string& eventData) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = callbacks_.find(eventName);
        if (it != callbacks_.end()) {
            // 复制回调列表以防止在回调中修改原列表
            auto cbList = it->second;
            for (auto& cbInfo : cbList) {
                try {
                    cbInfo.callback(eventData);
                }
                catch (const std::exception& e) {
                    std::cerr << "Exception in event callback: " << e.what() << std::endl;
                }
                catch (...) {
                    std::cerr << "Unknown exception in event callback." << std::endl;
                }
            }
        }
    }
    
private:
    std::unordered_map<std::string, std::vector<CallbackInfo>> callbacks_;
    std::mutex mutex_;
};

#endif // EVENT_H