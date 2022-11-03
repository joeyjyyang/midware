#include "subscriber.cpp"
#include "publisher.cpp"
#include "client.cpp"
#include "server.cpp"

class NodeHandle
{
public:
    NodeHandle() = default;

    Publisher createPublisher(const std::string endpoint)
    {
        return Publisher(context_, endpoint);
    }

    Subscriber createSubscriber(const std::string endpoint, const std::function<std::string(std::string)>& callback)
    {
        return Subscriber(context_, endpoint, callback);
    }

    Server createServer(const std::string endpoint, const std::function<std::string(std::string)>& callback)
    {
        return Server(context_, endpoint, callback);
    }

    Client createClient(const std::string endpoint)
    {
        return Client(context_, endpoint);
    }

private:
    zmq::context_t context_{};
};
