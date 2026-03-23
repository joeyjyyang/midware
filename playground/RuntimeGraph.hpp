// Prevent multiple inclusions of the same header file, which can lead to compilation errors due to redefinitions.
#pragma once

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <queue>
#include <string>
#include <thread>
#include <vector>

#include "IDriver.hpp"
#include "IRuntimeNode.hpp"

class RuntimeGraph {
public:
    // Static methods can only operate on static member variables, not instance variables.
    static void initialize(const unsigned int num_threads) {
        if (initialized_.load()) {
            std::cout << "RuntimeGraph is already initialized. Ignoring subsequent initialization call.\n";

            return;
        }

        num_threads_ = num_threads;
        initialized_.store(true);
    }

    // Static methods can only operate on static member variables, not instance variables.    
    // As per Meyers Singleton pattern, return static local variable as reference instead of pointer.
    static RuntimeGraph& getInstance() {
        if (!initialized_.load()) {
            std::cout << "RuntimeGraph is not initialized.\n";

            throw std::runtime_error("RuntimeGraph not initialized");
        }

        std::cout << "Getting RuntimeGraph Singleton instance...\n";

        // Static local variable is initialized only once in process, and is thread-safe; subsequent calls will return the same instance.
        static RuntimeGraph instance;

        return instance;
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

    unsigned int getInstanceCount() const {
        return instance_count_;
    }

    void stop() {
        {
            std::lock_guard<std::mutex> lock(mtx_);
            running_.store(false);
        }

        cv_.notify_all();
    }

    std::optional<std::string> registerNode(std::unique_ptr<IRuntimeNode>&& new_node) {
        if (!new_node) {
            return "Cannot register null node.";
        }

        for (const auto& node : nodes_) {
            if (node->getName() == new_node->getName()) {
                return "Cannot register node: duplicate name '" + new_node->getName() + "'.";
            }
        }

        std::cout << "Registering node: " << new_node->getName() << " with frequency: " << new_node->getFrequency() << " Hz\n";

        nodes_.emplace_back(std::move(new_node));

        return std::nullopt;
    }

    std::optional<std::string> dispatchTask(std::function<void()>&& task) {
        {
            std::lock_guard<std::mutex> lock(mtx_);

            if (!running_.load()) {
                return "RuntimeGraph is not running. Cannot dispatch task.";
            }

            task_queue_.emplace(std::move(task));
        }

        cv_.notify_one();

        return std::nullopt;
    }

    void run() {
        for (const auto& node : nodes_) {
            // Avoid dangling dereference to std::unique_ptr by capturing a raw pointer to the node in the lambda.
            IRuntimeNode* node_ptr = node.get();

            auto error = dispatchTask([node_ptr, this]() {
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

            if (error) {
                std::cerr << "Error dispatching task for node " << node_ptr->getName() << ": " << *error << "\n";
            }
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
    RuntimeGraph() {
        std::cout << "Creating RuntimeGraph with " << num_threads_ << " threads.\n";

        instance_count_++;
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
                            // Lock will automatically be released when going out of scope at the end of this block.
                            //lock.unlock.unlock()
                            return;
                        }
                    }
                }
            });
        }
    }

    std::atomic<bool> running_{false};
    // Use inline static to declare and define static member variable in the header file without violating the One Definition Rule (ODR).
    inline static std::atomic<bool> initialized_{false};
    inline static unsigned int num_threads_{0};
    inline static unsigned int instance_count_{0};

    std::mutex mtx_;
    std::condition_variable cv_;
    std::vector<std::thread> thread_pool_;
    std::queue<std::function<void()>> task_queue_;
    std::vector<std::unique_ptr<IRuntimeNode>> nodes_;
};
