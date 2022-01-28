#include <iostream>

auto myFunc(int a, int b) -> int {
  return a + b;
}

auto main() -> void {
  auto result = 0;
  result = myFunc(3, 4);
  std::cout << result;
}