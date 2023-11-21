//
//  mylisp.hpp
//  mylisp
//
//

#ifndef __mylisp_hpp__
#define __mylisp_hpp__

#include <string>
#include <list>
#include <vector>
#include <cassert>
#include <stdexcept>
#include <stack>
#include <exception>
#include <stdexcept>
#include <map>
#include <iostream>

class InterpreterException : public std::runtime_error
{
public:
  explicit InterpreterException(const std::string& s)
  : std::runtime_error(s)
  {}
  explicit InterpreterException(const char* pc)
  : std::runtime_error(pc)
  {}

};

#define THROWIF(C, M) if ((C)) throw InterpreterException(M);
using namespace std;

enum AtomType {
  AT_array,
  AT_number,
  AT_str,
  AT_identifier,
  AT_proc,
  AT_nil,
  AT_unknown
};
enum ValueType {
  VT_string,
  VT_number,
  VT_float,
  VT_double,
  VT_bool,
  VT_ident,
  VT_proced
};
enum LexemType {
  LT_OpPar,
  LT_ClosPar,
  LT_Apos,
  LT_Num,
  LT_Float,
  LT_Str,
  LT_Ident,
  LT_Unknown
};

class Lexem {
public:
  Lexem() : _type(LT_Unknown)
  {}
  Lexem(LexemType t, string v ="")
  :_type(t), _val(v)
  {}
  bool isAtom()
  {
    return (_type == LT_Num || _type == LT_Str || _type == LT_Float || _type == LT_Ident) ? true : false;
  }
  LexemType getType() {
    return _type;
  }
  string getValue() {
    return _val;
  }
  LexemType _type;
  string _val;
};
class Value {
public:
  Value()
  : _type(AT_unknown)
  , _val_int(0)
  {}
  
  Value(int v)
  : _type(AT_number)
  , _val_int(v)
  {}
  
  Value(std::string v, AtomType t)
  : _type(t)
  , _val(v)
  , _val_int(0)
  {}
  
  Value(const vector<Value> &v)
  : _type(AT_array)
  , _array(v)
  , _val_int(0)
  {}
  
  Value(const Value& v) : _type(v._type)
  {
    switch(v._type) {
        case AT_number:
        case AT_nil:
          _val_int = v._val_int;
          break;
        case AT_str:
        case AT_identifier:
        case AT_proc:
          _val = v._val;
          break;
        case AT_array:
          _array = v._array;
          break;
        default:
          break;
    }
  }
  AtomType _type;
  std::string _val;
  int _val_int;
  double _val_dbl;
  vector<Value> _array;
  
  const vector<Value> & asArray() const {
    return _array;
  }
  const Value & asArray(int idx) const {
    assert(_type == AT_array);
    return _array[idx];
  }
  const AtomType getType() const {
    return _type;
  }
  void setType(AtomType t) {
    _type = t;
  }
  const string asStr() const {
    return _val;
  }
  const int asInt() const {
    return _val_int;
  }
  bool isInt() {
    return _type == AT_number;
  }
  string toStr() const {
    if (_type == AT_str) {
      return _val;
    }
    else if (_type == AT_number) {
      return to_string(_val_int);
    }
    else if (_type == AT_nil) {
      return "nil";
    }
    InterpreterException("unexpected type");
    return "";
  }
};

inline vector<Value>::iterator first(vector<Value> v) {
  return v.begin();
}

inline vector<Value>::iterator first_one(vector<Value> v) {
  return v.begin()+1;
}

class Atom {
  AtomType type;
};

inline vector<Lexem>::iterator parseLexemArray(const vector<Lexem>& lexs, vector<Lexem>::iterator lexit, Value &value);

inline vector<Lexem>::iterator parseElement(const vector<Lexem>& lexs, vector<Lexem>::iterator lexit, Value &value)
{
  switch (lexit->getType()) {
    case LT_OpPar:
      lexit = parseLexemArray(lexs, ++lexit, value);
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

inline vector<Lexem>::iterator parseLexemArray(const vector<Lexem>& lexs, vector<Lexem>::iterator lexit, Value &value)
{
  value._type = AT_array;
  for (;lexit != lexs.end(); ++lexit) {
    if (lexit->getType() == LT_ClosPar) {
      break;
    }
    Value v;
    lexit = parseElement(lexs, lexit, v);
    value._array.push_back(v);
  }
  return lexit;
}

class Evaluate {
public:

  Value getFunc(Value v) {
    THROWIF(v.getType() != AT_array, "Value must be array");
    Value f = *v.asArray().begin();
    if (f.asStr() == "+") {
      if (v.getType() != AT_array) throw "Need array";
      return Value(plus_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "-") {
      return Value(minus_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "*") {
      return Value(mul_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "/") {
      return Value(div_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == ">") {
      return Value(more_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "<") {
      return Value(less_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "if") {
      return Value(if_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "loop") {
      return Value(loop_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "print") {
      return Value(print_operator(v.asArray().begin()+1, v.asArray().end()));
    }
    else if (f.asStr() == "setq") {
      return Value(setq_operator(v.asArray().begin()+1, v.asArray().end()));
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
    else {
      throw InterpreterException("unknown operator");
    }
    return Value();
  }
  
  int plus_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("+");
    THROWIF(distance(it, itend) < 2, "operator '+' needs 2 arguments");
    return getValue(*it).asInt() + getValue(*(it+1)).asInt();
  }
  int minus_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("-");
    THROWIF(distance(it, itend) < 2, "operator '-' needs 2 arguments");
    return getValue(*it).asInt() - getValue(*(it+1)).asInt();
  }
  int mul_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("*");
    THROWIF(distance(it, itend) < 2, "operator '*' needs 2 arguments");
    return getValue(*it).asInt() * getValue(*(it+1)).asInt();
  }
  int div_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("/");
    THROWIF(distance(it, itend) < 2, "operator '/' needs 2 arguments");
    return getValue(*it).asInt() / getValue(*(it+1)).asInt();
  }
  int more_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back(">");
    THROWIF(distance(it, itend) < 2, "operator '>' needs 2 arguments");
    return getValue(*it).asInt() > getValue(*(it+1)).asInt();
  }
  int less_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("<");
    THROWIF(distance(it, itend) < 2, "operator '<' needs 2 arguments");
    return getValue(*it).asInt() < getValue(*(it+1)).asInt();
  }
  Value if_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("if");
    THROWIF(distance(it, itend) < 3, "operator 'if' needs 3 arguments");
    return  getValue(*it).asInt() > 0 ? getValue(*(it+1)) : getValue(*(it+2));
  }
  
  Value loop_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("loop");
    THROWIF(distance(it, itend) < 1, "operator 'loop' needs 1 arguments");
    while(getValue(*it).asInt()) {
      getValue(*(it + 1));
    }
    return Value();
  }

  Value print_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("print");
    printf("%s", getValue(*it).toStr().c_str());
    return Value();
  }

  Value setq_operator(vector<Value>::const_iterator it, vector<Value>::const_iterator itend) {
    m_stack_trace.push_back("setq");
    m_var_map[it->asStr()] = getValue(*(it+1));
    return Value();
  }
  Value progn_operator(vector<Value> v) {
    m_stack_trace.push_back("progn");
    Value lastv;
    for (auto i = v.begin() + 1; i != v.end(); ++i) {
      lastv = getValue(*i);
    }
    return lastv;
  }
  Value let_operator(vector<Value> v) {
    m_stack_trace.push_back("let");
    Value lastv;
    THROWIF(v.size() <= 1, "argument count must be more 0");
    
    //THROWIF((v.begin()+1)->getType() != AT_array, "first argument must be array");
        auto let_params = v[1].asArray();
        for (auto m : let_params) {
          if (m.getType() == AT_array) {
            m_var_map[m.asArray(0).asStr()] = getValue(m.asArray(1));
          }
          else {
            m_var_map[m.asStr()] = 0;
          }
        }
    return lastv;
  }
  Value message_operator(vector<Value> v) {
    std::string fmt = v[1].asStr();
    auto c = fmt.begin();
    int idx=1;
    std::string res;
    while(c != fmt.end()) {
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

  Value getValue(Value v) {
    m_stack_trace.push_back("getValue");
    if (v.getType() == AT_array) {
      return getFunc(v);
    }
    else if (v.getType() == AT_identifier){
      return m_var_map[v.asStr()];
    }
    return v;
  }
  
  std::map<std::string, Value> m_var_map;
  std::map<std::string, Value> m_fn_map;
  std::vector<std::string> m_stack_trace;

};


class MyLisp
{
  public:
  int getString(std::string::iterator v)
  {
    return 0;
  }
  
  Value parseAtom(std::string::iterator v)
  {
    AtomType type = AT_unknown;
    std::string str;
    while (!isspace(*v))
    {
      if (isdigit(*v))
      {
        if (type == AT_unknown)
        {
          type = AT_number;
        }
      }
      else if (isalpha(*v))
      {
        if (type != AT_str)
        {
          type = AT_identifier;
        }
      }
      else if (*v == '\'')
      {
        if (type == AT_unknown)
        {
          type = AT_str;
          v++;
        }
        else if(type == AT_str)
        {
          v++;
        }
      }
      str += *v;
      v++;
      
    }
    return Value(str, type);
  }


  
  int parseElement(std::string::iterator v)
  {
    std::vector<Value> atomList;
    while (*v != ')')
    {
      //atomList = parseAtom(v);
    }
    char c = *v;
    if (c == '"')
    {
      getString(v);
    }
    else {
      //getValue(v);
    }
    return 0;
  }
  int parseList(std::string::iterator v)
  {
    std::string vs;
    char c = *v;
    while (c != ')')
    {
      if (c == '(')
      {
        parseList(++v);
      }
      else {
        parseElement(v);
      }
      v++;
      vs += c;
    }
   return 0;
  }
  
  
  int parse(std::string s)
  {
    parseList(s.begin());
    return 0;
  }
};

vector<Lexem> getLexems(string s);
inline Value run_program(std::string s) {
  Value v;
  vector<Lexem> lexs = getLexems(s);
  vector<Lexem>::iterator lexit = lexs.begin();
  parseElement(lexs, lexit, v);
  Evaluate eval;
  Value vr = eval.getFunc(v);
  return vr;
}

#define ASRT(C) if (!(C)) { printf(#C); throw std::runtime_error(#C);}
#define ASRTEQ(V,E) if (V!=E) {printf(#V); printf("; "); printf(#E);}
#define TEST_LEXEM1(V, I1) ASRT(V[0]._type == I1);
#define TEST_LEXEM2(V, I1, I2) ASRT(V[1]._type == I2); TEST_LEXEM1(V, I1)
#define TEST_LEXEM3(V, I1, I2, I3) ASRT(V[2]._type == I3); TEST_LEXEM2(V, I1, I2)
#define TEST_LEXEM4(V, I1, I2, I3, I4) ASRT(V[3]._type == I4); TEST_LEXEM3(V, I1, I2, I3)
#define TEST_LEXEM5(V, I1, I2, I3, I4, I5) ASRT(V[4]._type == I5); TEST_LEXEM4(V, I1, I2, I3, I4)
#define TEST_LEXEM6(V, I1, I2, I3, I4, I5, I6) ASRT(V[5]._type == I6); TEST_LEXEM5(V, I1, I2, I3, I4, I5)


#define TEST_LEXEM_VAL1(V, I1) ASRT(V[0]._val == I1);
#define TEST_LEXEM_VAL2(V, I1, I2) ASRT(V[1]._val == I2); TEST_LEXEM_VAL1(V, I1)
#define TEST_LEXEM_VAL3(V, I1, I2, I3) ASRT(V[2]._val == I3); TEST_LEXEM_VAL2(V, I1, I2)
#define TEST_LEXEM_VAL4(V, I1, I2, I3, I4) ASRT(V[3]._val == I4); TEST_LEXEM_VAL3(V, I1, I2, I3)
#define TEST_LEXEM_VAL5(V, I1, I2, I3, I4, I5) ASRT(V[4]._val == I5); TEST_LEXEM_VAL4(V, I1, I2, I3, I4)
#define TEST_LEXEM_VAL6(V, I1, I2, I3, I4, I5, I6) ASRT(V[5]._val == I6); TEST_LEXEM_VAL5(V, I1, I2, I3, I4, I5)

#define TEST_PARSE_TYPE(v, t) ASRT(v._type == t)
#define TEST_PARSE_S(o, t, v) ASRT(o.getType() == t); ASRT(o.asStr() == v);
#define TEST_PARSE_I(o, t, v) ASRT(o.getType() == t); ASRT(o.asInt() == v);

#endif /* mylisp_hpp */
