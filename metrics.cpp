#include <iostream>
#include <queue>
#include <vector>

struct Metrics {
  std::string id_;
  double score_;
};

auto metrics_score_max_cmp = [](const Metrics& a, const Metrics& b) {
    return a.score_ < b.score_; 
    // "less" means b is 'greater', so top() will have highest score
};

auto metrics_score_min_cmp = [](const Metrics& a, const Metrics& b) {
    return a.score_ > b.score_; 
    // "greater" means b is 'smaller', so top() will have lowest score
};

int main(int argc, char* argv[]) {
  // NOTE: Equivalent to std::priority_queue<int, std::vector<int>, std::less<int>> max_heap;
  std::priority_queue<int> nums_max_heap;
  std::priority_queue<int, std::vector<int>, std::greater<int>> nums_min_heap;

  // Time: O(log(n))
  nums_max_heap.push(2);
  nums_max_heap.push(1);
  nums_max_heap.push(7);
  nums_max_heap.push(3);
  nums_min_heap.push(2);
  nums_min_heap.push(1);
  nums_min_heap.push(7);
  nums_min_heap.push(3);

  std::cout << "Nums max heap: \n";

  while (!nums_max_heap.empty()) {
    int num = nums_max_heap.top();
    nums_max_heap.pop();

    std::cout << num << "\n";
  }

  std::cout << "Nums min heap: \n";

  while (!nums_min_heap.empty()) {
    int num = nums_min_heap.top();
    nums_min_heap.pop();

    std::cout << num << "\n";
  }

  std::priority_queue<Metrics, std::vector<Metrics>, decltype(metrics_score_max_cmp)> metrics_max_heap(metrics_score_max_cmp);
  std::priority_queue<Metrics, std::vector<Metrics>, decltype(metrics_score_min_cmp)> metrics_min_heap(metrics_score_min_cmp);

  // Time: O(log(n))
  metrics_max_heap.push({"", 3.14});
  metrics_max_heap.push({"", 1.41});
  metrics_max_heap.push({"", 2.71});
  metrics_min_heap.push({"", 3.14});
  metrics_min_heap.push({"", 1.41});
  metrics_min_heap.push({"", 2.71});

  std::cout << "Nums max heap: \n";

  while (!metrics_max_heap.empty()) {
    Metrics metrics = metrics_max_heap.top();
    metrics_max_heap.pop();

    std::cout << metrics.score_ << "\n";
  }

  std::cout << "Nums min heap: \n";

  while (!metrics_min_heap.empty()) {
    Metrics metrics = metrics_min_heap.top();
    metrics_min_heap.pop();

    std::cout << metrics.score_ << "\n";
  }

  return 0;
}