// any 타입의 값이 부호가 없는 정수 size_t인지 확인
auto isSize(any value) -> bool {
  return value.type() == typeid(size_t);
}

// any 타입의 값을 부호가 없는 정수 size_t로 변환해 반환
auto toSize(any value) -> size_t {
  return any_cast<size_t>(value);
}

// 매개변수로 받은 any 타입의 값이 불리언 타입이고 값이 참인 경우 참 반환
auto isTrue(any value) -> bool {
  return isBoolean(value) && toBoolean(value);
}

// 참조 여부 멤버로 갖는 구조체
struct Object {
  bool isMarked = false;
  virtual ~Object() {}
};

struct Array: Object {
  vector<any> values;
};