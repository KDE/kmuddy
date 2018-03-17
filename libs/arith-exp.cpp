// arith-exp.cpp - class to compile and evaluate logical/arithmetical expressions
//
// Authors:
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

#include <stack>

#include "arith-exp.h"

using namespace std;


//***************************************************************
// Flex/Bison function prototypes
//***************************************************************

bool compile(const char        *source_code,
             list<instruction> *compiled_code);


//***************************************************************
// Destructor for arith_exp_server class
//***************************************************************

arith_exp_server::~arith_exp_server()
{
   // Empty - keeps compiler quiet
} // arith_exp_server::~arith_exp_server


//***************************************************************
// Destructor for arith_exp class
//***************************************************************

arith_exp::~arith_exp()
{
   clear_compiled();

} // arith_exp::~arith_exp


//***************************************************************
// Function to compile an expression
// Returns true if successful
//***************************************************************

bool arith_exp::compile (const QString &expression)
{
   clear_compiled();
   
   return ::compile(expression.toLocal8Bit().data(),
                    &compiled_code);
   
} // arith_exp::compile


//***************************************************************
// Function to return result of executing compiled expression
//***************************************************************

cValue arith_exp::evaluate(arith_exp_server *resolver)
{
  cValue result;
  cValue data;
  
  typedef list<instruction>::const_iterator iterator;
  
  cValue arg;
  stack<cValue> istack;
  
  iterator position = compiled_code.begin();
  iterator end      = compiled_code.end();
  
  while (position != end)
  {
    switch (position->instr)
    {
      case instruction::push_value:
        istack.push (position->arg);
        break;
        
      case instruction::push_variable:
        istack.push (resolver->get(position->arg.asString()));
        break;
        
      case instruction::and_op:
        data   = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asInteger() && data.asInteger());
        istack.push (arg);
        break;
        
      case instruction::or_op:
        data   = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asInteger() || data.asInteger());
        istack.push (arg);
        break;
      
      case instruction::not_op:
        result = istack.top();
        istack.pop();
        arg = cValue (!result.asInteger());
        istack.push (arg);
        break;
        
      case instruction::greater_than:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() > data.asString());
        else
          arg = cValue (result.asDouble() > data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::greater_or_equal:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() >= data.asString());
        else
          arg = cValue (result.asDouble() >= data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::less_than:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() < data.asString());
        else
          arg = cValue (result.asDouble() < data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::less_or_equal:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() <= data.asString());
        else
          arg = cValue (result.asDouble() <= data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::equals:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() == data.asString());
        else
          arg = cValue (result.asDouble() == data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::not_equal:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (result.isString() && data.isString()) // string comparison
          arg = cValue (result.asString() != data.asString());
        else
          arg = cValue (result.asDouble() != data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::add:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asDouble() + data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::sub:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asDouble() - data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::mult:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asDouble() * data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::div:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        if (data.asDouble() != 0)
          arg = cValue (result.asDouble() / data.asDouble());
        else
          arg = cValue (0);
        istack.push (arg);
        break;
        
      case instruction::unary_minus:
        result = istack.top();
        istack.pop();
        arg = cValue (-result.asDouble());
        istack.push (arg);
        break;
        
      case instruction::cast_to_int:
        data = istack.top();
        istack.pop();
        arg = cValue (data.asInteger());
        istack.push (arg);
        break;
      
      case instruction::cast_to_double:
        data = istack.top();
        istack.pop();
        arg = cValue (data.asDouble());
        istack.push (arg);
        break;
        
      case instruction::cast_to_string:
        data = istack.top();
        istack.pop();
        arg = cValue (data.asString());
        istack.push (arg);
        break;
        
      case instruction::string_concat:
        data = istack.top();
        istack.pop();
        result = istack.top();
        istack.pop();
        arg = cValue (result.asString() + data.asString());
        istack.push (arg);
        break;
        
      case instruction::function_call: {
        list<cValue> arguments;
        
        while (!(istack.top().isMarker()))
        {
          arguments.push_front (istack.top());
          istack.pop();
          
        } // endwhile more function arguments to process
        
        istack.pop();
        
        arg = resolver->function_call (position->arg.asString(), arguments);
        istack.push (arg);
        break;
      }
      
      case instruction::push_func_arg_marker:
        arg.setAsMarker ();
        istack.push (arg);
        break;
      
      default:
        break;
        
    } // endswitch work out which operation to perform
    
    ++position;
  } // endwhile more code to execute
  
  result = istack.top();
  istack.pop();
  return result;
   
} // arith_exp::evaluate



//***************************************************************
// Function to free resources used to store compiled code
//***************************************************************
      
void arith_exp::clear_compiled()
{
   compiled_code.clear();
} // arith_exp::clear_compiled


