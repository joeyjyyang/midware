#include "IDriver.hpp"
#include "IRuntimeNode.hpp"
#include "LidarDriver.hpp"
#include "LidarNode.hpp"
#include "RuntimeGraph.hpp"

int main (int argc, char* argv[]) {
    std::unique_ptr<IDriver> lidar_driver_1 = std::make_unique<LidarDriver>(1000);
    std::unique_ptr<IDriver> lidar_driver_2 = std::make_unique<LidarDriver>(1111);

    std::unique_ptr<IRuntimeNode> lidar_node_1 = std::make_unique<LidarNode>("lidar_node_1", std::move(lidar_driver_1), 10.0);
    // lidar_driver_1 is nullptr after move, so cannot be used directly hereafter.
    std::unique_ptr<IRuntimeNode> lidar_node_2 = std::make_unique<LidarNode>("lidar_node_2", std::move(lidar_driver_2), 20.0);
    // lidar_driver_2 is nullptr after move, so cannot be used directly hereafter.

    RuntimeGraph runtime_graph(std::thread::hardware_concurrency());
    std::optional<std::string> error_1 = runtime_graph.registerNode(std::move(lidar_node_1));

    if (error_1) {
        std::cerr << "Error registering node: " << *error_1 << "\n";
    }

    // lidar_node_1 are nullptr after move, so cannot be used directly hereafter.
    std::optional<std::string> error_2 = runtime_graph.registerNode(std::move(lidar_node_2));

    if (error_2) {
        std::cerr << "Error registering node: " << *error_2 << "\n";
    }

    // lidar_node_2 are nullptr after move, so cannot be used directly hereafter.
    runtime_graph.run();

    // Wait for user input to prevent the program from exiting immediately.
    std::cin.get();

    return 0;
}
