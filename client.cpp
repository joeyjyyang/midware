#include <iostream>
#include <string>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

class Client
{
public:
    // User must use default constructor defined below.
    Client() = delete;

    Client(const std::string endpoint) : endpoint_(endpoint), socket_(zmq::socket_t{context_, zmq::socket_type::req})
    {
        socket_.connect(endpoint_);
    }

    // zmq::context_t copy constructor is deleted.
    Client(const Client& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    Client& operator=(const Client& other) = delete;

    // zmq::context_t move constructor is deleted.
    Client(Client&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    Client& operator=(Client&& other) noexcept = delete;

    void sendRequest(const std::string request)
    {
        const std::string serialized_request{request}; //{serialize(request)};
        std::cout << "Client sending: " << serialized_request << " to server.\n";
        socket_.send(zmq::buffer(serialized_request), zmq::send_flags::none);

        zmq::message_t reply{};
        const auto result = socket_.recv(reply, zmq::recv_flags::none);
        const std::string deserialized_reply{reply.to_string()}; //{deserialize(reply.to_string())};
        std::cout << "Client received: " << deserialized_reply << " from server.\n";
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_;
    std::string endpoint_;
};

/*int main(int argc, char* argv[])
{
    const std::string endpoint{"tcp://localhost:5555"};

    Client client1(endpoint);

    int num_requests{0};

    std::cout << "Enter number of requests to send: \n";
    std::cin >> num_requests;

    for (int i = 0; i < num_requests; i++)
    {
        client1.sendRequest("Message #" + std::to_string(i));
    }

    return 0;
}*/
