#include <iostream>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

class Subscriber
{
public:
    Subscriber(const std::string endpoint) : endpoint_(endpoint), socket_(zmq::socket_t{context_, zmq::socket_type::sub})
    {
        socket_.connect(endpoint_);
        spin();
    }

    void spin()
    {
        while (true)
        {
            zmq::message_t message{};
            const auto result = socket_.recv(message, zmq::recv_flags::none);
            const std::string deserialized_message{message.to_string()}; //{deserialize(message.to_string())};
            std::cout << "Subscriber received: " << deserialized_message << ".\n";
        }
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_;
    std::string endpoint_;
};

int main(int argc, char* argv[])
{
    const std::string endpoint{"tcp://localhost:5556"};

    Subscriber subscriber(endpoint);

    return 0;
}
