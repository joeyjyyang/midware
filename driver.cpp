#include <iostream>
#include <memory>
#include <string>

// ==============================
// Socket Interface
// ==============================
class ISocket {
public:
    ISocket() = default;
    virtual ~ISocket() = default;

    ISocket(const ISocket&) = delete;
    ISocket& operator=(const ISocket&) = delete;
    ISocket(ISocket&&) = delete;
    ISocket& operator=(ISocket&&) = delete;

    virtual bool connect() = 0;
    virtual bool poll() = 0;
    virtual void disconnect() = 0;
};

// ==============================
// Socket Protocols
// ==============================
struct TcpProtocol {
    bool connect()    { std::cout << "TCP: connect\n"; return true; }
    bool poll()       { std::cout << "TCP: poll\n"; return true; }
    void disconnect() { std::cout << "TCP: disconnect\n"; }
};

struct UdpProtocol {
    bool connect()    { std::cout << "UDP: connect\n"; return true; }
    bool poll()       { std::cout << "UDP: poll\n"; return true; }
    void disconnect() { std::cout << "UDP: disconnect\n"; }
};

// ==============================
// Templated Socket Implementation
// ==============================
template <typename Protocol>
class Socket final : public ISocket {
public:
    explicit Socket(const std::string& id) : id_(id) {
        std::cout << "Socket constructed\n";
    }

    ~Socket() override final {
        std::cout << "Socket destructed\n";
    }

    Socket(const Socket&) = delete;
    Socket& operator=(const Socket&) = delete;
    Socket(Socket&&) = delete;
    Socket& operator=(Socket&&) = delete;

    bool connect() override final {
        return protocol_.connect();
    }

    bool poll() override final {
        return protocol_.poll();
    }

    void disconnect() override final {
        protocol_.disconnect();
    }

private:
    Protocol protocol_;
    std::string id_;
};

// ==============================
// Driver Interface
// ==============================
class IDriver {
public:
    IDriver() = default;
    virtual ~IDriver() = default;

    IDriver(const IDriver&) = delete;
    IDriver& operator=(const IDriver&) = delete;
    IDriver(IDriver&&) = delete;
    IDriver& operator=(IDriver&&) = delete;

    virtual bool initialize() = 0;
    virtual bool poll() = 0;
    virtual void shutdown() = 0;
};

// ==============================
// IMU Driver Implementation
// ==============================
class ImuDriver final : public IDriver {
public:
    explicit ImuDriver(const std::string& id)
        : id_(id), socket_(std::make_unique<Socket<TcpProtocol>>(id_)) {
        std::cout << "ImuDriver constructed\n";
    }

    ~ImuDriver() override final {
        std::cout << "ImuDriver destructed\n";
    }

    ImuDriver(const ImuDriver&) = delete;
    ImuDriver& operator=(const ImuDriver&) = delete;
    ImuDriver(ImuDriver&&) = delete;
    ImuDriver& operator=(ImuDriver&&) = delete;

    bool initialize() override final {
        std::cout << "ImuDriver: initialize\n";
        return socket_->connect();
    }

    bool poll() override final {
        std::cout << "ImuDriver: poll\n";
        return socket_->poll();
    }

    void shutdown() override final {
        std::cout << "ImuDriver: shutdown\n";
        socket_->disconnect();
    }

private:
    std::unique_ptr<ISocket> socket_;
    std::string id_;
};

// ==============================
// LIDAR Driver Implementation
// ==============================
class LidarDriver final : public IDriver {
public:
    explicit LidarDriver(const std::string& id)
        : id_(id), socket_(std::make_unique<Socket<UdpProtocol>>(id_)) {
        std::cout << "LidarDriver constructed\n";
    }

    ~LidarDriver() override final {
        std::cout << "LidarDriver destructed\n";
    }

    LidarDriver(const LidarDriver&) = delete;
    LidarDriver& operator=(const LidarDriver&) = delete;
    LidarDriver(LidarDriver&&) = delete;
    LidarDriver& operator=(LidarDriver&&) = delete;

    bool initialize() override final {
        std::cout << "LidarDriver: initialize\n";
        return socket_->connect();
    }

    bool poll() override final {
        std::cout << "LidarDriver: poll\n";
        return socket_->poll();
    }

    void shutdown() override final {
        std::cout << "LidarDriver: shutdown\n";
        socket_->disconnect();
    }

private:
    std::unique_ptr<ISocket> socket_;
    std::string id_;
};

// ==============================
// Main
// ==============================
int main() {
    {
        std::cout << "Creating IMU driver\n";
        std::unique_ptr<IDriver> imu = std::make_unique<ImuDriver>("IMU_ID");
        imu->initialize();
        imu->poll();
        imu->shutdown();
    }

    std::cout << "\n";

    {
        std::cout << "Creating LIDAR driver\n";
        std::unique_ptr<IDriver> lidar = std::make_unique<LidarDriver>("LIDAR_ID");
        lidar->initialize();
        lidar->poll();
        lidar->shutdown();
    }

    return 0;
}
