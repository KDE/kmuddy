// instructions.h - definitions low-level instructions emitted by the compiler
//
//
// Authors:
// Alex Bache <alexbache@ntlworld.com>, 2005
// Tomas Mecir <kmuddy@kmuddy.com>, 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//

#ifndef INSTRUCTIONS_H
#define INSTRUCTIONS_H

#include "cvalue.h"
#include <kmuddy_export.h>

//***************************************************************
// Class to define a compiled instruction
//***************************************************************

class KMUDDY_EXPORT instruction
{
   public:
      enum instruction_type
      {
         push_value,
         push_variable,
         and_op,
         or_op,
         not_op,
         greater_than,
         greater_or_equal,
         less_than,
         less_or_equal,
         equals,
         not_equal,
         add,
         sub,
         mult,
         div,
         unary_minus,
         cast_to_int,
         cast_to_double,
         cast_to_string,
         string_concat,
         function_call,
         push_func_arg_marker
         
      }; // instruction_type
      
      instruction_type instr;
      cValue           arg;
      
}; // instruction




#endif // INSTRUCTIONS_H
