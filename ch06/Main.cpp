auto main() -> void {
  string sourceCode = R""""(
    function main() {
      printLine 'Hello, World!';
      printLine 1 + 2 * 3;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  auto objectCode = generate(syntaxTree);
  execute(objectCode);

  // Hello, World!
  // 7
}

// 코드 실행 함수 (입력; 코드 리스트, 함수 테이블)
auto execute(tuple<vector<Code>, map<string, size_t>>) -> void;