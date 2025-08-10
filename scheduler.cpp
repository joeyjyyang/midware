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
        running_.store(false);

        for (std::thread& thread : worker_thread_pool_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    std::optional<int> schedule(std::function<void()> task, const double freq_hz) {
        std::lock_guard<std::mutex> lock(schedule_mtx_);

        const unsigned int id = ++task_id_;
        schedule_[id] = {task, freq_hz};

        worker_thread_pool_.emplace_back([this, id]() {
            auto [func, freq] = schedule_[id];
            auto period = std::chrono::duration<double>(1.0 / freq);

            while (running_.load()) {
                func();
                std::this_thread::sleep_for(period);
            }
        });

        return id;
    }

    std::optional<int> deschedule(const unsigned int id) {
        std::lock_guard<std::mutex> lock(schedule_mtx_);

        if (schedule_.erase(id) > 0) {
            return id;
        }

        return std::nullopt;
    }

private:
    std::atomic<bool> running_{true};
    unsigned int task_id_{0};
    std::unordered_map<int, std::pair<std::function<void()>, double>> schedule_;
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
