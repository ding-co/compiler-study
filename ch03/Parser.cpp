static vector<Token>::iterator current;

// 토큰 리스트를 처음부터 끝까지 토큰 단위로 순회하며 구문 트리 구성
auto parse(vector<Token> tokens) -> Program* {
  auto result = new Program();
  // 전역 변수 current가 매개변수로 받은 토큰 리스트의 첫 번째 토큰 가리킴
  current = tokens.begin();
  // 순회 끝나면 구문 트리의 루트 노드 반환 (result)
  while (current -> kind != Kind::EndOfToken) {
    switch (current -> kind) {
      case Kind::Function: {
        // 함수 정의 분석하고 반환받은 함수 노드를 프로그램 노드에 추가
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

auto parseFunction() -> Function* {
  auto result = new Function();
  // Function 토큰은 함수 정의 시작을 나타내는 토큰일 뿐이므로 무시
  // 함수 노드 생성 후 현재 토큰 버림
  skipCurrent(Kind::Function);
  // 현재 토큰의 문자열을 앞서 생성한 함수 노드에 설정 후 현재 토큰 건너뜀
  result -> name = current -> string;
  skipCurrent(Kind::Identifier);
  // 매개변수 목록에서 괄호나 콤마와 같은 구분자는 건너뛰면서
  // 매개변수들의 이름을 함수 노드의 매개변수 리스트에 추가
  skipCurrent(Kind::LeftParen);
  if (current -> kind != Kind::RightParen) {
    do {
      result -> parameters.push_back(current -> string);
      skipCurrent(Kind::Identifier);
    } while (skipCurrentIf(Kind::Comma));
  }
  skipCurrent(Kind::RightParen);
  // 함수 본문의 앞/뒤 괄호는 건너뜀
  skipCurrent(Kind::LeftBrace);
  // 함수 본문 리스트를 함수 노드에 저장
  result -> block = parseBlock();
  skipCurrent(Kind::RightBrace);
  return result;
}

auto skipCurrent(Kind kind) -> void {
  // 매개변수로 받은 토큰이 현재 토큰과 같지 않으면 프로그램 종료
  if (current -> kind != kind) {
    cout << toString(kind) + " 토큰이 필요합니다.";
    exit(1);
  }
  // 같으면 현재 토큰 건너뜀 (무시)
  current++;
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
      // 현재 토큰이 특정 키워드가 아닐 때를 식의 시작으로 간주 (별도 키워드 없음)
      default:
        result.push_back(parseExpressionStatement());
        break;
    }
  }
  return result;
}

// 변수의 선언 분석하는 함수
auto parseVariable() -> Variable* {
  // var 키워드 무시 (변수 선언 노드 생성하고 현재 토큰 건너뜀)
  auto result = new Variable();
  skipCurrent(Kind::Variable);
  // var 키워드 다음에는 변수의 이름인 식별자 토큰
  // 현재 토큰의 문자열을 변수 선언 노드의 이름으로 설정하고 현재 토큰 건너뜀
  result -> name = current -> string;
  skipCurrent(Kind::Identifier);
  // 변수 초기화 강제
  skipCurrent(Kind::Assignment);
  // 변수 선언 노드의 초기화 식으로 설정
  result -> expression = parseExpression();
  // 단일문 => 끝 표시하는 문자 필요 (세미콜론 또는 개행)
  skipCurrent(Kind::Semicolon);
  return result;
}

// 식을 분석하고 반환받은 식 노드를 식의 문 노드로 감싸서 반환
auto parseExpressionStatement() -> ExpressionStatement* {
  auto result = new ExpressionStatement();
  result -> expression = parseExpression();
  skipCurrent(Kind::Semicolon);
  return result;
}

// 식 분석 함수; 우선순위 가장 낮은 대입 연산자부터 분석 시작
auto parseExpression() -> Expression* {
  return parseAssignment();
}

auto parseAssignment() -> Expression* {
  // or 연산자가 대입 연산자보다 우선순위 한 단계 더 높음
  auto result = parseOr();
  // 왼쪽 식 분석 끝나면 대입 연산자보다 우선순위 높은 연산자들의 분석은 모두 끝난 상태
  // 현재 토큰이 대입 연산자 아니면 식이 끝난 것으로 간주
  if (current -> kind != Kind::Assignment)
    return result;
  skipCurrent(Kind::Assignment);
  // 대입 연산자 왼쪽 식으로 참조 가능한 변수가 옴
  if (auto getVariable = dynamic_cast<GetVariable*>(result)) {
    auto result = new SetVariable();
    result -> name = getVariable -> name;
    // 재귀 호출 이유; 대입 연산자는 우측 결합 연산자임
    result -> value = parseAssignment();
    return result;
  }
  // 대입 연산자 왼쪽 식으로 배열이나 맵의 원소 참조도 있음
  if (auto getElement = dynamic_cast<GetElement*>(result)) {
    auto result = new SetElement();
    result -> sub = getElement -> sub; // 피연산자 식
    result -> index = getElement -> index; // 인덱스 식
    // 대입 연산자가 구문 트리에서 루트 노드임
    result -> value = parseAssignment();
    return result;
  }
  cout << "잘못된 대입 연산 식입니다.";
  exit(1);
}

auto parseOr() -> Expression* {
  // or 연산자 보다 and 연산자가 우선순위 한 단계 더 높음 (왼쪽 피연산자 식 분석 위함)
  auto result = parseAnd();
  // 연속된 or 연산자들 분석위함
  while (skipCurrentIf(Kind::LogicalOr)) {
    // or 연산자는 왼쪽 결합 연산자임 (재귀 호출 안함)
    // 왼쪽 식 먼저 평가 후 오른쪽 식 평가
    auto temp = new Or();
    temp -> lhs = result;
    temp -> rhs = parseAnd();
    result = temp;
  }
  return result;
}