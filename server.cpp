#include "headers.hpp"

class Server
{
public:
    // User must use default constructor defined below.
    Server() = delete;

    Server(zmq::context_t& context, const std::string endpoint, const std::function<std::string(std::string)>& callback) : endpoint_(endpoint), callback_(callback), socket_(zmq::socket_t{context, zmq::socket_type::rep})
    {
        socket_.bind(endpoint_);
        spin();
    }

    // zmq::context_t copy constructor is deleted.
    //Server(const Server& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    //Server& operator=(const Server& other) = delete;

    // zmq::context_t move constructor is deleted.
    //Server(Server&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    //Server& operator=(Server&& other) noexcept = delete;

    void spin()
    {
        while (true)
        {
            zmq::message_t request{};
            const auto result = socket_.recv(request, zmq::recv_flags::none);
            const std::string deserialized_request{request.to_string()}; //{deserialize(request.to_string())};
            std::cout << "Server received: " << deserialized_request << " from client.\n";

            // Do some processing.
            const std::string reply = callback_(deserialized_request);

            const std::string serialized_reply{reply}; //{serialize(reply)};
            std::cout << "Server sending: " << serialized_reply << " to client.\n";
            socket_.send(zmq::buffer(serialized_reply), zmq::send_flags::none);
        }
    }

private:
    zmq::socket_t socket_;
    std::string endpoint_;
    std::function<std::string(std::string)> callback_;
};
