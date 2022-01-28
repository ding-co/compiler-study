// 기계어 코드가 실행 권한이 없는 메모리 영역에 저장되어 있어서 실행 안됨
auto myFunc = static_cast<int(*)(int, int)>((void*)codes);
std::cout << myFunc(3, 4);