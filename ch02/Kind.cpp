// 어휘 문자열들을 열거형의 멤버들과 연관시킬 연관 데이터
static map<string, Kind> stringToKind = {
  {"#unknown",    Kind::Unknown},
  {"#EndOfToken", Kind::EndOfToken},

  {"null",        Kind::NullLiteral},
  {"true",        Kind::TrueLiteral},
  {"false",       Kind::FalseLiteral},
  {"#Number",     Kind::NumberLiteral},
  {"#String",     Kind::StringLiteral},
  {"#identifier", Kind::Identifier},
  {"function",    Kind::Function},
  {"return",      Kind::Return},
  {"var",         Kind::Variable},
  {"for",         Kind::For},
  {"break",       Kind::Break},
  {"continue",    Kind::Continue},
  {"if",          Kind::If},
  {"elif",        Kind::Elif},
  {"else",        Kind::Else},
  {"print",       Kind::Print},
  {"printLine",   Kind::PrintLine},
  
  {"and",         Kind::LogicalAnd},
  {"or",          Kind::LogicalOr},

  {"=",           Kind::Assignment},
  
  {"+",           Kind::Add},
  {"-",           Kind::Subtract},
  {"*",           Kind::Multiply},
  {"/",           Kind::Divide},
  {"%",           Kind::Modulo},

  {"==",          Kind::Equal},
  {"!=",          Kind::NotEqual},
  {"<",           Kind::LessThan},
  {">",           Kind::GreaterThan},
  {"<=",          Kind::LessOrEqual},
  {">=",          Kind::GreaterOrEqual},

  {",",           Kind::Comma},
  {":",           Kind::Colon},
  {";",           Kind::Semicolon},
  {"(",           Kind::LeftParen},
  {")",           Kind::RightParen},
  {"{",           Kind::LeftBrace},
  {"}",           Kind::RightBrace},
  {"[",           Kind::LeftBraket},
  {"]",           Kind::RightBraket},
};

// 매개변수로 받은 문자열이 키워드 문자열들 중 하나가 아니면 Kind::Unknown 반환
auto toKind(string string) -> Kind {
  if (stringToKind.count(string))
    return stringToKind.at(string);
  return Kind::Unknown;
}