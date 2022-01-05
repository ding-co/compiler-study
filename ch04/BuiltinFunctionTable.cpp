// 내장 함수 구현

// 전역 변수 builtinFunctionTable
// 내장 함수의 이름과 내장 함수 식을 키와 값으로 함
static map<string, function<any(vector<any>)>> builtinFunctionTable = {
  {"sqrt", [] (vector<any> values) -> any {
    // 제곱근 반환 함수
    return sqrt(toNumber(values[0]))
  }},
};