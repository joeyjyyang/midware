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

        // TODO: Add work here.

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarDriver " << uuid_ << " read() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
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

    virtual std::string getName() const = 0;

    virtual double getFrequency() const = 0;

private:
    // Technically, can have static members in interfaces, but considered bad design as interfaces define behavior not state.
    // Static members introduce implementation details and hidden dependencies.
};

class LidarNode : public IRuntimeNode {
public:
    LidarNode(const std::string& name, std::unique_ptr<IDriver>&& driver, const double frequency) : name_(name), driver_(std::move(driver)), frequency_(frequency) {}

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
    LidarNode(LidarNode&& other) noexcept : name_(std::move(other.name_)), driver_(std::move(other.driver_)), frequency_(other.frequency_) {}

    // Move Assignment Operator.
    // Technically, unnecessary since compiler will generate this.
    // LidarNode& operator=(LidarNode&&) noexcept = default;
    LidarNode& operator=(LidarNode&& other) noexcept {
        if (this != &other) {
            name_ = std::move(other.name_);
            driver_ = std::move(other.driver_);
            frequency_ = other.frequency_;
        }

        return *this;
    }

    void publish() final {
        auto start = std::chrono::high_resolution_clock::now();

        // TODO: Add work here.

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarNode " << name_ << " publish() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    void execute() final {
        auto start = std::chrono::high_resolution_clock::now();

        driver_->read();
        publish();

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarNode " << name_ << " execute() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    std::string getName() const final {
        return name_;
    }

    double getFrequency() const final {
        return frequency_;
    }

private:
    std::string name_;
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
                        // Need this to allow threads to exit while loop and become joinable to terminate gracefully.
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

        {
            std::lock_guard<std::mutex> lock(mtx_);
            running_.store(false);
        }

        cv_.notify_all();

        for (auto& thread : thread_pool_) {
            if (thread.joinable()) {
                std::cout << "Thread " << thread.get_id() << " joined.\n";

                thread.join();
            }
        }
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            running_.store(false);
        }

        cv_.notify_all();
    }

    void registerNode(std::unique_ptr<IRuntimeNode>&& node) {
        std::cout << "Registering node: " << node->getName() << " with frequency: " << node->getFrequency() << " Hz\n";

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
            // Avoid dangling dereference to std::unique_ptr by capturing a raw pointer to the node in the lambda.
            IRuntimeNode* node_ptr = node.get();

            dispatchTask([node_ptr, this]() {
                while(running_.load()) {
                    auto start = std::chrono::high_resolution_clock::now();

                    auto next = std::chrono::steady_clock::now() + std::chrono::milliseconds(static_cast<int>(1000.0 / node_ptr->getFrequency()));
                    node_ptr->execute();
                    // Use sleep_until() instead of sleep_for() to avoid accumulating drift over time. This ensures that the node executes at consistent intervals based on its specified frequency, even if individual executions take varying amounts of time.
                    std::this_thread::sleep_until(next);

                    auto end = std::chrono::high_resolution_clock::now();
                    std::cout << "RuntimeGraph dispatchTask() for " << node_ptr->getName() << " took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
                }

                std::cout << "Exiting task loop for node: " << node_ptr->getName() << "\n";
            });
        }
    }

    // // Copy Constructor.
    // // Technically, unnecessary since compiler will generate this.
    RuntimeGraph(const RuntimeGraph&) = delete;

    // // Copy Assignment Operator.
    // // Technically, unnecessary since compiler will generate this.
    RuntimeGraph& operator=(const RuntimeGraph&) = delete;

    // // Move Constructor.
    // // Technically, unnecessary since compiler will generate this.
    RuntimeGraph(RuntimeGraph&&) noexcept = delete;

    // // Move Assignment Operator.
    // // Technically, unnecessary since compiler will generate this.
    RuntimeGraph& operator=(RuntimeGraph&&) noexcept = delete;

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
    std::unique_ptr<IDriver> lidar_driver_1 = std::make_unique<LidarDriver>(1000);
    std::unique_ptr<IDriver> lidar_driver_2 = std::make_unique<LidarDriver>(1111);

    std::unique_ptr<IRuntimeNode> lidar_node_1 = std::make_unique<LidarNode>("lidar_node_1", std::move(lidar_driver_1), 10.0);
    // lidar_driver_1 is nullptr after move, so cannot be used directly hereafter.
    std::unique_ptr<IRuntimeNode> lidar_node_2 = std::make_unique<LidarNode>("lidar_node_2", std::move(lidar_driver_2), 20.0);
    // lidar_driver_2 is nullptr after move, so cannot be used directly hereafter.

    RuntimeGraph runtime_graph(std::thread::hardware_concurrency());
    runtime_graph.registerNode(std::move(lidar_node_1));
    // lidar_node_1 are nullptr after move, so cannot be used directly hereafter.
    runtime_graph.registerNode(std::move(lidar_node_2));
    // lidar_node_2 are nullptr after move, so cannot be used directly hereafter.
    runtime_graph.run();

    // Wait for user input to prevent the program from exiting immediately.
    std::cin.get();

    return 0;
}
