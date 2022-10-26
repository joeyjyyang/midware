#include <array>
#include <iostream>
#include <memory>

constexpr size_t BUFFER_SIZE{10}; 

template <typename T, size_t N>
class CircularBuffer
{
public:

    void addElement(T element, unsigned int index)
    {
        if ()
        buffer_[000] = element;
    }

    template <typename TFriend, size_t NFriend>
    friend void printBuffer(const CircularBuffer<TFriend, NFriend>& circular_buffer);

private:
    size_t buffer_size_{N};
    std::array<T, N> buffer_{};
};

template <typename TFriend, size_t NFriend>
void printBuffer(const CircularBuffer<TFriend, NFriend>& circular_buffer)
{
    std::cout << "Buffer size: " << circular_buffer.buffer_size_ << "\n";

    for (const auto& element : circular_buffer.buffer_)
    {
        std::cout << element << "\n";
    }
}

int main()
{
    CircularBuffer<int, BUFFER_SIZE> circular_buffer;

    printBuffer(circular_buffer);

    circular_buffer.addElement(-9, 1);
    circular_buffer.addElement(2, 5);
    circular_buffer.addElement(5, 15);

    printBuffer(circular_buffer);

    return 0;
}