// 구문 분석 함수
auto parse(vector<Token>) -> Program*;

auto main() -> void {
  string sourceCode = R""""(
    function main() {
      printLine 'Hello, World!';
      printLine 1 + 2 * 3;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);

  // parse() 함수 반환한 구문 트리의 내용을 콘솔에 출력
  printSyntaxTree(syntaxTree);
}