#include "node_handle.cpp"

int main(int argc, char* argv[])
{
    NodeHandle node_handle;

    Client client = node_handle.createClient("tcp://localhost:5555");

    int num_requests{0};

    std::cout << "Enter number of requests to send: \n";
    std::cin >> num_requests;

    for (int i = 0; i < num_requests; i++)
    {
        client.sendRequestReceiveReply("Message #" + std::to_string(i));
    }

    return 0;
}
