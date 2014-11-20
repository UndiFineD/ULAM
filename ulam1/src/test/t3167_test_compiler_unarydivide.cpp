#include "TestCase_EndToEndCompiler.h"

namespace MFM {

  BEGINTESTCASECOMPILER(t3167_test_compiler_unarydivide)
  {
    std::string GetAnswerKey()
    {
      return std::string("Ue_A { Unary(3) a(2);  Unary(3) b(3);  Unary(3) c(0);  Int(32) test() {  a 2 cast = b 4 cast = c a cast b cast / cast = c cast return } }\nExit status: 0");
    }
    
    std::string PresetTest(FileManagerString * fms)
    {
      //2 / 1 == 2 bit
      bool rtn1 = fms->add("A.ulam","element A { Unary(3) a, b, c; use test;  a = 2; b = 4; c = a / b; return c; } }");
      bool rtn2 = fms->add("test.ulam", "Int test() {");
      
      if(rtn1 & rtn2)
	return std::string("A.ulam");
      
      return std::string("");
    }
  }
  
  ENDTESTCASECOMPILER(t3167_test_compiler_unarydivide)
  
} //end MFM

