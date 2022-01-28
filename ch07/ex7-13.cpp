auto main() -> void {
  // result = myFunc(3, 4);
	__asm {
    push 3
    push 4
    call myFunc
    add esp, 8
    mov result, eax
	}
}