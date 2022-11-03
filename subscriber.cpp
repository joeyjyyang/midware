#include "headers.hpp"

class Subscriber
{
public:
    // User must use default constructor defined below.
    Subscriber() = delete;

    Subscriber(zmq::context_t& context, const std::string endpoint, const std::function<std::string(std::string)>& callback) : endpoint_(endpoint), callback_(callback), socket_(zmq::socket_t{context, zmq::socket_type::sub})
    {
        socket_.connect(endpoint_);
        socket_.set(zmq::sockopt::subscribe, ""); // Deprecated: socket_.setsockopt(ZMQ_SUBSCRIBE, "", 0)
        spin();
    }

    // zmq::context_t copy constructor is deleted.
    //Subscriber(const Subscriber& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    //Subscriber& operator=(const Subscriber& other) = delete;

    // zmq::context_t move constructor is deleted.
    //Subscriber(Subscriber&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    //Subscriber& operator=(Subscriber&& other) noexcept = delete;

    void spin()
    {
        while (true)
        {
            zmq::message_t message{};
            const auto result = socket_.recv(message, zmq::recv_flags::none);
            const std::string deserialized_message{message.to_string()}; //{deserialize(message.to_string())};
            std::cout << "Subscriber received: " << deserialized_message << ".\n";

            // Do some processing.
            const std::string reply = callback_(deserialized_message);
        }
    }

private:
    zmq::socket_t socket_;
    std::string endpoint_;
    std::function<std::string(std::string)> callback_;
};
