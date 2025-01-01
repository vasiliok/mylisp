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

enum AtomType {
  AT_list,
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
  LT_Char,
  LT_Str,
  LT_Ident,
  LT_Unknown
};

enum LispType {
  LSPT_Unknown,
  LSPT_Integer,
  LSPT_Float,
  LSPT_Character,
  LSPT_String,
  LSPT_Symbol,
  LSPT_Sequence,
  LSPT_List,
  LSPT_Array,
  LSPT_Vector,
  LSPT_Hash,
  LSPT_Function,
  LSPT_Macro
};

class Lexem {
public:
  Lexem() : _type(LT_Unknown)
  {}
  Lexem(LexemType t, std::string v ="")
  :_type(t), _val(v)
  {}
  bool isAtom()
  {
    return (_type == LT_Num || _type == LT_Str || _type == LT_Float || _type == LT_Ident) ? true : false;
  }
  LexemType getType() {
    return _type;
  }
  std::string getValue() {
    return _val;
  }
  LexemType _type;
  std::string _val;
};
class Value {
public:
  Value();
  
  Value(AtomType t);

  Value(int v);
  
  Value(std::string v, AtomType t);
  
  Value(const std::vector<Value>& v);
  
  Value(const Value& v);

  const std::vector<Value> & asArray() const {
    return _array;
  }
  std::vector<Value>& asArray() {
    return _array;
  }
  const Value & asArray(int idx) const {
    assert(_type == AT_list);
    return _array[idx];
  }
  const AtomType getType() const {
    return _type;
  }
  void setType(AtomType t) {
    _type = t;
  }
  const std::string asStr() const {
    return _val;
  }
  const int asInt() const {
    return _val_int;
  }
  bool isInt() {
    return _type == AT_number;
  }
  std::string toStr() const;

private:
  AtomType _type;
  std::string _val;
  int _val_int;
  double _val_dbl;
  std::vector<Value> _array;
  LispType _lispType;

};


class ParseElement
{
public:
  ParseElement(std::vector<Lexem>& lexemList)
    : m_lexemList(lexemList)
  {}
  std::vector<Lexem>::iterator parse(Value& value);
  std::vector<Lexem>::iterator parse(std::vector<Lexem>::iterator lexit, Value& value);
private:
  std::vector<Lexem>::iterator parseArray(std::vector<Lexem>::iterator lexit, Value& value);
  std::vector<Lexem> m_lexemList;
};

inline std::vector<Value>::iterator first(std::vector<Value> v) {
  return v.begin();
}

inline std::vector<Value>::iterator first_one(std::vector<Value> v) {
  return v.begin()+1;
}

class Atom {
  AtomType type;
};

//std::vector<Lexem>::iterator parseLexemArray(const std::vector<Lexem>& lexs, std::vector<Lexem>::iterator lexit, Value &value);
//
//// parses input lexem from lexs wich is pointed by lexit, result is stores in value
//// lexs - array of input lexem
//// lexit - iterator in array of input lexem
//// value - result value
//std::vector<Lexem>::iterator parseElement(const std::vector<Lexem>& lexs, std::vector<Lexem>::iterator lexit, Value& value);
//
//// function parses lexem from input array of lexem 'lexs' specified by iterator lexit and save result into value
//// different from parseElement function is assumption list of lexem starts from '(' 
//std::vector<Lexem>::iterator parseLexemArray(const std::vector<Lexem>& lexs, std::vector<Lexem>::iterator lexit, Value& value);


class Evaluate {
public:

  Value getFunc(Value v);

  int plus_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  int minus_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  int mul_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  int div_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  int more_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  int less_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  Value if_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  Value loop_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  Value print_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  Value setq_operator(std::vector<Value>::const_iterator it, std::vector<Value>::const_iterator itend);
  Value progn_operator(std::vector<Value> v);
  Value let_operator(std::vector<Value> v);
  Value defun_operator(std::vector<Value> v);
  Value message_operator(std::vector<Value> v);
  Value getValue(Value v);

private:
  
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

std::vector<Lexem> getLexems(std::string s);

Value run_program(std::string s);


//#define ASRT(C) if (!(C)) { printf(#C); throw std::runtime_error(#C);}
#define ASRT(C) GTEST_EXPECT_TRUE(C)
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
