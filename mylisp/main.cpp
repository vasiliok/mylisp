//
//  main.cpp
//  mylisp
//

#include <iostream>
#include "gtest/gtest.h"
#include "mylisp.h"

int main(int argc, const char * argv[]) {
  
  using namespace std;
  
  cout << "Hello, Lisp!\n";
  //lisp.parse("(+ 2 3)");
  std::vector<Lexem> lexs = getLexems("(+ 2 3)");
  
  //TEST_LEXEM1(lexs, LT_OpPar, LT_Atom, LT_Atom);
  //(V, I1) if (!c) { throw std::exception("error");};(lexs, LT_OpPar);
  TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Num, LT_Num, LT_ClosPar);
  TEST_LEXEM_VAL5(lexs, "", "+", "2", "3", "");
  
  {
    std::vector<Lexem> lexs = getLexems("(+ \"abc\" 2)");
    TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Str, LT_Num, LT_ClosPar);
    TEST_LEXEM_VAL5(lexs, "", "+", "abc", "2", "");

    lexs = getLexems("(+ 2 \"abc\")");
    TEST_LEXEM5(lexs, LT_OpPar, LT_Ident, LT_Num, LT_Str, LT_ClosPar);
    TEST_LEXEM_VAL5(lexs, "", "+", "2", "abc", "");

    lexs = getLexems("'(a b)");
    TEST_LEXEM5(lexs, LT_Apos, LT_OpPar, LT_Ident, LT_Ident, LT_ClosPar);
  }
  
  //todo Value must get after evaluation and have many different type like nint double bool, or list array, or function etc
  Value val_num("0", AT_number);
  
  {
    //(+ 2 3)
    Value val_arr(AT_list);
    val_arr.asArray().push_back(Value("+", AT_proc));
    val_arr.asArray().push_back(Value(2));
    val_arr.asArray().push_back(Value(3));
  }
  
  {
    // (streq (concat "a" "bc"), "abc")
    Value val_arr1(AT_list);
    val_arr1.asArray().push_back(Value("concat", AT_identifier));
    val_arr1.asArray().push_back(Value("a", AT_identifier));
    val_arr1.asArray().push_back(Value("abc", AT_str));

    Value val_arr(AT_list);
    val_arr.asArray().push_back(Value("streq", AT_proc));
    val_arr.asArray().push_back(Value(val_arr1));
    
    
  }
  //todo getLexem should return lexems with its logical structure like tree when list contain other list
  {
    ParseElement pe(getLexems("2"));
    //lexs = 
    Value v;
    pe.parse(v);

    //parseLexem(lexs, lexs.begin(), v);
    ASRT(v.getType() == AT_number);
    ASRT(v.asInt() == 2);
  }

  {
    ParseElement pe(getLexems("(+ 2 3)"));

    Value v;
    pe.parse(v);
    ASRT(v.getType() == AT_list);
    TEST_PARSE_S(v.asArray(0), AT_identifier, "+");
    TEST_PARSE_I(v.asArray(1), AT_number, 2);
    TEST_PARSE_I(v.asArray(2), AT_number, 3);
    Evaluate eval;
    Value vr = eval.getFunc(v);
    ASRT(vr.asInt() == 5);
  }
  {
    ParseElement pe(getLexems("(defun bar (a b))"));
    Value v;
    pe.parse(v);
    TEST_PARSE_S(v.asArray(0), AT_identifier, "defun");
    TEST_PARSE_S(v.asArray(1), AT_identifier, "bar");
    TEST_PARSE_S(v.asArray(2).asArray(0), AT_identifier, "a");
    TEST_PARSE_S(v.asArray(2).asArray(1), AT_identifier, "b");
  }
  {
    //std::function<int(std::vector<Value>::const_iterator, std::vector<Value>::const_iterator)> ff = std::mem_fn(&Evaluate::plus_operator);
    int(Evaluate::* pf)(std::vector<Value>::const_iterator, std::vector<Value>::const_iterator) = &Evaluate::plus_operator;
    
    ParseElement pe(getLexems("(+ 2 (+ 1 (+ 1 1)))"));
    Value v;
    pe.parse(v);
    GTEST_ASSERT_EQ(v.getType(), AT_list);
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

  {
    lexs = getLexems("\
(progn\
  (setq i 10)\
  (loop (> i 0)\
    (progn\
      (print i)\
      (setq i (- i 1))\
    )\
  )\
)");
    ParseElement pe(lexs);
    Value v;
    pe.parse(v);
    Evaluate eval;
    Value vr = eval.getFunc(v);
  }
  
  {
    Value v = run_program("(progn\
    (let ((birch 3) pine fir (oak 'some)))\
    (message \"Here are '%d' variables with '%s', '%s', and '%s' value.\" birch pine fir oak))");
    std::cout << v.asStr();
  }
  
  {
    Value v = run_program("(defun multiply-by-seven (number)\
\"Multiply NUMBER by seven\"\
(* 7 number))");
    std::cout << v.asStr();
  }
  
  return 0;
}
