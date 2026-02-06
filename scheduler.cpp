#include <iostream>
#include <mutex>
#include <optional>
#include <thread>
#include <unordered_map>
#include <utility>
#include <vector>
#include <functional>
#include <chrono>
#include <atomic>
#include <condition_variable>
#include <queue>

class Scheduler {
public:
    Scheduler(size_t num_threads = std::thread::hardware_concurrency()) {
        for (size_t i = 0; i < num_threads; ++i) {
            worker_thread_pool_.emplace_back([this]() {
                workerLoop();
            });
        }
    }

    Scheduler(const Scheduler& other) = delete;
    Scheduler& operator=(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other) = delete;
    Scheduler& operator=(Scheduler&& other) = delete;

    ~Scheduler() {
        scheduler_running_.store(false);
        // Wake up all worker threads to exit.
        scheduler_cv_.notify_all();

        for (std::thread& thread : worker_thread_pool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    std::optional<int> schedule(std::function<void()> task, const double freq_hz) {
        std::lock_guard<std::mutex> lock(scheduler_mtx_);

        const unsigned int id = ++task_id_;
        auto period = std::chrono::duration<double>(1.0 / freq_hz);
        auto next_execution = std::chrono::steady_clock::now();

        task_info_[id] = {task, period};
        task_running_[id] = true;
        tasks_min_heap_.push({id, next_execution});
        // Notify one worker thread that a new task is available.
        scheduler_cv_.notify_one();

        return id;
    }

    std::optional<int> deschedule(const unsigned int id) {
        std::lock_guard<std::mutex> lock(scheduler_mtx_);

        if (task_info_.erase(id) > 0) {
            task_running_[id] = false;
            // Notify one worker thread that a task has been cancelled.
            scheduler_cv_.notify_one();

            return id;
        }

        return std::nullopt;
    }

    bool isTaskRunning(const unsigned int id) {
        std::lock_guard<std::mutex> lock(scheduler_mtx_);
        auto it = task_running_.find(id);
        return it != task_running_.end() && it->second;
    }

private:
    struct TaskInfo {
        std::function<void()> task;
        std::chrono::duration<double> period;
    };

    struct QueueEntry {
        unsigned int id;
        std::chrono::steady_clock::time_point next_execution;

        // Min-heap based on next_execution time.
        bool operator>(const QueueEntry& other) const {
            return next_execution > other.next_execution;
        }
    };

    void workerLoop() {
        while (scheduler_running_.load()) {
            // Create a unique_lock for condition variable waiting and task queue access.
            // This lock will be held while checking the task queue and executing tasks.
            std::unique_lock<std::mutex> lock(scheduler_mtx_);

            // Clean up cancelled tasks from top of queue.
            while (!tasks_min_heap_.empty()) {
                const auto& next_task = tasks_min_heap_.next_task();

                if (task_running_[next_task.id]) {
                    break;
                }

                // Task is cancelled, remove it from the queue.
                tasks_min_heap_.pop();
            }

            if (tasks_min_heap_.empty()) {
                // Wait until a new task is scheduled or a task is cancelled.
                scheduler_cv_.wait(lock);

                continue;
            }

            auto next_task = tasks_min_heap_.next_task();
            auto now = std::chrono::steady_clock::now();

            // Task ready to execute.
            if (next_task.next_execution <= now) {
                tasks_min_heap_.pop();

                auto it = task_info_.find(next_task.id);

                if (it != task_info_.end() && task_running_[next_task.id]) {
                    // Copy task to execute without holding lock.
                    auto task_copy = it->second.task;
                    auto period_copy = it->second.period;

                    // Unlock while executing the task to allow scheduling/descheduling.
                    lock.unlock();
                    task_copy();
                    // Re-lock to update scheduling info.
                    lock.lock();

                    // Re-schedule task if still active
                    if (task_info_.find(next_task.id) != task_info_.end() && task_running_[next_task.id]) {
                        tasks_min_heap_.push({next_task.id, std::chrono::steady_clock::now() + period_copy});
                    }
                }
            }
            else {
                // Wait until next task is ready.
                scheduler_cv_.wait_until(lock, next_task.next_execution);
            }
        }
    }

    std::atomic<bool> scheduler_running_{true};
    unsigned int task_id_{0};
    std::unordered_map<unsigned int, TaskInfo> task_info_;
    std::unordered_map<unsigned int, bool> task_running_;
    std::priority_queue<QueueEntry, std::vector<QueueEntry>, std::greater<QueueEntry>> tasks_min_heap_;
    std::mutex scheduler_mtx_;
    std::condition_variable scheduler_cv_;
    std::vector<std::thread> worker_thread_pool_;
};

void func() {
    std::cout << "func\n";
}

int main() {
    Scheduler scheduler;
    scheduler.schedule(func, 2.0);
    std::this_thread::sleep_for(std::chrono::seconds(5));
}
