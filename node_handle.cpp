#include <zmq.hpp>

#include "subscriber.cpp"
#include "publisher.cpp"
#include "client.cpp"
#include "server.cpp"

class NodeHandle
{
public:
    // Alias for callback function object/functor type.
    using CallbackType = std::function<TCallbackReturnType(std::string)>;
    NodeHandle() = default;

    Publisher createPublisher(const std::string endpoint)
    {
        return Publisher(endpoint);
    }

    Subscriber createSubscriber(const std::string endpoint, const CallbackType& callback)
    {
        return Subscriber(endpoint, callback);
    }

    Server createServer(const std::string endpoint, const CallbackType& callback)
    {
        return Server(endpoint, callback);
    }

    Client createClient(const std::string endpoint)
    {
        return Client(endpoint);
    }

private:
    zmq::context_t context_{};
};

std::string testCallback(std::string message)
{
    // Do some work.

    std::cout << "Processing message from publisher: " << message << "...\n";
    const std::string reply = "Processed " + message;

    return reply;
}

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    Subscriber<std::string> subscriber("tcp://localhost:5556", testCallback);

    Publisher publisher = node_handle.createPublisher("tcp://*:5556");
    
    while (true)
    {
        publisher.publishMessage("test");
    }

    return 0;
}