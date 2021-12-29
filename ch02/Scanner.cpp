static string::iterator current;

enum class CharType {
  Unknown,                // 사용할 수 없는 문자
  WhiteSpace,             // 공백, 탭, 개행
  NumberLiteral,          // 숫자 리터럴
  StringLiteral,          // 문자열 리터럴
  IdentifierAndKeyword    // 식별자, 키워드
  OperatorAndPunctuator,  // 연산자, 구분자
};

// 현재 문자가 어떤 어휘의 시작 문자인지 판단
auto getCharType(char c) -> CharType {
  // 공백, 탭, 캐리지 리턴, 개행은 토큰 생성 필요 X
  if (' ' == c || '\t' == c || '\r' == c || '\n' == c)
    return CharType::WhiteSpace;
  if ('0' <= c && c <= '9')
    return CharType::NumberLiteral;
  if (c == '\'')
    return CharType::StringLiteral;
  if ('a' <= c && c <= 'z' || 'A' <= c && c <= 'Z')
    return CharType::IdentifierAndKeyword;
  if (33 <= c && c <= 47 && c != '\'' ||
      58 <= c && c <= 64 ||
      91 <= c && c <= 126)
    return CharType::OperatorAndPunctuator;
  return CharType::Unknown;
}

// 현재 문자가 어떤 어휘에 포함되는 문자인지 판단
auto isCharType(char c, CharType type) -> bool {
  switch (type) {
    case CharType::NumberLiteral:
      return '0' <= c && c <= '9';
    case CharType::StringLiteral:
      return 32 <= c && c <= 126 && c != '\'';
    case CharType::IdentifierAndKeyword:
      return '0' <= c && c <= '9' ||
             'a' <= c && c <= 'z' ||
             'A' <= c && c <= 'Z';
    case CharType::OperatorAndPunctuator:
      return 33 <= c && c <= 47 ||
             58 <= c && c <= 64 ||
             91 <= c && c <= 96 ||
             123 <= c && c <= 126;
    default:
      return false;
  }
}

// 숫자 리터럴의 문자열 분석하여 토큰 생성하는 함수
// 현재 문자부터 연속된 숫자 문자들 추출
auto scanNumberLiteral() -> Token {
  string string;
  if (*current == '.') {
    string += *current++;
    while (isCharType(*current, CharType::NumberLiteral))
      string += *current++;
  }
  return Token{Kind::NumberLiteral, string};
}

// 문자열 리터럴의 문자열 분석하여 토큰 생성하는 함수
// 현재 문자는 따옴표
// 현재 문자는 건너뛰고 그 다움 문자부터 따옴표 나오기 전까지의 문자들 추출
auto scanStringLiteral() -> Token {
  string string;
  current++;
  while (isCharType(*current, CharType::StringLiteral))
    string += *current++;
  // 문자열 리터럴 마지막 문자는 따옴표
  // while 문 종료된 시점에 현재 문자 따옴표인지 확인 후 현재 문자 건너뛰기
  if (*current != '\'') {
    cout << "문자열의 종료 문자가 없습니다.";
    exit(1);
  }
  current++;
  return Token{Kind::StringLiteral, string};
}

// 현재 문자는 알파벳
// 현재 문자부터 연속된 알파벳이나 순자 문자들 추출하여 토큰 생성
auto scanIdentifierAndKeyword() -> Token {
  string string;
  while (isCharType(*current, CharType::IdentifierAndKeyword))
    string += *current++;
  auto kind = toKind(string);
  if (kind == Kind::Unknown)
    kind = Kind::Identifier;
  return Token{kind, string};
}

// 현재 문자는 특수문자이므로 현재 문자부터 연속된 특수문자들 추출하여 토큰 생성
auto scanOperatorAndPunctuator() -> Token {
  stirng string;
  while (isCharType(*current, CharType::OperatorAndPunctuator))
    stirng += *current++;
  while (string.empty() == false && toKind(string) == Kind::Unknown) {
    string.pop_back();
    current--;
  }
  if (string.empty()) {
    cout << *current << " 사용할 수 없는 문자입니다.";
    exit(1);
  }
  return Token{toKind(string), string};
}

auto scan(string sourceCode) -> vector<Token> {
  vector<Token> result;
  sourceCode += '\0'; // 문자열 끝에 널 문자 추가
  // 현재 문자 가리키는 전역 변수 (current)
  current = sourceCode.begin();
  while (*current != '\0') {
    // getCharType() 함수; 현재 문자의 종류 반환 (CharType 열거형 멤버 중 하나)
    switch (getCharType(*current)) {
      // WhiteSpace인 경우 현재 문자 무시
      case CharType::WhiteSpace:
        current += 1;
        break;
      case CharType::NumberLiteral:
        result.push_back(scanNumberLiteral());
        break;
      case CharType::StringLiteral:
        result.push_back(scanStringLiteral());
        break;
      case CharType::IdentifierAndKeyword:
        result.push_back(scanIdentifierAndKeyword());
        break;
      case CharType::OperatorAndPunctuator:
        result.push_back(scanOperatorAndPunctuator());
        break;
      default:
        cout << *current << " 사용할 수 없는 문자입니다.";
        exit(1);
    }
  }
  // 토큰 리스트에 EndOfToken 토큰 추가
  result.push_back({Kind::EndOfToken});
  return result;
}