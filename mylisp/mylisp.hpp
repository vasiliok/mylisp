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
using namespace std;

enum AtomType
{
  AT_array,
  AT_number,
  AT_str,
  AT_identifier,
  AT_proc,
  AT_nil,
  AT_unknown
};

enum LexemType
{
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

class Value
{
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
  
  AtomType _type;
  std::string _val;
  int _val_int;
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
};

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
    Value f = *v.asArray().begin();
    if (f.asStr() == "+") {
      return Value(plus_operator(v.asArray().begin()+1));
    }
    if (f.asStr() == "-") {
      return Value(minus_operator(v.asArray().begin()+1));
    }
    if (f.asStr() == "*") {
      return Value(mul_operator(v.asArray().begin()+1));
    }
    if (f.asStr() == "/") {
      return Value(div_operator(v.asArray().begin()+1));
    }
    return Value();
  }
  
  int plus_operator(vector<Value>::const_iterator it) {
    return getValue(*it).asInt() + getValue(*(it+1)).asInt();
  }
  int minus_operator(vector<Value>::const_iterator it) {
    return getValue(*it).asInt() - getValue(*(it+1)).asInt();
  }
  int mul_operator(vector<Value>::const_iterator it) {
    return getValue(*it).asInt() * getValue(*(it+1)).asInt();
  }
  int div_operator(vector<Value>::const_iterator it) {
    return getValue(*it).asInt() / getValue(*(it+1)).asInt();
  }

  Value getValue(Value v) {
    if (v.getType() == AT_array) {
      return getFunc(v);
    }
    return v;
  }
  
  
  
};

//inline vector<Lexem>::iterator parseLexem(const vector<Lexem>& lexs, vector<Lexem>::iterator lexit, Value &value)
//{
//  if (lexit->getType() == LT_OpPar)
//  {
//    value._type = AT_array;
//  }
//  Value v;
//
//  for (;lexit != lexs.end(); ++lexit)
//  {
//    Value v_;
//
//    switch (lexit->getType()) {
//      case LT_OpPar:
//        //value_stack.push(val);
//        value._type = AT_array;
//        lexit = parseLexem(lexs, ++lexit, v, true);
//        value._array.push_back(v);
//        break;
//      case LT_Num:
//        v_ = Value(atoi(lexit->getValue().c_str()));
//        break;
//      case  LT_Str:
//        v_ = Value(lexit->getValue(), AT_str);
//        break;
//      case LT_Ident:
//        v_ = Value(lexit->getValue(), AT_identifier);
//        break;
//      case LT_Apos:
//        v_ = Value(lexit->getValue(), AT_identifier);
//        break;
//      case LT_ClosPar:
//        value = v;
//        break;
//    }
//
//  }
//  return lexit;
//}

    
//    AtomType type = AT_unknown;
//    std::string str;
//    while (!isspace(*v))
//    {
//      if (isdigit(*v))
//      {
//        if (type == AT_unknown)
//        {
//          type = AT_number;
//        }
//      }
//      else if (isalpha(*v))
//      {
//        if (type != AT_str)
//        {
//          type = AT_identifier;
//        }
//      }
//      else if (*v == '\'')
//      {
//        if (type == AT_unknown)
//        {
//          type= AT_str;
//          v++;
//        }
//        else if(type == AT_str)
//        {
//          v++;
//        }
//      }
//      str += *v;
//      v++;
//
//    }
//    return Value(str, type);

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
