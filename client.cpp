#include <iostream>
#include <string>

#include <zmq.hpp>

class Client
{
public:
    Client(const std::string address) : address_(address), socket_(zmq::socket_t{context_, zmq::socket_type::req})
    {
        socket_.connect(address);
    }

    void sendRequest(const std::string request)
    {
        //zmq::message_t{"test"};
        std::cout << "Client sending: " << request << "\n";
        socket_.send(zmq::buffer(request), zmq::send_flags::none);
        
        zmq::message_t reply{};
        const auto result = socket_.recv(reply, zmq::recv_flags::none);

        std::cout << "Client received: " << reply.to_string() << "\n";
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_{};
    std::string address_{};
};

int main()
{
    const std::string address{"tcp://localhost:5555"};

    Client client(address);
    client.sendRequest("test");

    return 0;
}
