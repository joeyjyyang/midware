#include <iostream>
#include <mutex>
#include <thread>
#include <vector>

void func() {
  std::cout << "func\n";
}

int main(int argc, char* argv[]) {
  std::thread worker_thread = std::thread([]() {
    while (true) {
      func();
    }
  });

  worker_thread.join();

  return 0;
}
