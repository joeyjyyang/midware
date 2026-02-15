#include <algorithm>
#include <cstddef>
#include <iostream>

class Buffer {
private:
    int* data_;
    size_t size_;

public:
    int* data() {
        return data_;
    }

    const int* data() const {
        return data_;
    }

    size_t size() const {
        return size_;
    }

    Buffer(size_t size) : data_(new int[size]), size_(size) {
        std::cout << "Constructor.\n";
    }

    ~Buffer() {
        std::cout << "Destructor.\n";
        delete[] data_;
    }

    Buffer(const Buffer& other) : data_(new int[other.size_]), size_(other.size_) {
        std::cout << "Copy constructor.\n";
        std::copy(other.data_, other.data_ + size_, data_);
    }

    Buffer& operator=(const Buffer& other) {
        std::cout << "Copy assignment operator.\n";

        if (this != &other) {
            // Allocate new memory first for exception safety.
            int* new_data = new int[other.size_];
            std::copy(other.data_, other.data_ + other.size_, new_data);
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
        }

        return *this;
    }

    // Good practice: Marking this noexcept enables optimal move semantics and prevents fallback to copy operations in standard containers.
    Buffer(Buffer&& other) noexcept : data_(other.data_), size_(other.size_) {
        std::cout << "Move constructor.\n";
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // Good practice: Marking this noexcept enables optimal move semantics and prevents fallback to copy operations in standard containers.
    Buffer& operator=(Buffer&& other) noexcept {
        std::cout << "Move assignment operator.\n";

        if (this != &other) {
            delete[] data_;
            data_ = other.data_;
            size_ = other.size_;
            other.data_ = nullptr;
            other.size_ = 0;
        }

        return *this;
    }
};

int main() {
    // Constructor.
    Buffer buf1(5);
    // i is size_t, so i * 10 promotos to size_t; therefore, need to static_cast to int to avoid narrowing conversion warning.
    for (size_t i = 0; i < buf1.size(); ++i) buf1.data()[i] = static_cast<int>(i * 10);

    // Copy constructor.
    // Can also do Buffer buf2(buf1); but using copy initialization syntax to demonstrate that it also works.
    Buffer buf2 = buf1;

    // Constructor.
    Buffer buf4(buf2.size());
    // Copy assignment operator.
    buf4 = buf2;

    // Move constructor.
    Buffer buf3 = std::move(buf1);

    // Constructor.
    Buffer buf5(buf3.size());
    // Move assignment operator.
    buf5 = std::move(buf3);

    return 0;
}
