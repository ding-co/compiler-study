// 전역 변수 선언 (functionTable)
// 함수의 이름, 함수 노드를 키와 값으로 가짐
static map<string, Function*> functionTable;

// 프로그램 노드에 포함된 함수 노드들을 functionTable 변수에 등록
auto interpret(Program* program) -> void {
  for (auto& node: program -> functions)
    functionTable[node -> name] = node;

  // 엔트리 포인트는 main() 함수
  // functionTable에서 키가 main으로 등록된 함수 노드 찾아 interpret() 함수 호출
  if (functionTable["main"] == nullptr)
    return;

  try {
    // main() 함수 호출 전 지역 변수 관리 위한 local 전역 변수에 함수 공간 추가
    // 현재 실행 중인 함수의 블록은 항상 바깥쪽 리스트의 마지막이고,
    // 현재 실행 중인 문 블록은 항상 안쪽 리스트의 첫번째임
    local.emplace_back().emplace_front();
    functionTable["main"] -> interpret();
  } catch (ReturnException e) {
    // 메인 함수 종료되면 생성했던 지역 변수 공간 제거
    local.pop_back();
  } 
}

// 함수의 실행은 단순히 본문의 노드들 순회하는 것이 전부임
// 함수 노드의 문 리스트 순회하며 interpret() 함수 호출 (함수 실행)
auto Function::interpret() -> void {
  for (auto& node: block)
    node -> interpret();
}

// print문 노드가 가진 식 노드들을 순회하며 interpert() 함수 호출 (print문 실행)
// 식 노드들을 순회하며 interpret() 함수 호출, 식 노드 반환 값 콘솔에 출력
// 식 노드의 interpret() 함수가 반환하는 값의 데이터 타입은 any임
auto Print::interpret() -> void {
  for (auto& node: arguments) {
    auto value = node -> interpret();
    cout << value;
  }

  // printLine 이면 콘솔에 개행 출력
  if (lineFeed) cout << endl;
}

// 문자열 리터럴 노드; 노드 가진 문자열 값 반환
auto StringLiteral::interpret() -> any {
  return value;
}

// 산술 연산자 노드; 연산 위해 양쪽 식 노드 순회해서 두 피연산자의 값 구함
// 멤버: 왼쪽 식 노드, 오른쪽 식 노드
auto Arithmetic::interpret() -> any {
  auto lValue = lhs -> interpret();
  auto rValue = rhs -> interpret();

  // 연산자 종류와 두 피연산자 데이터 타입에 따라 연산하고 결과값 반환
  if (kind == Kind::Add && isNumber(lValue) && isNumber(rValue))
    return toNumber(lValue) + toNumber(rValue);

  if (kind == Kind::Add && isString(lValue) && isString(rValue))
    return toString(lValue) + toString(rValue);

  if (kind == Kind::Subtract && isNumber(lValue) && isNumber(rValue))
    return toNumber(lValue) - toNumber(rValue);
}

// 논리 연산자; 단락 평가 고려
auto Or::interpret() -> any {
  return isTrue(lhs -> interpret()) ? true : rhs -> interpret();
}

auto And::interpret() -> any {
  return isFalse(lhs -> interpret()) ? false : rhs -> interpret();
}

// 지역 변수의 스코프를 관리하기 위한 전역 변수 선언
// 전역 변수 데이터 타입의 바깥쪽 리스트 => 함수의 블록 표현
// 안쪽의 리스트 => 문의 블록 표현 (ex. for문, if문, ...)
// 맵; 변수의 이름, 값
static list<list<map<string, any>>> local;

// 전역 변수를 관리하기 위한 전역 변수 선언
// 전역 변수는 블록과 관계없이 어디서든 참조 가능
// 맵; 변수의 이름, 값
static map<string, any> global;

// 변수 선언을 표현하는 노드의 interpret() 함수
auto Variable::interpret() -> void {
  // 변수의 이름, 초기화식의 결과값을 키와 값으로 local 전역 변수에 등록
  // 등록하는 위치: 현재 실행 중인 함수 블록에서 현재 실행 중인 문 블록
  local.back().front()[name] = expression -> interpret();
}

// 변수 값의 참조를 표현하는 노드의 interpret() 함수
// 변수의 이름을 키로 local 전역 변수 검색
// 현재 함수 블록에서 중첩되어 실행된 모든 문 블록들을 실행된 순서의 반대로 검색
auto GetVariable::interpret() -> any {
  for (auto& variables: local.back()) {
    if (variables.count(name))
      return variables[name];
  }
  // 변수의 선언을 local 전역 변수에서 찾지 못하면 global 전역 변수에서 찾기 시도
  // global 전역 변수에서도 못찾으면 null 반환
  if (global.count(name))
    return global[name];

  // 식별자의 이름으로 전역 변수 functionTable 검색
  // 같은 이름으로 등록된 함수 노드 찾아 반환
  if (functionTable.count(name))
    return functionTable[name];

  // builtinFunctionTable 전역 변수에 식별자의 이름으로 등록된 내장 함수 있는지 확인
  if (builtinFunctionTable.count(name))
    // 등록된 내장 함수 반환
    return builtinFunctionTable[name];

  return nullptr;
}

// 변수 값의 수정을 표현하는 노드
// 변수의 이름을 키로 local 전역 변수에서 선언을 찾아 저장된 값을 식의 반환값으로 바꾸면 됨
auto setVariable::interpret() -> any {
  for (auto& variables: local.back()) {
    if (variables.count(name))
      return variables[name] = value -> interpret();
  }
  return global[name] = value -> interpret();
}

// for문을 표현하는 노드
// 제어 변수의 유효 범위가 블록안에 종속되어야 함
// => 문 블록 먼저 생성 후 제어 변수 등록
auto For::interpret() -> void {
  local.back().emplace_front();
  variable -> interpret();
  while (true) {
    auto result = condition -> interpret();
    // 조건식은 본문을 실행하기 전마다 평가됨
    // 조건식 노드를 순회해 결과 거짓이면 무한 루프 탈출
    if (isTrue(result) == false)
      break;
    // 증감식은 본문의 실행이 끝났을 때마다 평가됨
    // 본문의 실행 끝나면 증감식 노드 순회
    try {
      for (auto& node: block)
        node -> interpret();
    } catch (ContinueException) {}
    expression -> interpret();
  }
  // 본문이 끝나면 앞서 생성했던 문 블록 제거
  local.back().pop_front();
}

// 조건문 if문
auto If:interpret() -> void {
  // 조건식 개수만큼 루프 돌기 (if절, elif절)
  for (auto i = 0; i < conditions.size(); i++) {
    // 조건식의 결과가 참인 경우에 본문 실행
    auto result = conditions[i] -> interpret();
    if (isTrue(result) == false)
      continue;
    // if문에 포함된 if절, elif절들은 서로 독립된 문 블록 가짐
    // 본문 실행 전에 문 블록 생성하고 본문 실행 끝나면 생성한 문 블록 제거
    // 조건식의 결과가 참이면 본문 하나만 실행하므로
    // 다른 elif 절의 본문이 실행되지 않도록 if문 실행 종료
    local.back().emplace_front();
    for (auto& node: blocks[i])
      node -> interpret();
    local.back().pop_front();
    return;
  }
  // else절 없으면 if문 실행 종료
  if (elseBlock.empty())
    return;
  // else절 있으면 else절 본문 실행 후 if문 종료
  local.back().emplace_front();
  for (auto& node: elseBlock())
    node -> interpret();
  local.back().pop_front();
}

// 예외 처리에 사용할 continue 예외 객체 정의
// 멤버 변수는 없음 (값 전달 X)
struct ContinueException {};

auto Continue::interpret() -> void {
  throw ContinueException;
}

// 함수 호출 노드
auto Call::interpret() -> any {
  auto value = sub -> interpret();
  // 피연산자 식 노드의 반환값이 함수 노드 아니면 null 반환
  if (isFunction(value) == false)
    return nullptr;

  // 인자식 노드 리스트 순회해 인자값을 구하고,
  // 매개변수의 이름과 인자값을 매핑시켜 보관
  map<string, any> parameters;
  for (auto i = 0; i < arguments.size(); i++) {
    auto name = toFunction(value) -> parameters[i];
    parameters[name] = arguments[i] -> interpret();
  }

  // 함수 호출 전에 전역 변수 local에 인자값 리스트로 초기화된 함수 블록 생성
  local.emplace_back().emplace_front(parameters);
  try {
  // 함수 호출
  toFunction(value) -> interpet();
  } catch(ReturnException exception) {
  // 함수 호출 끝나면 생성했던 블록 제거
  local.pop_back();
  // return 예외 발생 시 return 예외 객체에 저장된 반환값 반환
  return exception.result
  }
  
  // 피연산자식 노드의 결과가 내장 함수인지 확인
  auto value = sub -> interpret();
  if (isBuiltinFunction(value)) {
    // 인자식 노드 순회해서 인자값들을 리스트에 담고
    // 인자값 리스트와 함께 내장 함수 호출 후 반환된 값 그대로 반환
    vector<any> values;
    for (auto i = 0; i < arguments.size(); i++)
      values.push_back(arguments[i] -> interpret());
    return toBuiltin(value)(values);
  }
}

// 인자로 받은 any 타입의 값의 데이터 타입이 함수의 호출을 표현하는 노드인지 확인
auto isFunction(any value) -> bool {
  return value.type() == typeid(Function*);
}

// return 예외 객체 정의
// return문은 값이 있으므로 any 타입의 멤버 변수 하나 가짐
auto ReturnException {
  any result;
}

auto Return::interpret() -> void {
// 반환값 식 노드 순회한 결과를 인자로 예외 객체 생성
  throw ReturnException{expression -> interpret()};
}

// 내장 함수 관리하는 전역 변수
// extern 키워드 (다른 cpp 파일에 선언된 전역 변수 참조)
extern map<string, function<any(vector<any>)>> builtinFunctionTable;

// 배열 리터럴 노드
// 인자식 노드 리스트 가지므로 인자식 노드들이 반환한 값들을 초기값으로 하는 배열 생성해서 반환
auto ArrayLiteral::interpret() -> any {
  auto result = make_shared<vector<any>>();
  for (auto& node: values)
    result -> push_back(node -> interpert());
  return result;
}

// 원소값 참조 표현 노드; 피연산자 식 노드, 인덱스 식 노드 (배열에서 해당 인덱스 값 반환)
// 피연산자식 노드 결과값은 배열,
// 인덱스 식 노드 결과값은 인덱스로 사용할 정수
auto GetElement::interpret() -> any {
  auto object = sub -> interpret();
  auto index_ = index -> interpret();
  if (isArray(object) && isNumber(index_))
    return getValueOfArray(object, index_);
  return nullptr;
}

// 원소값 수정 표현 노드; 피연산자 식 노드, 인덱스 식 노드, 대입 식 노드 (값 변경 위함)
// 대입 식 노드 결과값은 배열의 해당 인덱스에 대입할 값
auto setElement::interpret() -> any {
  auto object = sub -> interpret();
  auto index_ = index -> interpret();
  auto value_ = value -> interpret();
  if (isArray(object) && isNumber(index_))
    return setValueOfArray(object, index_, value_);
  return nullptr;
}