// instructions.h - definitions low-level instructions emitted by the compiler
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
