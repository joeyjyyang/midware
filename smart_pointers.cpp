#include <iostream>
#include <cassert>

template<typename T>
class SmartPointer
{

};

template<typename T>
class UniquePointer : public SmartPointer<T>
{
public:
    // 0. Default constructor.
    UniquePointer()
    {
        std::cout << "Default constructor 1.\n";
    }

    // 0. Default constructor.
    UniquePointer(T* obj)
    {
        std::cout << "Default constructor 2.\n";

        raw_pointer_ = obj;
    }

    // 1. Copy constructor.
    UniquePointer(const UniquePointer& other) = delete;

    // 2. Copy assignment operator.
    UniquePointer& operator=(const UniquePointer& other) = delete;

    // 3. Move constructor.
    UniquePointer(UniquePointer&& other)
    {
        std::cout << "Move constructor.\n";

        if (raw_pointer_ != other.raw_pointer_)
        {
            raw_pointer_ = other.raw_pointer_;
            other.raw_pointer_ = nullptr;
        }
    }

    // 4. Move assignment operator.
    UniquePointer& operator=(UniquePointer&& other)
    {
        std::cout << "Move assignment operator.\n";

        if (raw_pointer_ != other.raw_pointer_)
        {
            delete raw_pointer_;
            raw_pointer_ = other.raw_pointer_;
            other.raw_pointer_ = nullptr;
        }

        return *this;
    }

    // 5. Destructor.
    ~UniquePointer()
    {
        delete raw_pointer_;
    }

    bool operator!=(const UniquePointer& other)
    {
        if (raw_pointer_ != other.raw_pointer_)
        {
            return true;
        }
        else
        {
            return false;
        }
    }

    bool operator==(const UniquePointer& other)
    {
        return !(*this != other);
    }

    operator bool()
    {

    }

    T operator*()
    {
        return *raw_pointer_;
    }

    T operator->()
    {
        return raw_pointer_;
    }

    void reset()
    {
        delete raw_pointer_;
    }

    void reset(T* obj)
    {
        delete raw_pointer_;
        raw_pointer_ = obj;
    }

private:
    T* raw_pointer_{nullptr};
};

int main(int argc, char* argv[])
{
    UniquePointer<int> unique_pointer_a; // 0.
    UniquePointer<int> unique_pointer_b(new int{5}); //0.
    //UniquePointer<int> unique_pointer_c(unique_pointer_b); // 1.
    //unique_pointer_a = unique_pointer_b; // 2.
    UniquePointer<int> unique_pointer_d(std::move(unique_pointer_b));
    unique_pointer_a = std::move(unique_pointer_d);

    assert(unique_pointer_a != unique_pointer_b);
    assert(!(unique_pointer_a == unique_pointer_b));

    if (unique_pointer_b)
    {
        std::cout << "True.\n";
    }
    else
    {
        std::cout << "False.\n";
    }

    if (unique_pointer_a && unique_pointer_b)
    {
        std::cout << "True.\n";
    }
    else
    {
        std::cout << "False.\n";
    }

    return 0;
}
