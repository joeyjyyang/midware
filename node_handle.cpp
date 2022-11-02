#include <zmq.hpp>

#include "subscriber.cpp"
#include "publisher.cpp"
#include "client.cpp"
#include "server.cpp"

class NodeHandle
{
public:
    NodeHandle() = default;

private:
    zmq::context_t context_{};
};

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    return 0;
}