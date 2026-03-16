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
- std::optional

*/

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <queue>
#include <string>
#include <thread>
#include <vector>

class IDriver {
public:
    // Virtual destructor is essential for proper cleanup of derived classes when deleted through a base class pointer.
    virtual ~IDriver() = default;

    // Pure virtual function(s) below.

    virtual void read() = 0;

    virtual uint32_t getUuid() const = 0;

private:
    // Technically, can have static members in interfaces, but considered bad design as interfaces define behavior not state.
    // Static members introduce implementation details and hidden dependencies.
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
        auto start = std::chrono::high_resolution_clock::now();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarDriver read() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    uint32_t getUuid() const final {
        return uuid_;
    }

private:
    uint32_t uuid_;
};

class IRuntimeNode {
public:
    // Virtual destructor is essential for proper cleanup of derived classes when deleted through a base class pointer.
    virtual ~IRuntimeNode() = default;

    // Pure virtual function(s) below.

    virtual void publish() = 0;

    virtual void execute() = 0;

    virtual double getFrequency() const = 0;

private:
    // Technically, can have static members in interfaces, but considered bad design as interfaces define behavior not state.
    // Static members introduce implementation details and hidden dependencies.
};

class LidarNode : public IRuntimeNode {
public:
    LidarNode(std::unique_ptr<IDriver>&& driver, const double frequency) : driver_(std::move(driver)), frequency_(frequency) {}

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
    // LidarNode(LidarNode&&) noexcept = default;
    LidarNode(LidarNode&& other) noexcept : driver_(std::move(other.driver_)) {}

    // Move Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    // LidarNode& operator=(LidarNode&&) noexcept = default;
    LidarNode& operator=(LidarNode&& other) noexcept {
        if (this != &other) {
            driver_ = std::move(other.driver_);
        }

        return *this;
    }

    void publish() final {
        auto start = std::chrono::high_resolution_clock::now();

        std::this_thread::sleep_for(std::chrono::seconds(1));

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarNode publish() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    void execute() final {
        auto start = std::chrono::high_resolution_clock::now();

        driver_->read();
        publish();

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarNode execute() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    double getFrequency() const final {
        return frequency_;
    }

private:
    double frequency_{0.0};
    std::unique_ptr<IDriver> driver_;
};

class RuntimeGraph {
public:
    // Mark single argument constructors as explicit to prevent unintended implicit conversions.
    explicit RuntimeGraph(const unsigned int num_threads) : num_threads_(num_threads) {
        std::cout << "RuntimeGraph constructed called with " << num_threads_ << " threads.\n";

        running_.store(true);

        for (unsigned int i = 0; i < num_threads_; ++i) {
            // More efficient than thread_pool_.emplace_back(std::thread([this]() { ... })) since it constructs the thread in place after forwarding the lambda to the thread constructor, avoiding an extra move/copy of the thread object.
            thread_pool_.emplace_back([this]() {
                std::cout << "Thread " << std::this_thread::get_id() << " started.\n";

                while (true) {
                    std::function<void()> task;

                    {
                        std::unique_lock<std::mutex> lock(mtx_);

                        // If no tasks, release the lock and wait until a task is added to the queue. The lambda predicate ensures that spurious wakeups don't cause the thread to proceed without a task.
                        // If tasks, acquire the lock, pop a task from the queue, release the lock, and execute it. This allows multiple threads to efficiently wait for and process tasks without busy-waiting.
                        cv_.wait(lock, [this]() {
                            // Also wake threads if running_ is false to allow them to exit gracefully.
                            return !task_queue_.empty() || !running_.load();
                        });

                        // Thread is now awake; check which condition caused the wakeup and handle it appropriately.
                        if (!task_queue_.empty()) {
                            task = std::move(task_queue_.front());
                            task_queue_.pop();
                            lock.unlock();
                            // Execute the task outside of the lock to allow other threads to access the queue.
                            task();
                        }
                        else if (!running_.load()) {
                            return;
                        }
                    }
                }
            });
        }
    }

    ~RuntimeGraph() {
        std::cout << "RuntimeGraph destructor called.\n";

        running_.store(false);
        cv_.notify_all();

        for (auto& thread : thread_pool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void stop() {
        running_.store(false);
        cv_.notify_all();
    }

    void registerNode(std::unique_ptr<IRuntimeNode>&& node) {
        nodes_.emplace_back(std::move(node));
    }

    void dispatchTask(std::function<void()>&& task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            task_queue_.emplace(std::move(task));
        }

        cv_.notify_one();
    }

    void run() {
        for (const auto& node : nodes_) {
            dispatchTask([&node, this]() {
                while(running_.load()) {
                    auto start = std::chrono::high_resolution_clock::now();

                    node->execute();
                    std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>(1000.0 / node->getFrequency())));

                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << std::chrono::duration_cast<std::chrono::microseconds>(end - start).count() << " microseconds\n";
                }
            });
        }
    }

    // // Copy Constructor.
    // // Technically, unnecessary since compiler will generate this.
    // RuntimeGraph(const RuntimeGraph&) = default;

    // // Copy Assignment Operator.
    // // Technically, unnecessary since compiler will generate this.
    // RuntimeGraph& operator=(const RuntimeGraph&) = default;

    // // Move Constructor.
    // // Technically, unnecessary since compiler will generate this.
    // RuntimeGraph(RuntimeGraph&&) noexcept = default;

    // // Move Assignment Operator.
    // // Technically, unnecessary since compiler will generate this.
    // RuntimeGraph& operator=(RuntimeGraph&&) noexcept = default;

private:
    unsigned int num_threads_;
    std::mutex mtx_;
    std::condition_variable cv_;
    std::vector<std::thread> thread_pool_;
    std::queue<std::function<void()>> task_queue_;
    std::vector<std::unique_ptr<IRuntimeNode>> nodes_;
    std::atomic<bool> running_{false};
};

int main (int argc, char* argv[]) {
    std::unique_ptr<IDriver> lidar_driver = std::make_unique<LidarDriver>(12345);

    std::unique_ptr<IRuntimeNode> lidar_node = std::make_unique<LidarNode>(std::move(lidar_driver), 10.0);

    RuntimeGraph runtime_graph(std::thread::hardware_concurrency());
    // lidar_driver is nullptr after move, so cannot be used directly hereafter.
    //runtime_graph.registerNode(std::move(lidar_node));
    // lidar_node is nullptr after move, so cannot be used directly hereafter.run(
    //runtime_graph.run();

    return 0;
}
