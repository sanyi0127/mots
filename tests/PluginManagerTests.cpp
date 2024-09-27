// tests/PluginManagerTests.cpp
#include "../include/Test.h"
#include "../include/PluginManager.h"
#include <filesystem>
#include <iostream>

// 定义插件路径的辅助函数
std::string getPluginPath() {
    std::filesystem::path exePath = std::filesystem::current_path();
#if defined(_WIN32)
    return (exePath / "SamplePlugin.dll").string();
#elif defined(__APPLE__)
    return (exePath / "libSamplePlugin.dylib").string();
#else
    return (exePath / "libSamplePlugin.so").string();
#endif
}

// 测试插件管理器是否能成功加载插件
TEST(TestLoadPluginSuccess) {
    PluginManager manager;
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");

    // 尝试再次加载相同插件，应该失败
    bool loadedAgain = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(!loadedAgain, "Loading the same plugin again should fail or handle appropriately");
}

// 测试加载不存在的插件
TEST(TestLoadPluginFailure) {
    PluginManager manager;
#if defined(_WIN32)
    std::string invalidPath = "nonexistent_plugin.dll";
#elif defined(__APPLE__)
    std::string invalidPath = "nonexistent_plugin.dylib";
#else
    std::string invalidPath = "nonexistent_plugin.so";
#endif
    bool loaded = manager.loadPlugin(invalidPath);
    ASSERT_TRUE(!loaded, "Loading a nonexistent plugin should fail");
}

// 测试事件注册与触发
TEST(TestEventRegistrationAndTriggering) {
    PluginManager manager;
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");

    bool eventTriggered = false;
    // 通过插件名称注册事件回调
    bool registered = manager.registerPluginEvent("SamplePlugin", "OnTestEvent", [&](const std::string& data) {
        std::cout << "[Test Event Callback] Received data: " << data << std::endl;
        eventTriggered = true;
    });
    ASSERT_TRUE(registered, "Event should be registered successfully");

    // 触发事件
    manager.triggerPluginEvent("OnTestEvent", "TestData");

    ASSERT_TRUE(eventTriggered, "Event should be triggered and callback should be invoked");
}

// 测试插件卸载
TEST(TestPluginUnload) {
    PluginManager manager;
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");

    // 插件管理器析构时会卸载插件
    // 这里明确调用 unloadAll 并检查状态
    manager.unloadAll();

    // 尝试为已卸载的插件注册事件，应该失败
    bool registered = manager.registerPluginEvent("SamplePlugin", "OnAfterUnload", [&](const std::string& data) {
        // 这段代码不应该被调用
        std::cout << "[OnAfterUnload Event] Received data: " << data << std::endl;
    });
    ASSERT_TRUE(!registered, "Registering event for unloaded plugin should fail");

    // 尝试触发事件后，应该不会有回调
    bool eventTriggered = false;
    manager.registerPluginEvent("SamplePlugin", "OnAfterUnload", [&](const std::string& data) {
        eventTriggered = true;
    });
    manager.triggerPluginEvent("OnAfterUnload", "DataAfterUnload");
    ASSERT_TRUE(!eventTriggered, "Event should not be triggered after plugin unload");
}

// 假设新增一个名为 "AnotherPlugin" 的插件
TEST(TestLoadMultiplePlugins) {
    PluginManager manager;
    
    // 加载第一个插件
    std::string pluginPath1 = getPluginPath(); // "SamplePlugin"
    bool loaded1 = manager.loadPlugin(pluginPath1);
    ASSERT_TRUE(loaded1, "SamplePlugin should load successfully");
    
    // 假设另一个插件位于相同目录下，并且已经存在
    // 需要确保存在该插件，否则此测试需要有一个有效的插件路径
    std::string pluginPath2 = (std::filesystem::current_path() / "AnotherPlugin.dll").string(); // 根据平台调整扩展名
    bool loaded2 = manager.loadPlugin(pluginPath2);
    ASSERT_TRUE(loaded2, "AnotherPlugin should load successfully");
    
    // 确保两个插件都已加载
    ASSERT_TRUE(manager.loadPlugin(pluginPath1) == false, "SamplePlugin should not load again");
    ASSERT_TRUE(manager.loadPlugin(pluginPath2) == false, "AnotherPlugin should not load again");
}

TEST(TestUnloadSpecificPlugin) {
    PluginManager manager;
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");
    
    // 注册一个事件
    bool eventTriggered = false;
    bool registered = manager.registerPluginEvent("SamplePlugin", "OnSpecificEvent", [&](const std::string& data) {
        eventTriggered = true;
    });
    ASSERT_TRUE(registered, "Event should be registered successfully");
    
    // 触发事件，确保回调被调用
    manager.triggerPluginEvent("OnSpecificEvent", "BeforeUnload");
    ASSERT_TRUE(eventTriggered, "Event should be triggered and callback should be invoked");
    
    // 重置标志
    eventTriggered = false;
    
    // 卸载插件
    bool unloaded = manager.unloadPlugin("SamplePlugin");
    ASSERT_TRUE(unloaded, "Plugin should unload successfully");
    
    // 尝试触发事件，回调不应被调用
    manager.triggerPluginEvent("OnSpecificEvent", "AfterUnload");
    ASSERT_TRUE(!eventTriggered, "Event should not be triggered after plugin unload");
    
    // 尝试再次卸载，应该失败
    bool unloadedAgain = manager.unloadPlugin("SamplePlugin");
    ASSERT_TRUE(!unloadedAgain, "Unloading already unloaded plugin should fail");
}

TEST(TestTriggerUnregisteredEvent) {
    PluginManager manager;
    
    // 加载插件
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");
    
    // 触发一个未注册的事件，应该不会调用任何回调，也不会崩溃
    manager.triggerPluginEvent("NonExistentEvent", "SomeData");
    
    // 如果没有异常发生，测试通过
    ASSERT_TRUE(true, "Triggering an unregistered event should not cause any issues");
}


TEST(TestRegisterEventWithoutLoadingPlugin) {
    PluginManager manager;
    
    // 尝试为未加载的插件注册事件
    bool registered = manager.registerPluginEvent("NonLoadedPlugin", "SomeEvent", [&](const std::string& data) {
        // 不应该被调用
    });
    ASSERT_TRUE(!registered, "Registering event for a non-loaded plugin should fail");
}

TEST(TestPluginNameUniqueness) {
    PluginManager manager;
    
    // 加载第一个插件
    std::string pluginPath1 = getPluginPath(); // "SamplePlugin"
    bool loaded1 = manager.loadPlugin(pluginPath1);
    ASSERT_TRUE(loaded1, "SamplePlugin should load successfully");
    
    // 假设另一个插件具有相同的名称
    std::string pluginPath2 = (std::filesystem::current_path() / "DuplicateNamePlugin.dll").string(); // 根据平台调整扩展名
    bool loaded2 = manager.loadPlugin(pluginPath2);
    ASSERT_TRUE(!loaded2, "Loading DuplicateNamePlugin with same name should fail");
}

TEST(TestEventCallbackExceptionHandling) {
    PluginManager manager;
    std::string pluginPath = getPluginPath();
    bool loaded = manager.loadPlugin(pluginPath);
    ASSERT_TRUE(loaded, "Plugin should load successfully");
    
    bool normalCallbackTriggered = false;
    bool exceptionCallbackTriggered = false;
    
    // 注册一个正常的回调
    bool registered1 = manager.registerPluginEvent("SamplePlugin", "OnExceptionEvent", [&](const std::string& data) {
        normalCallbackTriggered = true;
        std::cout << "[Normal Callback] Received data: " << data << std::endl;
    });
    ASSERT_TRUE(registered1, "Normal event callback should be registered successfully");
    
    // 注册一个会抛出异常的回调
    bool registered2 = manager.registerPluginEvent("SamplePlugin", "OnExceptionEvent", [&](const std::string& data) {
        exceptionCallbackTriggered = true;
        std::cout << "[Exception Callback] About to throw exception." << std::endl;
        throw std::runtime_error("Intentional Exception in Callback");
    });
    ASSERT_TRUE(registered2, "Exception event callback should be registered successfully");
    
    // 触发事件
    manager.triggerPluginEvent("OnExceptionEvent", "TestData");
    
    // 断言两个回调都被调用
    ASSERT_TRUE(normalCallbackTriggered, "Normal callback should be triggered");
    ASSERT_TRUE(exceptionCallbackTriggered, "Exception callback should be triggered");
}

