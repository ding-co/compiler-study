#include <windwos.h>
#include <iostream>

auto copy2ExecutableMemory(unsigned char codes[], size_t size) -> void* {
  auto memory = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
  memcpy(memory, codes, size);
  DWORD nouse;
  VirtualProtect(memory, size, PAGE_EXECUTE_rEAD, &nouse);
  return memory;
}

auto main() -> void {
  unsigned char codes[] = {
    0x8B, 0x44, 0x24, 0x04, // mov eax, [esp + 4]
    0x03, 0x44, 0x24, 0x08, // add eax, [esp + 8]
    0xC3                    // ret
  };
  auto executableMemory = copy2ExecutableMemory(codes, sizeof codes);
auto myFunc = static_cast(*)(int, int)>(executableMemory);
std::cout << myFunc(3, 4);
}