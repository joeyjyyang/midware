// Prevent multiple inclusions of the same header file, which can lead to compilation errors due to redefinitions.
#pragma once

#include <string>

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
