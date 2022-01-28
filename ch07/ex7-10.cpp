__declspec(naked) auto myFunc() -> int {
	__asm {
    mov eax, 7
	}
}