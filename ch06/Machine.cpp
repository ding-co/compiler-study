// 스택 프레임 구조체
struct StackFrame {
  // 변수 공간
  vector<any> variables;
  // 피연산자 공간
  vector<any> operandStack;
  // 명령어 포인터
  size_t instructionPointer = 0;
};

// 콜 스택
static vector<StackFrame> callStack;

// 코드 리스트의 첫 번째 코드부터 실행하기 위해
// startup() 함수의 스택 프레임을 생성해 콜 스택에 넣음
auto execute(tuple<vector<Code>, map<string, size_t>> objectCode) -> void {
  callStack.emplace_back();
  auto codeList = get<0>(objectCode);
  auto functionTable = get<1>(objectCode);
  while (true) {
    // 실행할 코드 가져오기 (명령어 포인터; 콜 스택의 최상단에 있는 스택 프레임의 명령어 포인터)
    auto code = codeList[callStack.back().instructionPointer];
    // 코드의 명령에 따라 분기해 적절한 동작 수행
    switch (code.instruction) {
      case Instruction::GetGlobal: {
        // 함수명; 코드의 인자를 문자열로 변환
        auto name = toString(code.operand);
        // 함수명으로 함수 테이블을 검색해 함수의 주소를 피연산자 스택에 넣음
        if (global.count(name))
          pushOperand(global[name]);
        else if (global.count(name))
          pushOperand(global[name]);
        else if (builtinFunctionTable.count(name))
          pushOperand(builtinFunctionTable[name]);
        // 등록된 함수 없으면 널을 피연산자 스택에 넣음
        else
          pushOperand(nullptr);
        break;
      }
      case Instruction::SetGlobal: {
        auto name = toString(code.operand);
        global[name] = peekOperand();
        break;
      }
      case Instruction::Call: {
        // 호출할 함수의 주소는 피연산자 스택에 있으므로 피연산자 스택에서 값을 하나 꺼냄
        auto operand = popOperand();
        // 피연산자 스택에서 꺼낸 값이 주소라면 스택 프레임 생성
        // 생성한 스택 프레임의 명령어 포인터를 호출할 함수의 주소로 설정
        if (isSize(operand)) {
          StackFrame stackFrame;
          stackFrame.instructionPointer = toSize(operand);
          // 함수 호출 인자를 매개변수로 만드는 과정
          for (auto i = 0; i < toSize(code.operand); i++) {
            stackFrame.variables.push_back(callStack.back().operandStack.back());
            callStack.back().operandStack.pop_back();
          }
          // 초기화된 스택 프레임을 콜 스택에 넣고 명령어 포인터 증가되지 않도록 하기
          callStack.push_back(stackFrame);
          continue;
        }
        else
          pushOperand(nullptr);
          if (isBuiltinFunction(operand)) {
            vector<any> arguments;
            for (auto i = 0; i < toSize(code.operand); i++)
              arguments.push_back(popOperand());
          }
          pushOperand(toBuiltin(operand)(arguments));
          break;
      }
      // 지역변수들의 값을 저장할 공간 확보
      // Alloca 명령의 인자값만큼 변수 배열 크기 늘려 지역변수들의 값을 저장할 공간 추가 확보
      case Instruction::Alloca: {
        auto extraSize = toSize(code.operand);
        auto currentSize = callStack.back().variables.size();
        callStack.back().variables.resize(currentSize + extraSize);
        break;
      }
      // 문자열을 인자로 가짐
      case Instruction::PushString: {
        pushOperand(code.operand);
        break;
      }
      // 콘솔에 출력할 값의 개수 인자로 가짐
      // 인자의 크기만큼 피연산자 스택에서 값을 꺼내 콘솔에 출력
      case Instruction::Print: {
        for (auto i = 0; i < toSize(code.operand); i++) {
          auto value = popOperand();
          cout << value;
        }
        break;
      }
      // 함수의 값 반환하고 함수 종료
      case Instruction::Return: {
        any result = nullptr;
        if (callStack.back().operandStack.empty() == false)
          result = callStack.back().operandStack.back();
        // 현재 스택 프레임을 콜 스택에서 제거하여 함수 종료
        callStack.pop_back();
        // 임시로 보관했던 반환값을 현재 스택 프레임의 피연산자 스택에 넣음
        callStack.back().operandStack.push_back(result);
        collectGarbage();
        break;
      }
      // 두 개의 피연산자 가지는 덧셈 명령
      case Instruction::Add: {
        auto rValue = popOperand();
        auto lValue = popOperand();
        if (isNumber(lValue) && isNumber(rValue))
          pushOperand(toNumber(lValue) + toNumber(rValue));
        else if (isString(lValue) && isString(rValue))
          pushOperand(toString(lValue) + toString(rValue));
        else
          pushOperand(0.0);
        break;
      }
      // 왼쪽 식의 값이 참인지 거짓인지에 따라 동작 달라짐
      case Instruction::LogicalOr: {
        auto value = popOperand();
        if (isTrue(value)) {
          pushOperand(value);
          callStack.back().instructionPointer = toSize(code.operand);
          continue;
        }
        break;
      }
      // 지역변수의 값 참조
      // 현재 스택 프레임의 변수 배열에서 오프셋 위치에 저장되어 있는 값을 피연산자 스택에 넣음
      case Instruction::GetLocal: {
        auto index = toSize(code.operand);
        pushOperand(callStack.back().variables[index]);
        break;
      }
      // 지역변수의 값 변경
      // 현재 스택 프레임의 변수 배열의 오프셋 위치에 값 대입
      case Instruction::SetLocal: {
        auto index = toSize(code.operand);
        callStack.back().variables[index] = peekOperand();
        break;
      }
      // 조건 없이 점프하는 명령
      case Instruction::Jump: {
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      // 조건식의 결과에 따라 점프하는 명령
      case Instruction::ConditionJump: {
        auto condition = popOperand();
        if (isTrue(condition))
          break;
        callStack.back().instructionPointer = toSize(code.operand);
        continue;
      }
      // 배열 생성해 피연산자 스택에 넣는 명령
      case Instruction::PushArray: {
        auto result = new Array();
        auto size = toSize(code.operand);
        for (auto i = size; i > 0; i--)
          result -> values.push_back(popOperand());
        pushOperand(result);
        objects.push_back(result);
        break;
      }
      // 배열이나 맵의 원소값 참조
      case Instruction::GetElement: {
        auto index = popOperand();
        auto object = popOperand();
        if (isArray(sub) && isNumber(index))
          pushOperand(getValueOfArray(sub, index));
        else if (isMap(sub) && isString(index))
          pushOperand(getValueOfMap(sub, index));
        else
          pushOperand(nullptr);
        break;
      }
      // 배열이나 맵의 원소값 변경
      case Instruction::SetElement: {
        auto index = popOperand();
        auto sub = popOperand();
        if (isArray(sub) && isNumber(index))
          setValueOfArray(sub, index, peekOperand());
        else if (isMap(sub) && isString(index))
          setValueOfMap(sub, index, peekOperand());
        break;
      }
      // 명렁어 Exit; startup() 함수의 스택 프레임을 콜 스택에서 제거하고 실행 종료
      case Instruction::Exit:
        callStack.pop_back();
        return;
    }
    // 명령어 포인터 1 증가
    callStack.back().instructionPointer += 1;
  }
}

// 현재 스택 프레임의 피연산자 스택에서 값을 꺼내 반환
auto popOperand() -> any {
  auto value = callStack.back().operandStack.back();
  callStack.back().operandStack.pop_back();
  return value;
}

// 현재 스택 프레임의 피연산자 스택에 값을 넣음
auto pushOperand(any value) -> void {
  callStack.back().operandStack.push_back(value);
}

// 스택에서 값 제거 X
auto peekOperand() -> any {
  return callStack.back().operandStack.back();
}

// 전역 변수 선언
static map<string, any> global

// 내장 함수 정의
extern map<string, function<any(vector<any>)>> builtinFunctionTable;

// 객체에 참조 표시하는 함수 정의
auto markObject(any value) -> void {
  if (isArray(value)) {
    if (toArray(value) -> isMarked) return;
    toArray(value) -> isMarked = true;
  }
  for (auto& value: toArray(value) -> values)
    markObject(value);
}

// 변수 순회하는 함수 정의
auto collectGarbage() -> void {
  for (auto& stackFrame: callStack) {
    for (auto& value: stackFrame.variables)
      markObject(value);
  }
  for (auto& value: stackFrame.operandStack)
    markObject(value);
  for (auto& [key, value]: global)
    markObject(value);
  seepObject();
}

// 참조 가능 여부 관계없이 할당한 모든 맵과 배열 담는 리스트 선언
static list<Object*> objects;

// 스윕 함수
auto sweepObject() -> void {
  objects.remove_if([](Object* object) {
    if (object -> isMarked) {
      object -> isMarked = false;
      return false;
    }
    delete object;
    return true;
  });
}