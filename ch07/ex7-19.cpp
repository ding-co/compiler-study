// 기계어 코드를 실행 권한이 있는 메모리 영역으로 복사한 후 실행하면 배열에 저장되어 있던 기계어 코드 실행 가능
auto executableMemory = copy2ExecutableMemory(codes, sizeof codes);
auto myFunc = static_cast(*)(int, int)>(executableMemory);
std::cout << myFunc(5, 6);