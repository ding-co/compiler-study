static vector<Token>::iterator current;

auto skipCurrent(Kind kind) -> void {
  if (current -> kind != kind) {
    cout << toString(kind) + " 토큰이 필요합니다.";
    exit(1);
  }
  current++;
}

auto parseOr() -> Expression* {
  // or 연산자 보다 and 연산자가 우선순위 한 단계 더 높음
  auto result = parseAnd();
  while (skipCurrentIf(Kind::LogicalOr)) {
    // Or 연산자는 왼쪽 결합 연산자임 (재귀 호출 안함)
    auto temp = new Or();
    temp -> lhs = result;
    temp -> rhs = parseAnd();
    result = temp;
  }
  return result;
}

auto parseAssignment() -> Expression* {
  auto result = parseOr();
  if (current -> kind != Kind::Assignment)
    return result;
  skipCurrent(Kind::Assignment);
  if (auto getVariable = dynamic_cast<GetVariable*>(result)) {
    auto result = new SetVariable();
    result -> name = getVariable -> name;
    // 재귀 호출 이유; 대입 연산자는 우측 결합 연산자임
    result -> value = parseAssignment();
    return result;
  }
  if (auto getElement = dynamic_cast<GetElement*>(result)) {
    auto result = new SetElement();
    result -> sub = getElement -> sub;
    result -> index = getElement -> index;
    result -> value = parseAssignment();
    return result;
  }
  cout << "잘못된 대입 연산 식입니다.";
  exit(1);
}

auto parseVariable() -> Variable* {
  auto result = new Variable();
  skipCurrent(Kind::Variable);
  result -> name = current -> string;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::Assignment);
  result -> expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}

// 우선순위 가장 낮은 대입 연산자부터 분석 시작
auto parseExpression() -> Expression* {
  return parseAssignment();
}

auto parseExpressionStatement() -> ExpressionStatement* {
  auto result = new ExpressionStatement();
  result -> expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}

auto parseBlock() -> vector<Statement*> {
  vector<Statement*> result;
  while (current -> kind != Kind::RightBrace) {
    switch (current -> kind) {
      case Kind::Variable:
        result.push_back(parseVariable());
        break;
      case Kind::EndOfToken:
        cout << *current << " 잘못된 구문입니다.";
        exit(1);
      default:
        result.push_back(parseExpressionStatement());
        break;
    }
  }
  return result;
}

auto parseFunction() -> Function* {
  auto result = new Function();
  // Function 토큰은 함수 정의 시작을 나타내는 토큰일 뿐이므로 무시
  skipCurrent(Kind::Function);
  result -> name = current -> string;
  skipCurrent(Kind::Identifier);
  skipCurrent(Kind::LeftParen);
  if (current -> kind != Kind::RightParen) {
    do {
      result -> parameters.push_back(current -> string);
      skipCurrent(Kind::Identifier);
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  skipCurrent(Kind::LeftBrace);
  result -> block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}

// 토큰 리스트를 처음부터 끝까지 토큰 단위로 순회하며 구문 트리 구성
auto parse(vector<Token> tokens) -> Program* {
  auto result = new Program();
  // 전역 변수 current
  current = tokens.begin();
  // 순회 끝나면 구문 트리의 루트 노드 반환
  while (current -> kind != Kind::EndOfToken) {
    switch (current -> kind) {
      case Kind::Function: {
        result -> functions.push_back(parseFunction());
        break;
      }
      default:
        cout << *current << " 잘못된 구문입니다.";
        exit(1);
    }
  }
  return result;
}