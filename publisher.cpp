#include <iostream>
#include <string>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

class Publisher
{
public:
    // User must use default constructor defined below.
    Publisher() = delete;

    Publisher(const std::string endpoint) : endpoint_(endpoint), socket_(zmq::socket_t{context_, zmq::socket_type::pub})
    {
        socket_.bind(endpoint_);
    }

    // zmq::context_t copy constructor is deleted.
    Publisher(const Publisher& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    Publisher& operator=(const Publisher& other) = delete;

    // zmq::context_t move constructor is deleted.
    Publisher(Publisher&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    Publisher& operator=(Publisher&& other) noexcept = delete;

    void publishMessage(const std::string message)
    {
        const std::string serialized_message{message}; //{serialize(message)};
        std::cout << "Publisher sending: " << serialized_message << ".\n";
        socket_.send(zmq::buffer(serialized_message), zmq::send_flags::none);
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_;
    std::string endpoint_;
};

/*int main(int argc, char* argv[])
{
    const std::string endpoint{"tcp://*:5556"};

    Publisher publisher(endpoint);

    while (true)
    {
        publisher.publishMessage("test");
    }

    return 0;
}*/
