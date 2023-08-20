//
//  main.cpp
//  mylisp
//

#include <iostream>
#include "mylisp.hpp"

int main(int argc, const char * argv[]) {
  // insert code here...
  
  std::cout << "Hello, World!\n";
  //lisp.parse("(+ 2 3)");
  vector<Lexem> lexs = getLexems("(+ 2 3)");
  
  //TEST_LEXEM1(lexs, LT_OpPar, LT_Atom, LT_Atom);
  //(V, I1) if (!c) { throw std::exception("error");};(lexs, LT_OpPar);
  TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Num, LT_Num, LT_ClosPar);
  TEST_LEXEM_VAL5(lexs, "", "+", "2", "3", "");
  
  {
    vector<Lexem> lexs = getLexems("(+ \"abc\" 2)");
    TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Str, LT_Num, LT_ClosPar);
    TEST_LEXEM_VAL5(lexs, "", "+", "abc", "2", "");

    lexs = getLexems("(+ 2 \"abc\")");
    TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Num, LT_Str, LT_ClosPar);
    TEST_LEXEM_VAL5(lexs, "", "+", "2", "abc", "");

    lexs = getLexems("'(a b)");
    TEST_LEXEM5(lexs, LT_Apos, LT_OpPar, LT_Ident, LT_Ident, LT_ClosPar);
  }
  
  Value val_num("0", AT_number);
  
  {
    //(+ 2 3)
    Value val_arr("", AT_array);
    val_arr._array.push_back(Value("+", AT_proc));
    val_arr._array.push_back(Value(2));
    val_arr._array.push_back(Value(3));
  }
  
  {
    // (streq (concat "a" "bc"), "abc")
    Value val_arr1("", AT_array);
    val_arr1._array.push_back(Value("concat", AT_identifier));
    val_arr1._array.push_back(Value("a", AT_identifier));
    val_arr1._array.push_back(Value("abc", AT_str));

    Value val_arr("", AT_array);
    val_arr._array.push_back(Value("streq", AT_proc));
    val_arr._array.push_back(Value(val_arr1));
    
    
  }

  {
    lexs = getLexems("2");
    Value v;
    parseElement(lexs, lexs.begin(), v);

    //parseLexem(lexs, lexs.begin(), v);
    ASRT(v.getType() == AT_number);
    ASRT(v.asInt() == 2);
  }

  {
    lexs = getLexems("(+ 2 3)");
    Value v;
    vector<Lexem>::iterator lexit = lexs.begin();
    parseElement(lexs, lexit, v);
    ASRT(v._type == AT_array);
    TEST_PARSE_S(v.asArray(0), AT_identifier, "+");
    TEST_PARSE_I(v.asArray(1), AT_number, 2);
    TEST_PARSE_I(v.asArray(2), AT_number, 3);
    Evaluate eval;
    Value vr = eval.getFunc(v);
    ASRT(vr.asInt() == 5);
  }
  {
    lexs = getLexems("(defun bar (a b))");
    Value v;
    vector<Lexem>::iterator lexit = lexs.begin();
    parseElement(lexs, lexit, v);
    TEST_PARSE_S(v.asArray(0), AT_identifier, "defun");
    TEST_PARSE_S(v.asArray(1), AT_identifier, "bar");
    TEST_PARSE_S(v.asArray(2).asArray(0), AT_identifier, "a");
    TEST_PARSE_S(v.asArray(2).asArray(1), AT_identifier, "b");
  }
  {
    lexs = getLexems("(+ 2 (+ 1 (+ 1 1)))");
    Value v;
    vector<Lexem>::iterator lexit = lexs.begin();
    parseElement(lexs, lexit, v);
    ASRT(v._type == AT_array);
    TEST_PARSE_S(v.asArray(0), AT_identifier, "+");
    TEST_PARSE_I(v.asArray(1), AT_number, 2);
    TEST_PARSE_S(v.asArray(2).asArray(0), AT_identifier, "+");
    TEST_PARSE_I(v.asArray(2).asArray(1), AT_number, 1);
    TEST_PARSE_S(v.asArray(2).asArray(2).asArray(0), AT_identifier, "+");
    TEST_PARSE_I(v.asArray(2).asArray(2).asArray(1), AT_number, 1);
    TEST_PARSE_I(v.asArray(2).asArray(2).asArray(2), AT_number, 1);
    Evaluate eval;
    Value vr = eval.getFunc(v);
    ASRT(vr.asInt() == 5);
  }

  
  return 0;
}
