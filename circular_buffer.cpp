#include <array>
#include <cassert>
#include <iostream>
#include <memory>

// Next steps:
// 1. How to add multiple items?
// 2. How to add item with different sizes?

constexpr std::size_t BUFFER_SIZE{6}; 

template <typename T, std::size_t N>
class CircularBuffer // CircularQueue, RingBuffer
{
public:
    CircularBuffer() = default;

    inline bool isFull() const
    {
        return (buffer_length_ == buffer_size_ ? true : false);
    }

    inline bool isEmpty() const
    {
        return (buffer_length_ == 0 ? true : false);
    }

    inline T read()
    {
        if (isEmpty())
        {
            assert(reader_tail_ == writer_head_);
            std::cout << "Empty; nothing to read.\n";

            return -1;
        }

        T item = buffer_[reader_tail_];
        ++reader_tail_;
        reader_tail_ %= buffer_size_;

        --buffer_length_;

        return item;
    }

    inline void write(T item)
    {
        if (isFull())
        {
            assert(reader_tail_ == writer_head_);
            std::cout << "Full; unable to write.\n";

            return;
        }

        buffer_[writer_head_] = item;
        ++writer_head_;
        writer_head_ %= buffer_size_;

        ++buffer_length_;
    }

    template <typename TFriend, std::size_t NFriend>
    friend void printBuffer(const CircularBuffer<TFriend, NFriend>& circular_buffer);

private:
    std::array<T, N> buffer_{};
    std::size_t buffer_size_{N};
    int buffer_length_{0};
    int reader_tail_{0};
    int writer_head_{0};
};

template <typename TFriend, std::size_t NFriend>
void printBuffer(const CircularBuffer<TFriend, NFriend>& circular_buffer)
{
    std::cout << "Buffer size   : " << circular_buffer.buffer_size_ << "\n";
    std::cout << "Buffer length : " << circular_buffer.buffer_length_ << "\n";
    std::cout << "Reader index  : " << circular_buffer.reader_tail_ << "\n";
    std::cout << "Writer index  : " << circular_buffer.writer_head_ << "\n";

    std::cout << "Indices       : ";
 
    for (unsigned int i = 0; i < circular_buffer.buffer_size_; ++i)
    {
        std::cout << i << " ";
    }

    std::cout << "\n";
    std::cout << "Items         : ";

    for (const auto& item : circular_buffer.buffer_)
    {
        std::cout << item << " ";
    }

    std::cout << "\n";
}

int main()
{
    CircularBuffer<int, BUFFER_SIZE> circular_buffer;

    //printBuffer(circular_buffer);

    circular_buffer.read();
    circular_buffer.write(1);
    circular_buffer.write(2);
    circular_buffer.read();
    circular_buffer.write(3);
    circular_buffer.read();
    circular_buffer.write(4);
    circular_buffer.write(5);
    circular_buffer.write(6);
    circular_buffer.read();
    circular_buffer.write(7);
    circular_buffer.read();
    circular_buffer.write(8);
    circular_buffer.write(9);
    circular_buffer.write(10);
    circular_buffer.write(11);
    circular_buffer.read();
    circular_buffer.write(12);
    circular_buffer.write(13);

    printBuffer(circular_buffer);

    return 0;
}