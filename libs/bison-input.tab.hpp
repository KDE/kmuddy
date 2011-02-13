/* A Bison parser, made by GNU Bison 2.3.  */

/* Skeleton interface for Bison's Yacc-like parsers in C

   Copyright (C) 1984, 1989, 1990, 2000, 2001, 2002, 2003, 2004, 2005, 2006
   Free Software Foundation, Inc.

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor,
   Boston, MA 02110-1301, USA.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* Tokens.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
   /* Put the tokens into the symbol table, so that GDB and other debuggers
      know about them.  */
   enum yytokentype {
     INTEGER = 258,
     DOUBLE_V = 259,
     STRING_V = 260,
     STRING_LITERAL = 261,
     FUNCTION_NAME = 262,
     STRING_TYPECAST = 263,
     INT_TYPECAST = 264,
     DOUBLE_TYPECAST = 265,
     AND = 266,
     OR = 267,
     GT = 268,
     GE = 269,
     LT = 270,
     LE = 271,
     EQUALS = 272,
     NOT_EQUALS = 273,
     NE = 274,
     EQ = 275,
     UMINUS = 276,
     FUNCTION_CALL = 277
   };
#endif
/* Tokens.  */
#define INTEGER 258
#define DOUBLE_V 259
#define STRING_V 260
#define STRING_LITERAL 261
#define FUNCTION_NAME 262
#define STRING_TYPECAST 263
#define INT_TYPECAST 264
#define DOUBLE_TYPECAST 265
#define AND 266
#define OR 267
#define GT 268
#define GE 269
#define LT 270
#define LE 271
#define EQUALS 272
#define NOT_EQUALS 273
#define NE 274
#define EQ 275
#define UMINUS 276
#define FUNCTION_CALL 277




#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE
#line 61 "bison-input.ypp"
{
   int     int_val;
   double  double_val;
   char   *string_val;
   int nothing;
}
/* Line 1489 of yacc.c.  */
#line 100 "bison-input.tab.hpp"
	YYSTYPE;
# define yystype YYSTYPE /* obsolescent; will be withdrawn */
# define YYSTYPE_IS_DECLARED 1
# define YYSTYPE_IS_TRIVIAL 1
#endif

extern YYSTYPE yylval;

