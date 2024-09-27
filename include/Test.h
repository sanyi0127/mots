// include/Test.h
#ifndef TEST_H
#define TEST_H

#include <string>
#include <vector>
#include <functional>
#include <iostream>
#include <sstream>

class Test {
public:
    using TestFunc = std::function<void()>;

    static Test& getInstance() {
        static Test instance;
        return instance;
    }

    void registerTest(const std::string& testName, TestFunc func) {
        tests_.emplace_back(testName, func);
    }

    void run() {
        int passed = 0;
        int failed = 0;
        for (const auto& [name, func] : tests_) {
            try {
                func();
                std::cout << "[PASS] " << name << std::endl;
                ++passed;
            }
            catch (const std::exception& e) {
                std::cout << "[FAIL] " << name << " - " << e.what() << std::endl;
                ++failed;
            }
            catch (...) {
                std::cout << "[FAIL] " << name << " - Unknown exception" << std::endl;
                ++failed;
            }
        }
        std::cout << "===================================" << std::endl;
        std::cout << "Total: " << tests_.size() << ", Passed: " << passed << ", Failed: " << failed << std::endl;
    }

    // Assertion macros
    struct AssertionException : public std::exception {
        std::string message;
        AssertionException(const std::string& msg) : message(msg) {}
        const char* what() const noexcept override {
            return message.c_str();
        }
    };

    static void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw AssertionException("Assertion failed: " + message);
        }
    }

    template <typename T, typename U>
    static void assertEqual(const T& a, const U& b, const std::string& message = "") {
        if (!(a == b)) {
            std::ostringstream oss;
            oss << "Assertion failed: " << a << " != " << b;
            if (!message.empty()) {
                oss << " (" << message << ")";
            }
            throw AssertionException(oss.str());
        }
    }

private:
    Test() = default;
    std::vector<std::pair<std::string, TestFunc>> tests_;
};

// Macros to define tests and assertions
#define TEST(testName) \
    void testName(); \
    struct testName##_Register { \
        testName##_Register() { \
            Test::getInstance().registerTest(#testName, testName); \
        } \
    } testName##_instance; \
    void testName()

#define ASSERT_TRUE(condition, message) \
    Test::assertTrue(condition, message)

#define ASSERT_EQ(a, b, message) \
    Test::assertEqual(a, b, message)

#endif // TEST_H