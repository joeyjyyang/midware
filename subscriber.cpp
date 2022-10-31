#include <iostream>
#include <string>

#include <zmq.hpp>

//#include "deserializer.cpp"
//#include "serializer.hpp"

template <typename TCallbackReturnType>
class Subscriber
{
public:
    // Alias for callback function object/functor type.
    using CallbackType = std::function<TCallbackReturnType(std::string)>;

    // User must use default constructor defined below.
    Subscriber() = delete;

    Subscriber(const std::string endpoint, const CallbackType& callback) : endpoint_(endpoint), callback_(callback), socket_(zmq::socket_t{context_, zmq::socket_type::sub})
    {
        socket_.connect(endpoint_);
        socket_.set(zmq::sockopt::subscribe, ""); // Depra socket_.setsockopt(ZMQ_SUBSCRIBE, "", 0)
        spin();
    }

    // zmq::context_t copy constructor is deleted.
    Subscriber(const Subscriber& other) = delete;

    // zmq::context_t copy assignment operator is deleted.
    Subscriber& operator=(const Subscriber& other) = delete;

    // zmq::context_t move constructor is deleted.
    Subscriber(Subscriber&& other) noexcept = delete;

    // zmq::context_t move assignment operator is deleted.
    Subscriber& operator=(Subscriber&& other) noexcept = delete;

    void spin()
    {
        while (true)
        {
            zmq::message_t message{};
            const auto result = socket_.recv(message, zmq::recv_flags::none);
            const std::string deserialized_message{message.to_string()}; //{deserialize(message.to_string())};
            std::cout << "Subscriber received: " << deserialized_message << ".\n";

            // Do some processing.
            const TCallbackReturnType& reply = callback_(deserialized_message);
        }
    }

private:
    zmq::context_t context_{};
    zmq::socket_t socket_;
    std::string endpoint_;
    CallbackType callback_;
};

/*std::string testCallback(std::string message)
{
    // Do some work.

    std::cout << "Processing message from publisher: " << message << "...\n";
    const std::string reply = "Processed " + message;

    return reply;
}

int main(int argc, char* argv[])
{
    const std::string endpoint{"tcp://localhost:5556"};

    Subscriber<std::string> subscriber(endpoint, testCallback);

    return 0;
}*/
