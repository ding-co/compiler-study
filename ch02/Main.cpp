// 어휘 분석 함수
auto scan(string) -> vector<Token>;

auto main() -> void {
  string sourceCode = R""""(
    function main() {
      printLine 'Hello, World!';
      printLine 1 + 2 * 3;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  // printTokenist() 함수: 토큰 리스트의 내용을 콘솔에 출력 가능
  printTokenList(tokenList);
}
