#include "node_handle.cpp"

std::string callback(std::string message)
{
    // Do some work.

    std::cout << "Processing message from publisher: " << message << "...\n";
    const std::string reply = "Processed " + message;

    return reply;
}

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    Subscriber subscriber = node_handle.createSubscriber("tcp://localhost:5556", callback);

    return 0;
}
