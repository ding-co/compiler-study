auto main() -> void {
  // result = myFunc();
	__asm {
    call myFunc()
    mov result, eax
	}
}