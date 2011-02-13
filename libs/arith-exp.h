// arith-exp.h - class to compile and evaluate logical/arithmetical expressions
//
/*
Copyright 2005-2011 Tomas Mecir <kmuddy@kmuddy.com>
Copyright 2005 Alex Bache <alexbache@ntlworld.com>

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License as
published by the Free Software Foundation; either version 2 of 
the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

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
