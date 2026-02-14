#include <cstddef>
#include <algorithm>

class Buffer {
private:
    int* data_;
    size_t size_;

public:
    // Accessors for data_
    int* data() { return data_; }
    const int* data() const { return data_; }
    size_t size() const { return size_; }
    // Constructor
    Buffer(size_t size)
        : data_(new int[size]), size_(size) {}

    // Destructor
    ~Buffer() {
        delete[] data_;
    }

    // Copy constructor
    Buffer(const Buffer& other)
        : data_(new int[other.size_]), size_(other.size_) {
        std::copy(other.data_, other.data_ + size_, data_);
    }

    // Copy assignment
    Buffer& operator=(const Buffer& other) {
        if (this != &other) {
            // Allocate new memory first for exception safety
            int* new_data = new int[other.size_];
            std::copy(other.data_, other.data_ + other.size_, new_data);
            delete[] data_;
            data_ = new_data;
            size_ = other.size_;
        }
        return *this;
    }

    // Move constructor
    // Good practice: Marking this noexcept enables optimal move semantics and prevents fallback to copy operations in standard containers.
    Buffer(Buffer&& other) noexcept
        : data_(other.data_), size_(other.size_) {
        other.data_ = nullptr;
        other.size_ = 0;
    }

    // Move assignment
    // Good practice: Marking this noexcept enables optimal move semantics and prevents fallback to copy operations in standard containers.
    Buffer& operator=(Buffer&& other) noexcept {
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

// Simple test for Buffer class
#include <iostream>

int main() {
    // Construct Buffer
    Buffer buf1(5);
    for (size_t i = 0; i < buf1.size(); ++i) buf1.data()[i] = static_cast<int>(i * 10);

    // Copy constructor
    Buffer buf2 = buf1;
    std::cout << "buf2 after copy: ";
    for (size_t i = 0; i < buf2.size(); ++i) std::cout << buf2.data()[i] << ' ';
    std::cout << std::endl;

    // Move constructor
    Buffer buf3 = std::move(buf1);
    std::cout << "buf3 after move: ";
    for (size_t i = 0; i < buf3.size(); ++i) std::cout << buf3.data()[i] << ' ';
    std::cout << std::endl;

    // Copy assignment
    Buffer buf4(buf2.size());
    buf4 = buf2;
    std::cout << "buf4 after copy assignment: ";
    for (size_t i = 0; i < buf4.size(); ++i) std::cout << buf4.data()[i] << ' ';
    std::cout << std::endl;

    // Move assignment
    Buffer buf5(buf3.size());
    buf5 = std::move(buf3);
    std::cout << "buf5 after move assignment: ";
    for (size_t i = 0; i < buf5.size(); ++i) std::cout << buf5.data()[i] << ' ';
    std::cout << std::endl;

    return 0;
}
