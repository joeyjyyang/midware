#include "node_handle.cpp"

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    Publisher publisher = node_handle.createPublisher("tcp://*:5556");
    
    while (true)
    {
        publisher.publishMessage("test");
    }

    return 0;
}
