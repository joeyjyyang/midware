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

class Scheduler {
public:
    Scheduler() = default;
    Scheduler(const Scheduler& other) = delete;
    Scheduler& operator=(const Scheduler& other) = delete;
    Scheduler(Scheduler&& other) = delete;
    Scheduler& operator=(Scheduler&& other) = delete;

    ~Scheduler() {
        scheduler_running_.store(false);

        for (std::thread& thread : worker_thread_pool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    std::optional<int> schedule(std::function<void()> task, const double freq_hz) {
        std::lock_guard<std::mutex> lock(schedule_mtx_);

        const unsigned int id = ++task_id_;
        task_id_func_hash_table_[id] = {task, freq_hz};
        task_id_running_hash_table[id] = true;

        // Important: Capture copies of task and freq_hz to ensure they remain valid in the worker thread,
        // since we want to avoid the alternative of using locks inside the worker thread's lambda.
        auto task_copy = task;
        auto freq_copy = freq_hz;

        worker_thread_pool_.emplace_back([this, id, task_copy, freq_copy]() {
            auto period = std::chrono::duration<double>(1.0 / freq_copy);

            while (scheduler_running_.load() && isTaskRunning(id)) {
                task_copy();
                std::this_thread::sleep_for(period);
            }
        });

        return id;
    }

    std::optional<int> deschedule(const unsigned int id) {
        std::lock_guard<std::mutex> lock(schedule_mtx_);

        if (task_id_func_hash_table_.erase(id) > 0) {
            task_id_running_hash_table[id] = false;

            return id;
        }

        return std::nullopt;
    }

    bool isTaskRunning(const unsigned int id) {
        std::lock_guard<std::mutex> lock(schedule_mtx_);
        auto it = task_id_running_hash_table.find(id);

        return it != task_id_running_hash_table.end() && it->second;
    }

private:

    std::atomic<bool> scheduler_running_{true};
    unsigned int task_id_{0};
    std::unordered_map<int, std::pair<std::function<void()>, double>> task_id_func_hash_table_;
    std::unordered_map<int, bool> task_id_running_hash_table;
    std::mutex schedule_mtx_;
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
