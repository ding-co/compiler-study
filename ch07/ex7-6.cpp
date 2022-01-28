auto main() -> void {
	__asm {
    mov eax, 0
    START:
      add eax, 1
      cmp eax, 3
      jne START
	}
}