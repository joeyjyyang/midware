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

#include <atomic>
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
    // Mark single argument constructors as explicit to prevent unintended implicit conversions.
    explicit LidarNode(std::unique_ptr<IDriver>&& driver) : driver_(std::move(driver)) {
        running_.store(true);
    }

    // Technically, unnecessary since compiler will generate this.
    ~LidarNode() override = default;

    // Copy Constructor.
    // Technically, unnecessary since compiler will generate this.
    LidarNode(const LidarNode&) = delete;

    // Copy Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    LidarNode& operator=(const LidarNode&) = delete;

    // Move Constructor.
    // Technically, unnecessary since compiler will generate this.
    LidarNode(LidarNode&& other) noexcept : driver_(std::move(other.driver_)) {}

    // Move Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    LidarNode& operator=(LidarNode&& other) noexcept {
        if (this != &other) {
            driver_ = std::move(other.driver_);
        }

        return *this;
    }

    void stop() {
        running_.store(false);
    }

    void execute() final {
        while (running_.load()) {
            auto start = std::chrono::high_resolution_clock::now();

            driver_->read();
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds\n";
        }
    }

private:
    std::unique_ptr<IDriver> driver_;
    std::atomic<bool> running_{false};
};

class LidarDriver : public IDriver {
public:
    // Mark single argument constructors as explicit to prevent unintended implicit conversions.
    explicit LidarDriver(const uint32_t uuid) : uuid_(uuid) {}

    // Technically, unnecessary since compiler will generate this.
    ~LidarDriver() override = default;

    // Copy Constructor.
    // Technically, unnecessary since compiler will generate this.
    LidarDriver(const LidarDriver&) = default;

    // Copy Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    LidarDriver& operator=(const LidarDriver&) = default;

    // Move Constructor.
    // Technically, unnecessary since compiler will generate this.
    LidarDriver(LidarDriver&&) noexcept = default;

    // Move Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    LidarDriver& operator=(LidarDriver&&) noexcept = default;

    void read() final {
        std::cout << "Reading data from LidarDriver with UUID: " << uuid_ << "\n";
    }

    uint32_t getUUID() const final {
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
    // driver is nullptr after move, so cannot be used directly hereafter.

    node->execute();

    return 0;
}
