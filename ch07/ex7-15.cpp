__declspec(naked) auto myFunc(int a, int b) -> int {
  __asm {
    push ebp
    mov ebp, esp

    sub esp, 8
    mov dword ptr[ebp - 4], 3
    mov dword ptr[ebp - 8], 4

    mov eax, [ebp + 8]
    add eax, [ebp + 12]
    add eax, [ebp - 4]
    add eax, [ebp - 8]

    mov esp,ebp
    pop ebp
    ret
  }
}