// Prevent multiple inclusions of the same header file, which can lead to compilation errors due to redefinitions.
#pragma once

#include <chrono>
#include <iostream>

#include "IDriver.hpp"

class LidarDriver : public IDriver {
public:
    // Mark single argument constructors as explicit to prevent unintended implicit conversions.
    explicit LidarDriver(const uint32_t uuid) : uuid_(uuid) {}

    // Technically, unnecessary since compiler will generate this.
    ~LidarDriver() override final = default;

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

    void read() override final {
        auto start = std::chrono::high_resolution_clock::now();

        // TODO: Add work here.

        auto end = std::chrono::high_resolution_clock::now();
        std::cout << "LidarDriver " << uuid_ << " read() took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count() << " milliseconds\n";
    }

    uint32_t getUuid() const override final {
        return uuid_;
    }

private:
    uint32_t uuid_;
};
