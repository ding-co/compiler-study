// main 함수의 주소를 가져오는 GetGlobal 코드 생성 (함수 호출 위해 주소 필요)
auto generate(Program* program) -> tuple<vector<Code>, map<string, size_t>> {
  writeCode(Instruction::GetGlobal, string("main"));
  // 함수 호출하는 명령 (매개변수 개수는 0)
  writeCode(Instruction::Call, static_cast<size_t>(0));
  // main 함수 종료 시 프로그램 종료하는 Exit 명령
  // 코드 리스트와 함수 테이블 반환
  writeCode(Instruction::Exit);
  // 함수 테이블에 함수 등록, 함수 내용을 목적 코드로 생성하기 위해
  // 함수 노드 리스트 순회 코드 추가
  for (auto& node: program -> functions)
    node -> generate();
  return {codeList, functionTable};
}

// 명령어와 인자를 코드 리스트에 추가하고, 추가된 코드 리스트의 인덱스를 주소로 반환
auto writeCode(Instruction instruction) -> size_t {
  codeList.push_back({instruction});
  return codeList.size() - 1;
}
auto writeCode(Instruction instruction, any operand) -> size_t {
  codeList.push_back({instruction, operand});
  return codeList.size() - 1;
}

// 전역 변수 코드 리스트 선언
static vector<Code> codeList;

// 현재 주소를 함수의 주소로 함수 테이블에 등록
auto Function::generate() -> void {
  functionTable[name] = codeList.size();
  // 본문의 문 노드 리스트 순회하여 함수 내용 -> 목적 코드로 생성
  // 이후 함수 종료
  for (auto& node: block)
    node -> generate();
  writeCode(Instruction::Return);
  // 함수 실행 위해 필요한 메모리 공간 할당하도록 Alloca 명령 생성
  auto temp = writeCode(Instruction::Alloca);
  for (auto& node: block)
    node -> generate();
  // 본문 노드 순회 끝나면 전역 변수 localSize의 값으로 Alloca 명령 인자 패치
  patchOperand(temp, localSize);
  // 함수의 블록 초기화
  initBlock();
  for (auto& node: block)
    node -> generate();
  // 순회 끝나면 함수 블록 제거
  popBlock();
  // 인자값 참조 위해 매개변수 이름 등록하는 코드
  initBlock();
  for (auto& name: parameters)
    setLocal(name);
  }
}

// 함수 이름과 주소를 키와 값으로 보관하는 함수 테이블 선언
static map<string, size_t> functionTable;

// 인자 식 노드 순회하여 식의 목적 코드 생성
auto Print::generate() -> void {
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1] -> generate();
  // 인자 식 노드 개수를 인자로 print 명령 행성
  // print 명령; 인자 크기만큼 피연산자 스택에서 값을 꺼내 콘솔에 출력
  writeCode(Instruction::Print, arguments.size());
  // printLine 이었다면 콘솔에 개행 출력하는 명령 생성
  if (lineFeed)
    writeCode(Instruction::PrintLine);
}

// 문자열 리터럴 노드
// 멤버인 문자열 값을 인자로 피연산자 스택에 문자열 넣는 PushString 명령 생성
auto StringLiteral::generate() -> void {
  writeCode(Instruction::PushString, value);
}

// 산술 연산자 노드
// 왼쪽 식 노드와 오른쪽 식 노드를 차례로 순회하고 산술 연산 명령 생성
auto Arithmetic::generate() -> void {
  map<Kind, Instruction> instructions = {
    {Kind::Add,       Instruction::Add},
    {Kind::Subtract,  Instruction::Subtract},
    {Kind::Multiply,  Instruction::Multiply},
    {Kind::Divide,    Instruction::Divide},
    {Kind::Modulo,    Instruction::Modulo},
  };
  lhs -> generate();
  rhs -> generate();
  writeCode(instructions[kind]);
}

// 식을 표현하는 문 노드의 generate() 함수에서 식 노드 순회해 목적 코드 생성
auto ExpressionStatement::generate() -> void {
  expression -> generate();
  // 피연산자 스택에서 남은 값을 꺼내 버림
  writeCode(Instruction::PopOperand);
}

// 논리 Or 연산자
// 단락 평가 되도록 코드 생성
auto Or::generate() -> void {
  // 왼쪽 식 노드 순회해 코드 생성
  lhs -> generate();
  // 오른쪽 식의 끝을 알 수 없어서 LogicalOr 주소를 임시 보관
  auto or = writeCode(Instruction::LogicalOr);
  // 오른쪽 식 노드 순회해 코드 생성 (현재 주소가 오른쪽 식의 끝 주소)
  rhs -> generate();
  // LogicalOr 명령의 인자를 현재 주소로 패치
  patchAddress(or);
}

// 코드의 인자를 코드 리스트의 크기로 설정
// 코드 리스트 크기에 해당하는 주소는 아직 생성되지 않은 코드의 주소임
auto patchAddress(size_t codeIndex) -> void {
  codeList[codeIndex].operand = codeList.size();
}

// 지역 변수의 오프셋(상대 주소)을 관리하기 위한 전역 변수 선언
// 맵; 변수의 이름과 오프셋을 키와 값으로 가짐
// 리스트; 변수의 유효 범위 관리
static list<map<string, size_t>> symbolStack;

// 블록 단위로 가장 큰 오프셋 값을 관리하는 전역 변수
static vector<size_t> offsetStack;

// 오프셋 스택; 블록이 끝나면 값을 버리므로 이 값을 보관할 전역 변수
static size_t localSize;

// 코드의 인자를 매개변수로 받은 값으로 설정
auto patchOperand(size_t codeIndex, size_t operand) -> void {
  codeList[codeIndex].operand = operand;
}

auto initBlock() -> void {
  // 함수의 크기를 관리하는 전역 변수를  0으로 초기화
  localSize = 0;
  // 오프셋 스택에 기본값인 0 추가해 블록 생성
  offsetStack.push_back(0);
  // 심볼 스택에 빈 블록 생성
  symbolStack.emplace_front();
}

// 오프셋 스택과 심볼 스택에 생성된 블록 제거
auto popBlock() -> void {
  offsetStack.pop_back();
  symbolStack.pop_front();
}

// 변수 선언 표현하는 노드에서는 현재 블록에 변수의 이름 등록
auto Variable::generate() -> void {
  setLocal(name);
  // 초기화 식 노드 순회하고 변수의 값 초기화하도록 setLocal 명령 생성
  expression -> generate();
  writeCode(Instruction::SetLocal, getLocal(name));
  // 변수의 값 초기화 위해 생성한 SetLocal 명령은 대입한 값을 연산 결과로 남김
  // 이 값이 피연산자 스택에 남아 있지 않도록 PopOperand 명령 생성
  writeCode(Instruction::PopOperand);
}

auto setLocal(string name) -> void {
  symbolStack.front()[name] = offsetStack.back();
  offsetStack.back() += 1;
  localSize = max(localSize, offsetStack.back());
}

// 변수명을 매개변수로 받아 오프셋 반환
auto getLocal(string name) -> size_t {
  for (auto& symbolTable: symbolStack) {
    if (symbolTable.count(name))
      return symbolTable[name];
  }
  return SIZE_MAX;
}

// 선언이 없는 변수의 참조를 전역 변수의 참조로 간주
auto GetVariable::generate() -> void {
  if (getLocal(name) == SIZE_MAX)
    // 변수의 이름을 인자로 전역 변수 참조하는 GetGlobal 명령 생성
    writeCode(Instruction::GetGlobal, name);
  else
    // 변수의 오프셋을 인자로 지역 변수를 참조하는 GetLocal 명령 생성
    writeCode(Instruction::GetLocal, getLocal(name));
}


auto SetVariable::generate() -> void {
  value -> generate();
  if (getLocal(name) == SIZE_MAX)
    // 변수의 이름을 인자로 전역 변수의 값을 수정하는 SetGlobal 명령 생성
    writeCode(Instruction::SetGlobal, name);
  else
    // 변수의 오프셋을 인자로 지역 변수의 값을 수정하는 SetLocal 명령 생성
    writeCode(Instruction::SetLocal, getLocal(name));
}

// 블록 생성 후 제어 변수 초기화하는 목적 코드 생성하여
// 제어 변수의 유효 범위가 for문에 종속되도록 함
auto For::generate() -> void {
  pushBlock();
  variable -> generate();
  // 본문 실행할 때마다 조건식을 실행하기 위해 조건식의 주소를 알아야 함
  // 현재 주소를 임시로 보관 후 조건식 노드를 순회해 목적 코드 생성
  auto jumpAddress = codeList.size();
  condition -> generate();
  // 식의 결과가 참이 아닌 경우에 점프하는 명령 생성
  auto conditionJump = writeCode(Instruction::ConditionJump);
  // 본문 노드들을 순회해 조건식의 결과가 참인 경우 실행할 목적 코드 생성
  for (auto& node: block)
    node -> generate();
  // 본문 실행 후 증감식 실행 위해 증감식 노드를 순회해 목적 코드 생성
  // 식의 결과값이 버려지도록 PopOperand 명령 생성
  expression -> generate();
  writeCode(Instruction::PopOperand);
  // 증감식 실행 후 반복문이 조건식부터 다시 실행되도록 조건식의 주소로 점프
  writeCode(Instruction::Jump, jumpAddress);
  // 조건식의 결과가 참이 아닌 경우에 현재 주소로 점프하도록 명령 패치
  // 생성했던 for문 블록 제거
  patchAddress(conditionJump);
  popBlock();
  // Continue 스택의 블록은 for문 노드의 시작 부분에서 생성
  continueStack.emplace_back();
  pushBlock()
  // 증감식 노드 순회해 목적 코드 생성 전에 현재 주소를 임시로 보관
  auto continueAddress = codeList.size();
  expresssion -> generate();
  // Continue 스택의 현재 블록에 있는 Jump 코드들을 증감식의 주소로 점프하도록 패치
  // 시작 부분에서 생성한 블록 제거
  popBlock();
  for (auto& jump: continueStack.back())
    patchOperand(jump, continueAddress);
  continueStack.pop_back();
}

// 심볼 스택에 빈 블록 생성
// 오프셋 스택에 새 블록 생성 (시작값: 현재 블록의 오프셋 값)
auto pushBlock() -> void {
  symbolStack.emplace_front();
  offsetStack.push_back(offsetStack.back());
}

// 생성한 점프 명령 코드들을 보관할 리스트 생성, 조건식의 개수만큼 반복함으로써 시작
auto If::generate() -> void {
  vector<size_t> jumpList;
  for (auto i = 0; i < conditions.size(); i++) {
    conditions[i] -> generate();
    // 조건식의 결과가 참이 아니면 다음 조건식의 주소로 점프하도록 명령 생성
    auto conditionJump = writeCode(Instruction::ConditionJump);
    // 조건식의 결과가 참이면 for문과 동일하게 블록 생성
    pushBlock();
    for (auto& node: blocks[i])
      // 본문 노드 순회해서 목적 코드 생성
      node -> generate();
    // 생성했던 블록 제거
    popBlock();
    // 본문 실행 후 elif절이나 else절 실행하지 않도록 Jump 명령 생성
    // 생성된 코드의 주소를 점프 코드 리스트에 보관
    jumpList.push_back(writeCode(Instruction::Jump));
    // 조건식의 결과가 참이 아니면 현재 주소로 점프, 생성했던 명령 패치
    patchAddress(conditionJump);
    // else절이 있다면 else절 본문의 문 노드 순회해 목적 코드 생성
    if (elseBlock.empty() == false) {
      pushBlock();
      for (auto& node: elseBlock)
        node -> generate();
      popBlock();
    }
  }
  // 하나의 본문 실행 후 현재 주소로 점프하도록 점프 코드 리스트 순회해 Jump 명령 패치
  for (auto& jump: jumpList)
    patchAddress(jump);
}

// 가장 가까운 for문의 증감식의 주소로 점프하도록 구현
// Jump 코드 주소 담을 리스트를 전역 변수로 선언
static vector<vector<size_t>> continueStack;

// 생성한 코드의 주소를 Continue 스택의 현재 블록에 추가
auto Continue::generate() -> void {
  if (continueStack.empty()) return;
  auto jumpCode = writeCode(Instruction::Jump);
  continueStack.back().push_back(jumpCode);
}

// 인자값들의 순서 유지 위해 역순으로 인자 식 노드들 순회
auto Call:generate() -> void {
  for (auto i = arguments.size(); i > 0; i--)
    arguments[i - 1] -> generate();
  // 호출할 함수 주소 얻기 위해 피연산자식 노드 순회해 목적 코드 생성
  sub -> generate();
  // 인자 식 노드 개수를 인자로 함수를 호출하는 Call 명령 생성
  writeCode(Instruction::Call, arguments.size());
}

// 식 노드 순회해 반환값을 피연산자 스택에 넣는 코드 생성, Return 명령 생성
auto Return::generate() -> void {
  expression -> generate();
  writeCode(Instruction::Return);

// 원소 식 리스트 순회해 피연산자 스택에 원소값 쌓이도록 함 (역순으로 순회)
auto ArrayLiteral::generate() -> void {
  for (auto i = values.size(); i > 0; i--)
    values[i - 1] -> generate();
  
  // 인자 식 순회 후 피연산자 스택에 쌓여 있는 원소값들을 모두 꺼내 배열 생성하는 명령 생성
  writeCode(Instruction::PushArray, values.size());
}

// 배열이나 맵의 원소 참조하는 목적 코드 생성
auto GetElement::generate() -> void {
  sub -> generate();
  index -> generate();
  // 피연산자 스택에 쌓여 있는 두 값 꺼내 원소 참조, 참조한 원소값 다시 넣는 명령 생성
  writeCode(Instruction::GetElement);
}

// 대입 식과 피연산자 식, 인덱스 식을 차례로 순회해 목적 코드 생성
auto SetElement::generate() -> void {
  value -> generate();
  sub -> generate();
  index -> generate();
  // 피연산자 스택에 쌓여 있는 세 값 꺼내 원소 값 수정 후, 수정한 값 다시 스택에 넣는 명령 생성
  writeCode(Instruction::SetElement);
}