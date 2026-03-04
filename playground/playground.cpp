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

class DriverNode : public IRuntimeNode {
public:
    DriverNode(const uint32_t uuid) : uuid_(uuid) {}

    void execute() final override {
        while (true) {
            auto start = std::chrono::high_resolution_clock::now();

            std::cout << "Executing DriverNode with UUID: " << uuid_ << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(1));

            auto end = std::chrono::high_resolution_clock::now();
            std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds" << std::endl;
        }
    }

private:
    uint32_t uuid_;
};

class RuntimeGraph {

};

int main (int argc, char* argv[]) {
    std::unique_ptr<IRuntimeNode> node = std::make_unique<DriverNode>(12345);
    node->execute();

    return 0;
}
