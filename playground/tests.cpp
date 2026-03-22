// sudo apt-get install libgtest-dev libgmock-dev
// #include <gmock/gmock.h>
// #include <gtest/gtest.h>

#include <cassert>

#include "IDriver.hpp"
#include "IRuntimeNode.hpp"
#include "LidarDriver.hpp"
#include "LidarNode.hpp"
#include "RuntimeGraph.hpp"

class MockDriver : public IDriver {
public:
    void read() final {
        read_count_++;
    }

    uint32_t getUuid() const final {
        return 0;
    }

    int getReadCount() const {
        return read_count_;
    }

private:
    int read_count_{0};
};

class MockNode : public IRuntimeNode {
public:
    explicit MockNode(const std::string& name, const double frequency) : name_(name), frequency_(frequency) {}

    void publish() final {
        publish_count_++;
    }

    void execute() final {
        execute_count_++;
    }

    std::string getName() const final {
        return name_;
    }

    double getFrequency() const final {
        return frequency_;
    }

    int getPublishCount() const {
        return publish_count_;
    }

    int getExecuteCount() const {
        return execute_count_;
    }

private:
    std::string name_;
    double frequency_;
    int publish_count_{0};
    int execute_count_{0};
};

void test_lidar_node_calls_driver_read() {
    // Dependency injection of a mock driver into LidarNode.
    std::unique_ptr<IDriver> mock_driver = std::make_unique<MockDriver>();
    // Idiomatically, we want to keep ownership of the mock driver in the test function to verify its state after the node executes, so we capture a raw pointer to it before moving it into the node.
    MockDriver* mock_ptr = static_cast<MockDriver*>(mock_driver.get());

    LidarNode node("test_node", std::move(mock_driver), 10.0);
    node.execute();

    assert(mock_ptr->getReadCount() == 1);
    std::cout << "PASS: test_lidar_node_calls_driver_read\n";
}

void test_register_null_node_returns_error() {
    RuntimeGraph& graph = RuntimeGraph::getInstance(1);
    std::optional<std::string> error = graph.registerNode(nullptr);

    assert(error.has_value());
    assert(error->find("null") != std::string::npos);
    std::cout << "PASS: test_register_null_node_returns_error\n";
}

void test_register_duplicate_node_returns_error() {
    RuntimeGraph& graph = RuntimeGraph::getInstance(1);
    std::unique_ptr<IRuntimeNode> mock_1 = std::make_unique<MockNode>("node_1", 10.0);
    std::unique_ptr<IRuntimeNode> mock_2 = std::make_unique<MockNode>("node_1", 10.0);

    graph.registerNode(std::move(mock_1));
    std::optional<std::string> error = graph.registerNode(std::move(mock_2));

    assert(error.has_value());
    assert(error->find("duplicate") != std::string::npos);
    std::cout << "PASS: test_register_duplicate_node_returns_error\n";
}

void test_dispatch_task_after_stop_returns_error() {
    RuntimeGraph& graph = RuntimeGraph::getInstance(1);
    graph.stop();

    std::optional<std::string> error = graph.dispatchTask([]() {});
    assert(error.has_value());
    std::cout << "PASS: test_dispatch_task_after_stop_returns_error\n";
}

int main(int argc, char* argv[]) {
    test_lidar_node_calls_driver_read();
    test_register_null_node_returns_error();
    test_register_duplicate_node_returns_error();
    test_dispatch_task_after_stop_returns_error();

    return 0;
}
