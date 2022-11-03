#include "node_handle.cpp"

std::string callback(std::string request)
{
    // Do some work.

    std::cout << "Processing request from client: " << request << "...\n";
    const std::string reply = "Processed " + request;

    return reply;
}

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    Server server = node_handle.createServer("tcp://*:5555", callback);

    return 0;
}
