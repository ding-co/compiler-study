// 프로그램 노드
// 멤버: 함수 노드의 리스트
struct Program {
  vector<struct Function*> functions;
};

// 모든 문 노드와 식 노드의 부모 노드
struct Statement {};
struct Expression {};

// 함수의 정의
// 멤버: 함수 이름, 매개변수 이름 리스트, 실행할 문 리스트
struct Function: Statement {
  string name;
  vector<string> parameters;
  vector<Statement*> block;
};

// return문
// 멤버: 반환식
struct Return: Statement {
  Expression* expression;
};

// 변수의 선언
// 멤버: 변수의 이름, 초기화식
struct Variable: Statement {
  string name;
  Expression* expression;
};

// for문
// 멤버: 변수의 선언, 조건식, 증감식, 실행할 문 리스트
struct For: Statement {
  Variable* variable;
  Expression* condition;
  Expression* expression;
  vector<Statement*> block;
};

// break문과 continue문
// 멤버 없음
struct Break: Statement {};
struct Continue: Statement {};

// if문
// 멤버: 조건식 리스트, 각 조건식의 결과가 참일 때 실행할 문 리스트의 리스트, 거짓일 때 실행할 문 리스트
struct If: Statement {
  vector<Expression*> conditions;
  vector<vector<Statement*>> blocks;
  vector<Statement*> elseBlock;
};

// print문과 printLine문
// 멤버: 개행 여부, 콘솔에 출력할 식 리스트
struct Print: Statement {
  bool lineFeed = false;
  vector<Expression*> arguments;
};

// 식의 문
// 멤버: 식
struct ExpressionStatement: Statement {
  Expression* expression;
};

// 논리 연산자 or, and (이항 연산자)
// 멤버: 왼쪽 식, 오른쪽 식
struct Or: Expression {
  Expression* lhs; // left hand side
  Expression* rhs; // right hand side
};
struct And: Expression {
  Expression* lhs;
  Expression* rhs;
};

// 관계 연산자, 산술 연산자 (이항 연산자)
// 멤버: 연산자의 종류, 왼쪽 피연산자 식, 오른쪽 피연산자 식
struct Relational: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
};
struct Arithmetic: Expression {
  Kind kind;
  Expression* lhs;
  Expression* rhs;
};

// 단항 연산자
// 멤버: 연산자의 종류(+, -), 하나의 피연산자 식
struct Unary: Expression {
  Kind kind;
  Expression* sub;
};

// 함수의 호출
// 멤버: 피연산자 식, 인자식 리스트
struct Call: Expression {
  Expression* sub; // ex. add(1,2)
  vector<Expression*> arguments;
};

// 배열과 맵의 원소 참조
// 멤버: 피연산자 식, 인덱스 식
struct GetElement: Expression {
  Expression* sub; // ex. map['property']
  Expression* index; // ex. array[0]
};

// 배열과 맵의 원소 수정
// 멤버: 피연산자 식, 인덱스 식, 초기화 식
struct SetElement: Expression {
  Expression* sub; // ex. map['property'] = 3;
  Expression* index; // ex. array[0] = 3;
  Expression* value;
};

// 변수의 참조
// 멤버: 변수의 이름
struct GetVariable: Expression {
  string name;
};

// 변수의 수정
// 멤버: 변수의 이름, 초기화식
struct SetVariable: Expression {
  string name;
  Expression* value;
};

// 널 리터럴, 불리언 리터럴, 숫자 리터럴, 스트링 리터럴
// 멤버: 널 리터럴 노드는 없음, 불리언 리터럴 노드는 불리언 값
//      숫자 리터럴 노드는 실수 값, 스트링 리터럴 노드는 문자열 값
struct NullLiteral: Expression {};
struct BooleanLiteral: Expression {
  bool value = false;
};
struct NumberLiteral: Expression {
  double value = 0.0;
};
struct StringLiteral: Expression {
  string value;
};

// 배열 리터럴
// 멤버: 원소식 리스트
struct ArrayLiteral: Expression {
  vector<Expression*> values; // ex. [1, 2, 3]
};

// 맵 리터럴
// 멤버: 문자열과 원소식을 쌍으로 하는 맵
struct MapLiteral: Expression {
  map<string, Expression*> values; // ex. {'a': 1, 'b': 2}
};