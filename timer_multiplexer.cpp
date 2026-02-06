#include <cstdint>
#include <queue>
#include <vector>

////////////////////////////////////////////////////////////
// Hardware API
////////////////////////////////////////////////////////////

typedef void (*callback_function)();

void set_and_start_hardware_timer(int64_t ms);
int64_t get_hardware_timer_time();

////////////////////////////////////////////////////////////
// Software timer manager
////////////////////////////////////////////////////////////

struct Timer {
    int64_t expiry_time_ms;
    callback_function cb;
};

struct TimerCompare {
    bool operator()(const Timer& a, const Timer& b) const {
        return a.expiry_time_ms > b.expiry_time_ms;
    }
};

static std::priority_queue<
    Timer,
    std::vector<Timer>,
    TimerCompare
> timer_heap;

static int64_t software_time_ms = 0;
static int64_t last_programmed_timeout_ms = 0;

////////////////////////////////////////////////////////////
// Program hardware for earliest expiry
////////////////////////////////////////////////////////////

static void program_next_timer()
{
    if (timer_heap.empty()) {
        return;
    }

    int64_t next_expiry = timer_heap.top().expiry_time_ms;
    int64_t delay = next_expiry - software_time_ms;

    if (delay < 0)
        delay = 0;

    last_programmed_timeout_ms = delay;
    set_and_start_hardware_timer(delay);
}

////////////////////////////////////////////////////////////
// Public API
////////////////////////////////////////////////////////////

void set_shared_timer(int64_t timeout_ms,
                      callback_function cb)
{
    Timer t;
    t.expiry_time_ms = software_time_ms + timeout_ms;
    t.cb = cb;

    timer_heap.push(t);

    // if earliest changed → reprogram hardware
    if (timer_heap.top().expiry_time_ms == t.expiry_time_ms) {
        program_next_timer();
    }
}

////////////////////////////////////////////////////////////
// Hardware ISR
////////////////////////////////////////////////////////////

void hardware_interrupt()
{
    // advance software clock
    software_time_ms += last_programmed_timeout_ms;

    // fire all expired timers
    while (!timer_heap.empty() &&
           timer_heap.top().expiry_time_ms <= software_time_ms)
    {
        Timer t = timer_heap.top();
        timer_heap.pop();

        if (t.cb) {
            t.cb();
        }
    }

    // schedule next timeout
    program_next_timer();
}
