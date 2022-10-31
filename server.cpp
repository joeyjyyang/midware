#include <functional>
#include <iostream>
#include <string>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

template <typename TCallbackReturnType>
class Server
{
public:
    // Alias for callback function object/functor type.
    using CallbackType = std::function<TCallbackReturnType(std::string)>;

    // User must use default constructor defined below.
    Server() = delete;

    Server(const std::string endpoint, const CallbackType& callback) : endpoint_(endpoint), callback_(callback), socket_(zmq::socket_t{context_, zmq::socket_type::rep})
    {
        socket_.bind(endpoint_);
        spin();
    }

    // zmq::context_t copy constructor is deleted.
    Server(const Server& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    Server& operator=(const Server& other) = delete;

    // zmq::context_t move constructor is deleted.
    Server(Server&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    Server& operator=(Server&& other) noexcept = delete;

    void spin()
    {
        while (true)
        {
            zmq::message_t request{};
            const auto result = socket_.recv(request, zmq::recv_flags::none);
            const std::string deserialized_request{request.to_string()}; //{deserialize(reply.to_string())};
            std::cout << "Server received: " << deserialized_request << " from client.\n";

            // Do some processing.
            const TCallbackReturnType& reply = callback_(deserialized_request);

            const std::string serialized_reply{reply}; //{serialize(reply)};
            std::cout << "Server sending: " << serialized_reply << " to client.\n";
            socket_.send(zmq::buffer(serialized_reply), zmq::send_flags::none);
        }
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_;
    std::string endpoint_;
    CallbackType callback_;
};

/*std::string testCallback(std::string request)
{
    // Do some work.

    std::cout << "Processing request from client: " << request << "...\n";
    const std::string reply = "Processed " + request;

    return reply;
}

int main()
{
    const std::string endpoint = "tcp://*:5555";
    Server<std::string> server(endpoint, testCallback);

    return 0;
}*/
