auto copy2ExecutableMemory(unsigned char codes[], size_t size) -> void* {
  auto memory = VirtualAlloc(nullptr, size, MEM_COMMIT, PAGE_READWRITE);
  memcpy(memory, codes, size);
  DWORD nouse;
  VirtualProtect(memory, size, PAGE_EXECUTE_rEAD, &nouse);
  return memory;
}