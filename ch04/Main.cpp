// 구문 트리 실행 함수
// 구문 트리의 루트 노드를 입력 받음
auto interpret(Program*) -> void;

auto main() -> void {
  string sourceCode = R""""(
    function main() {
      printLine 'Hello, World!';
      printLine 1 + 2 * 3;
      printLine true or 'Hello, World!';
      printLine false or 'Hello, World!';
      printLine true and 'Hello, World!';
      printLine false and 'Hello, World!';
      global = 4;
      var local = 13;
      global = local = 7'
      printLine 'global: ', global;
      printLine 'local: ', local;
      for i=0, i<3, i=i+1 {
        printLine 'i: ', i;
      }
      for i=0, i<5, i=i+1 {
        if i == 1 {
          printLine 'one';
        } elif i == 2 {
          printLine 'two';
        } elif i == 3 {
          printLine 'three';
        } else {
          printLine i;
        }
      }
      for i=0, i<3, i=i+1 {
        if i == 1 {
          continue;
        }
        printLine 'i: ', i;
      }
      sayHoHoHo();
      add(1, 3);
      print getC(3, 4);
      print sqrt(getC(3, 4));
      print [1, 2, 3];
      print ['first, 'second', 'third'][1];
      var array = ['first', 'second', 'third'];
      array[1] = '2nd';
      printLine array[1];
    }

    function sayHoHoHo() {
      print 'Ho! Ho! Ho!';
    }

    function add(a, b) {
      print a + b;
    }

    function getC(a, b) {
      return a * a + b * b;
    }
  )"""";
  auto tokenList = scan(sourceCode);
  auto syntaxTree = parse(tokenList);

  // 구문 트리의 루트 노드를 인자로 넘김
  interpret(syntaxTree);

  // [출력 결과]
  // Hello, World!
  // 7
  // true
  // Hello,World!
  // Hello, World!
  // false
  // global: 7
  // local: 7
  // i: 0
  // i: 1
  // i: 2
  // 0
  // one
  // two
  // three
  // 4
  // i: 0
  // i: 2
  // Ho! Ho! Ho!
  // 4
  // 25
  // 5
  // [ 1 2 3 ]
  // second
  // 2nd
}