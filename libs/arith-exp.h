// arith-exp.h - class to compile and evaluate logical/arithmetical expressions
//
// Authors:
// Alex Bache <alexbache@ntlworld.com>, 2005
// Tomas Mecir <kmuddy@kmuddy.com>, 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef ARITH_EXP_H
#define ARITH_EXP_H

#include <qstring.h>
#include <list>

#include <kmuddy_export.h>

#include "instructions.h"

using namespace std;


//***************************************************************
// Pure virtual base class for supplying variable values and
// function call results
//***************************************************************

class KMUDDY_EXPORT arith_exp_server
{
   public:
      virtual ~arith_exp_server();
      
      // implement this function in your derived class
      virtual cValue get (QString var_name) = 0;

      // implement this function in your derived class
      virtual cValue function_call(const QString &function_name,
                                   list<cValue> &arguments) = 0;

}; // arith_exp_server



//***************************************************************
// Class to compile and evaluate logical/arithmetical expressions
//***************************************************************

class KMUDDY_EXPORT arith_exp
{
   private:
      list<instruction> compiled_code;
      
      // Free resources used to store compiled code
      void clear_compiled();
      
   public:
      ~arith_exp();
      
      // Compiles an expression, returns true if successful
      bool compile (const QString &expression);
      
      // Returns result of executing compiled expression
      cValue evaluate(arith_exp_server *resolver);
      
}; // arith_exp



#endif // ARITH_EXP_H
