#include <iostream>

auto myFunc(int a, int b) -> int {
  int c = 3;
  int d = 4;
  return a + b + c + d;
}

auto main() -> void {
  std::cout << myFunc(1, 2);
}