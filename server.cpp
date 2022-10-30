#include <string>
#include <chrono>
#include <thread>
#include <iostream>

#include <zmq.hpp>

class Server
{
public:
    Server(const std::string address) : address_(address), socket_(zmq::socket_t{context_, zmq::socket_type::rep})
    {
        socket_.bind(address);
    }

    void run()
    {
        while (true)
        {
            zmq::message_t request;

            const auto result = socket_.recv(request, zmq::recv_flags::none);
            std::cout << "Server received: " << request.to_string() << "\n";

            // Do some processing.
            const std::string response = processRequest(request.to_string());

            std::cout << "Server sending: " << response << "\n";
            socket_.send(zmq::buffer(response), zmq::send_flags::none);
        }
    }

    const std::string processRequest(const std::string request)
    {
        return "header" + request + "footer";
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_{};
    std::string address_{};
};

int main() 
{
    const std::string address{"tcp://*:5555"};

    Server server(address);
    server.run();

    return 0;
}
