//
//  mylisp.cpp
//  mylisp
//
//

#include "mylisp.h"
#include <string>
#include <functional>
 Value::Value()
  : _type(AT_unknown)
  , _lispType(LSPT_Unknown)
  , _val_int(0)
 {}

 Value::Value(AtomType t)
  : _type(t)
  , _lispType(LSPT_Unknown)
  , _val_int(0)
  , _val_dbl(.0)
{}
  
Value::Value(int v)
  : _type(AT_number)
  , _lispType(LSPT_Unknown)
  , _val_int(v)
{}

Value::Value(std::string v, AtomType t)
  : _type(t)
  , _val(v)
  , _val_int(0)
  , _lispType(LSPT_Unknown)
{}

Value::Value(const std::vector<Value>& v)
  : _type(AT_list)
  , _array(v)
  , _val_int(0)
{}

Value::Value(const Value& v)
  : _type(v._type)
{
  switch (v._type) {
  case AT_number:
  case AT_nil:
    _val_int = v._val_int;
    break;
  case AT_str:
  case AT_identifier:
  case AT_proc:
    _val = v._val;
    break;
  case AT_list:
    _array = v._array;
    break;
  default:
    break;
  }
}

std::string Value::toStr() const {
  if (_type == AT_str) {
    return _val;
  }
  else if (_type == AT_number) {
    return std::to_string(_val_int);
  }
  else if (_type == AT_nil) {
    return "nil";
  }
  InterpreterException("unexpected type");
  return "";
}

Value run_program(std::string s) {
  Value v;
  std::vector<Lexem> lexs = getLexems(s);
  std::vector<Lexem>::iterator lexit = lexs.begin();
  ParseElement pe(lexs);
  pe.parse(v);
  Evaluate eval;
  Value vr = eval.getFunc(v);
  return vr;
}

std::vector<Lexem> getLexems(std::string s)
{
  std::vector<Lexem> lexs;
  std::string::iterator sit = s.begin();
  while (sit != s.end())
  {
    LexemType ltype = LT_Unknown;
    char c = *sit;
    if (!isspace(c))
    {
      if (c == '(') {
        lexs.push_back(Lexem(LT_OpPar));
      }
      else if (c == ')') {
        lexs.push_back(Lexem(LT_ClosPar));
      }
      else if (c == '\'') {
       lexs.push_back(Lexem(LT_Apos));
      }
      else {
        std::string atm;
        if (*sit == '\"')
        {
          sit++;
          while (*sit != '\"' && sit != s.end())
          {
            atm += *sit;
            sit++;
          }
          ltype = LT_Str;
          sit++;
        }
        else
        {
          ltype = LT_Num;
          while (sit != s.end() && !isspace(*sit) && *sit != '(' && *sit != ')' && *sit != '\'')
          {
            atm += *sit;
            if (!isdigit(*sit))
            {
              ltype = LT_Ident;
            }
            sit++;
          }
          
        }
        lexs.push_back(Lexem(ltype, atm));
        continue;
      }
    }
    sit++;
  }
  return lexs;
}

std::vector<Lexem>::iterator ParseElement::parse(Value& value)
{
  return parse(m_lexemList.begin(), value);
}
std::vector<Lexem>::iterator ParseElement::parse(std::vector<Lexem>::iterator lexit, Value& value)
{
  switch (lexit->getType()) {
  case LT_OpPar:
    lexit = parseArray(++lexit, value);
    break;
  case LT_Num:
    value = Value(atoi(lexit->getValue().c_str()));
    break;
  case  LT_Str:
    value = Value(lexit->getValue(), AT_str);
    break;
  case LT_Ident:
    value = Value(lexit->getValue(), AT_identifier);
    break;
  case LT_Apos:
    value = Value(lexit->getValue(), AT_identifier);
    break;
  case LT_ClosPar:
    break;
  }
  return lexit;
}

std::vector<Lexem>::iterator ParseElement::parseArray(std::vector<Lexem>::iterator lexit, Value& value)
{
  value.setType(AT_list);
  for (; lexit != m_lexemList.end(); ++lexit) {
    if (lexit->getType() == LT_ClosPar) {
      break;
    }
    Value v;
    lexit = parse(lexit, v);
    value.asArray().push_back(v);
  }
  return lexit;
}
//std::map<std::string, std::mem_fn

Value Evaluate::getFunc(Value v)
{
  THROWIF(v.getType() != AT_list, "Value must be array");

  Value f = *v.asArray().begin();
  if (f.asStr() == "+") {
    if (v.getType() != AT_list) throw "Need array";
    return Value(plus_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "-") {
    return Value(minus_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "*") {
    return Value(mul_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "/") {
    return Value(div_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == ">") {
    return Value(more_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "<") {
    return Value(less_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "if") {
    return Value(if_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "loop") {
    return Value(loop_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "print") {
    return Value(print_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "setq") {
    return Value(setq_operator(v.asArray().begin() + 1, v.asArray().end()));
  }
  else if (f.asStr() == "progn") {
    return Value(progn_operator(v.asArray()));
  }
  else if (f.asStr() == "let") {
    return Value(let_operator(v.asArray()));
  }
  else if (f.asStr() == "message") {
    return Value(message_operator(v.asArray()));
  }
  else if (f.asStr() == "defun") {
    return Value(defun_operator(v.asArray()));
  }
  else {
    throw InterpreterException("unknown operator");
  }
  return Value();
}

int Evaluate::plus_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("+");
  THROWIF(distance(it, itend) < 2, "operator '+' needs 2 arguments");
  return getValue(*it).asInt() + getValue(*(it + 1)).asInt();
}
int Evaluate::minus_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("-");
  THROWIF(distance(it, itend) < 2, "operator '-' needs 2 arguments");
  return getValue(*it).asInt() - getValue(*(it + 1)).asInt();
}
int Evaluate::mul_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("*");
  THROWIF(distance(it, itend) < 2, "operator '*' needs 2 arguments");
  return getValue(*it).asInt() * getValue(*(it + 1)).asInt();
}
int Evaluate::div_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("/");
  THROWIF(distance(it, itend) < 2, "operator '/' needs 2 arguments");
  return getValue(*it).asInt() / getValue(*(it + 1)).asInt();
}
int Evaluate::more_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back(">");
  THROWIF(distance(it, itend) < 2, "operator '>' needs 2 arguments");
  return getValue(*it).asInt() > getValue(*(it + 1)).asInt();
}
int Evaluate::less_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("<");
  THROWIF(distance(it, itend) < 2, "operator '<' needs 2 arguments");
  return getValue(*it).asInt() < getValue(*(it + 1)).asInt();
}
Value Evaluate::if_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("if");
  THROWIF(distance(it, itend) < 3, "operator 'if' needs 3 arguments");
  return  getValue(*it).asInt() > 0 ? getValue(*(it + 1)) : getValue(*(it + 2));
}

Value Evaluate::loop_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("loop");
  THROWIF(distance(it, itend) < 1, "operator 'loop' needs 1 arguments");
  while (getValue(*it).asInt()) {
    getValue(*(it + 1));
  }
  return Value();
}

Value Evaluate::print_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("print");
  printf("%s", getValue(*it).toStr().c_str());
  return Value();
}

Value Evaluate::setq_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend) {
  m_stack_trace.push_back("setq");
  m_var_map[it->asStr()] = getValue(*(it + 1));
  return Value();
}
Value Evaluate::progn_operator(std::vector<Value> v) {
  m_stack_trace.push_back("progn");
  Value lastv;
  for (auto i = v.begin() + 1; i != v.end(); ++i) {
    lastv = getValue(*i);
  }
  return lastv;
}
Value Evaluate::let_operator(std::vector<Value> v) {
  m_stack_trace.push_back("let");
  Value lastv;
  THROWIF(v.size() <= 1, "argument count must be more 0");

  //THROWIF((v.begin()+1)->getType() != AT_array, "first argument must be array");
  auto let_params = v[1].asArray();
  for (auto m : let_params) {
    if (m.getType() == AT_list) {
      m_var_map[m.asArray(0).asStr()] = getValue(m.asArray(1));
    }
    else {
      m_var_map[m.asStr()] = 0;
    }
  }
  return lastv;
}

Value Evaluate::defun_operator(std::vector<Value> v)
{
  m_stack_trace.push_back("defun");
  return Value();
}

Value Evaluate::message_operator(std::vector<Value> v) {
  std::string fmt = v[1].asStr();
  auto c = fmt.begin();
  int idx = 1;
  std::string res;
  while (c != fmt.end()) {
    if (*c == '%') {
      if (++c != fmt.end()) {
        if (*c == 'd') {
          res += std::to_string(getValue(v[idx]).asInt());
        }
        else if (*c == 's') {
          res += getValue(v[idx]).asStr();
        }
        idx++;
      }
    }
    else {
      res += *c;
    }
    c++;
  }
  std::cout << res;
  return Value(res, AT_str);
}

Value Evaluate::getValue(Value v) {
  m_stack_trace.push_back("getValue");
  if (v.getType() == AT_list) {
    return getFunc(v);
  }
  else if (v.getType() == AT_identifier) {
    return m_var_map[v.asStr()];
  }
  return v;
}


//std::vector<Lexem>::iterator parseLexemArray(const std::vector<Lexem>& lexs, std::vector<Lexem>::iterator lexit, Value& value)
//{
//  value.setType(AT_list);
//  for (; lexit != lexs.end(); ++lexit) {
//    if (lexit->getType() == LT_ClosPar) {
//      break;
//    }
//    Value v;
//    lexit = parseElement(lexs, lexit, v);
//    value.asArray().push_back(v);
//  }
//  return lexit;
//}


