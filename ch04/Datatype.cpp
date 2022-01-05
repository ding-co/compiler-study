// 매개변수로 받은 any 타입에 저장된 값의 타입이 문자열인지 확인
auto isString(any value) -> bool {
  return value.type() == typeid(string);
}

// 매개변수로 받은 any 타입의 값을 문자열로 캐스팅해 반환
auto toString(any value) -> string {
  return any_cast<string>(value);
}

// any 타입에 저장된 값의 데이터 타입 확인하고 캐스팅하여 콘솔에 출력
auto operator<<(ostream& stream, any& value) -> ostream& {
  if (isString(value)) stream << toString(value);
  // 배열 내용도 콘솔에 출력되도록 함
  else if (isArray(value)) {
    stream << "[";
    auto temp - toArray(value);
    for (auto& element: *temp)
      steram << element << " ";
    stream << "]";
  }
  return stream;
}

// 매개변수로 받은 any 타입의 값의 데이터 타입이 내장 함수인지 확인
auto isBuiltinFunction(any value) -> bool {
  return value.type() == typeid(function<any(vector<any>)>);
}

// 매개변수로 받은 배열에서 매개변수로 받은 인덱스에 해당하는 값 반환
auto getValueOfArray(any object, any index) -> any {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object) -> size())
    return toArray(object) -> at(i);
  return nullptr;
}

// 대입 연산자는 대입한 값 결과값으로 남김
// 배열의 원소에 저장한 값 반환
auto setValueOfArray(any object, any index, any value) -> any {
  auto i = static_cast<size_t>(toNumber(index));
  if (i >= 0 && i < toArray(object) -> size())
    toArray(object) -> at(i) = value;
  return value;
}