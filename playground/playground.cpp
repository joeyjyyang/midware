/*

- String parsing
- Lambdas/functors
- Smart pointers
- Move semantics
- Variadic templates
- Testing and dependency injection
- Performance profiling and optimization
- C++20 features (concepts, ranges, coroutines)
- C++17 features (structured bindings, if constexpr, fold expressions)

*/

#include <chrono>
#include <cstdint>
#include <iostream>
#include <memory>
#include <string>
#include <thread>

class IRuntimeNode {
public:
    // Virtual destructor is essential for proper cleanup of derived classes when deleted through a base class pointer.
    virtual ~IRuntimeNode() = default;

    // Pure virtual function(s) below.

    virtual void execute() = 0;

private:
    // Technically, can have static members in interfaces, but considered bad design as interfaces define behavior not state.
    // Static members introduce implementation details and hidden dependencies.
};

class IDriver {
public:
    // Virtual destructor is essential for proper cleanup of derived classes when deleted through a base class pointer.
    virtual ~IDriver() = default;

    // Pure virtual function(s) below.

    virtual void read() = 0;
    virtual uint32_t getUUID() const = 0;

private:
    // Technically, can have static members in interfaces, but considered bad design as interfaces define behavior not state.
    // Static members introduce implementation details and hidden dependencies.
};

class LidarNode : public IRuntimeNode {
public:
    LidarNode(std::unique_ptr<IDriver> driver) : driver_(std::move(driver)) {}

    void execute() final override {
        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            std::cout << "Executing LidarNode with driver UUID: " << driver_->getUUID() << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        }
    }

private:
    std::unique_ptr<IDriver> driver_;
};

class LidarDriver : public IDriver {
public:
    // Mark single argument constructors as explicit to prevent unintended implicit conversions.
    explicit LidarDriver(const uint32_t uuid) : uuid_(uuid) {}

    void read() final override {
        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            std::cout << "Executing LidarDriver with UUID: " << uuid_ << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        }
    }

    uint32_t getUUID() const final override {
        return uuid_;
    }

private:
    uint32_t uuid_;
};

class RuntimeGraph {

};

int main (int argc, char* argv[]) {
    std::unique_ptr<IDriver> driver = std::make_unique<LidarDriver>(12345);
    std::unique_ptr<IRuntimeNode> node = std::make_unique<LidarNode>(std::move(driver));
    node->execute();

    return 0;
}
