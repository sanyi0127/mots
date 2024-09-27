// plugins/AnotherPlugin/AnotherPlugin.cpp
#include "../../include/IPlugin.h"
#include <iostream>
#include <unordered_map>

class AnotherPlugin : public IPlugin {
public:
    AnotherPlugin() {}
    ~AnotherPlugin() override {}

    bool initialize() override {
        std::cout << "AnotherPlugin initialized." << std::endl;
        return true;
    }

    void shutdown() override {
        std::cout << "AnotherPlugin shutdown." << std::endl;
    }

    std::string getName() const override {
        return "AnotherPlugin";
    }

    void registerEvent(const std::string& eventName, EventCallback callback) override {
        callbacks_[eventName].emplace_back(callback);
    }

    void triggerEvent(const std::string& eventName, const std::string& eventData) override {
        auto it = callbacks_.find(eventName);
        if (it != callbacks_.end()) {
            for (auto& cb : it->second) {
                cb(eventData);
            }
        }
    }

private:
    std::unordered_map<std::string, std::vector<EventCallback>> callbacks_;
};

// 使用导出宏确保函数被正确导出
extern "C" PLUGIN_API IPlugin* CreatePlugin() {
    return new AnotherPlugin();
}