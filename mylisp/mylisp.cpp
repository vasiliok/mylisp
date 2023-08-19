//
//  mylisp.cpp
//  mylisp
//
//

#include "mylisp.hpp"

vector<Lexem> getLexems(string s)
{
  vector<Lexem> lexs;
  string::iterator sit = s.begin();
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
        string atm;
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
          while (!isspace(*sit) && sit != s.end() && *sit != '(' && *sit != ')' && *sit != '\'')
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

