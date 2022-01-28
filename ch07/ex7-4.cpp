auto main() -> void {
	__asm {
    mov eax, 1
    add eax, 7
    sub eax, 2
    imul eax, 3
    mov ebx, 4
    mov edx, 0
	}
}