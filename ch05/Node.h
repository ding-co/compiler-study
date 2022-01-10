// 노드에 대해 구문 트리 순회 위한 가상 함수 정의

// 부모 문 노드에 순수 가상 함수 선언
struct Statement {
  virtual auto generate() -> void = 0;
};

// 부모 문 노드를 상속받는 모든 문 노드에 generate() 함수 선언
struct Function: Statement {
  auto generate() -> void;
};

// 부모 식 노드에 순수 가상 함수 선언
struct Expression {
  virtual auto generate() -> void = 0;
};

// 부모 식 노드 상속받는 모든 식 노드에 generate() 함수 선언
struct Or: Expression {
  auto generate() -> void;
};