#include <iostream>

auto myFunc() -> int {
  return 7;
}

auto main() -> void {
  auto result = 0;
  result = myFunc();
  std::cout << result;
}