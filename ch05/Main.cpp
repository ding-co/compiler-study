// 구문 트리의 루트 노드 입력받아 목적 코드 출력
auto generate(Program*) -> tuple<vector<Code>, map<string, size_t>>;

auto main() -> void {
  string sourceCode = R""""(
    function main() {
      print 'Hello, World!';
      print 1 * 2 + 3 * 4;
      1 + 2;
      print false or 1 + 2;
      var a = 'first';
      var b = 'second';
      var local = 'hello';
      local = 'world';
      print local;
      global = 'world';
      print global;
      for i=0, i<3; i=i+1 {
        print i;
      }
      if true {
        print 'if';
      } elif true {
        print 'elif';
      } else {
        print 'else';
      }
      for i=0, i<3, i=i+1 {
        continue;
        print i;
      }
      print add(3, 4);
      [1, 2 + 3, 'element'];
      [1, 2][0];
      var array = [1, 2];
      array[0] = 'element';
    }
    
    function add(a, b) {
      return a + b;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);
  auto objectCode = generate(syntaxTree);
  // generate() 함수 반환한 목적 코드 내용 콘솔에 출력 가능
  printObjectCode(objectCode);
}