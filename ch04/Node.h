// 부모 문 노드에 순수 가상 함수 선언
// 문 노드 순회 시 interpret() 가상 함수 호출해 구문 트리 순회
struct Statement {
  virtual auto interpret() -> void = 0;
};

// 부모 문 노드 상속받는 모든 문 노드에 interpret() 함수 선언
struct Function: Statement {
  auto interpret() -> void;
};

// 부모 식 노드에 순수 가상 함수 선언
// 식은 결과값이 있으므로 반환값의 데이터 타입을 any로 선언
// 식 노드를 순회할 때 가상 함수 호출해서 구문 트리 순회
struct Expression {
  virtual auto interpret() -> any = 0;
};

// 부모 식 노드 상속받는 모든 식 노드에 interpret() 함수 선언
struct Or: Expression {
  auto interpret() -> any;
};