#include <iostream>

__declspec(naked) auto add(int a, int b) -> int {
  __asm {
    mov eax, [esp + 4]
    add eax, [esp + 8]
    ret
  }
}

auto main() -> void {
  std::cout << add(3, 4);
}