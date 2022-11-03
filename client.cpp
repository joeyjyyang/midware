#include "headers.hpp"

class Client
{
public:
    // User must use default constructor defined below.
    Client() = delete;

    Client(zmq::context_t& context, const std::string endpoint) : endpoint_(endpoint), socket_(zmq::socket_t{context, zmq::socket_type::req})
    {
        socket_.connect(endpoint_);
    }

    // zmq::context_t copy constructor is deleted.
    //Client(const Client& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    //Client& operator=(const Client& other) = delete;

    // zmq::context_t move constructor is deleted.
    //Client(Client&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    //Client& operator=(Client&& other) noexcept = delete;

    void sendRequestReceiveReply(const std::string request)
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
    zmq::socket_t socket_;
    std::string endpoint_;
};
