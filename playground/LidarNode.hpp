// Prevent multiple inclusions of the same header file, which can lead to compilation errors due to redefinitions.
#pragma once

#include <chrono>
#include <iostream>
#include <memory>
#include <string>

#include "IDriver.hpp"
#include "IRuntimeNode.hpp"

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
