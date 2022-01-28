auto main() -> void {
  unsigned char codes[] = {
    0x8B, 0x44, 0x24, 0x04, // mov eax, [esp + 4]
    0x03, 0x44, 0x24, 0x08, // add eax, [esp + 8]
    0xC3                    // ret
  };
}