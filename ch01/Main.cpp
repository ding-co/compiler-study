auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print 'Hello, World!';
    }
  )"""";
  auto tokenList = scan(sourceCode); // 어휘 분석
  auto syntaxTree = parse(tokenList); // 구문 분석
  // interpret(syntaxTree); // 인터프리터
  auto objectCode = generate(syntaxTree); // 컴파일러
  execute(objectCode); // 가상머신
}