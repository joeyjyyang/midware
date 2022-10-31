#include <iostream>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

class Publisher
{
public:
    Publisher(const std::string endpoint) : endpoint_(endpoint), socket_(zmq::socket_t{context_, zmq::socket_type::pub})
    {
        socket_.bind(endpoint_);
    }

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

int main(int argc, char* argv[])
{
    const std::string endpoint{"tcp://*:5556"};

    Publisher publisher(endpoint);
    publisher.publishMessage("test");

    return 0;
}
