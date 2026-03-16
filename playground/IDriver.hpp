// Prevent multiple inclusions of the same header file, which can lead to compilation errors due to redefinitions.
#pragma once

#include <cstdint>

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
