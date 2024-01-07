/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_BISON_INPUT_TAB_HPP_INCLUDED
# define YY_YY_BISON_INPUT_TAB_HPP_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    INTEGER = 258,                 /* INTEGER  */
    DOUBLE_V = 259,                /* DOUBLE_V  */
    STRING_V = 260,                /* STRING_V  */
    STRING_LITERAL = 261,          /* STRING_LITERAL  */
    FUNCTION_NAME = 262,           /* FUNCTION_NAME  */
    STRING_TYPECAST = 263,         /* STRING_TYPECAST  */
    INT_TYPECAST = 264,            /* INT_TYPECAST  */
    DOUBLE_TYPECAST = 265,         /* DOUBLE_TYPECAST  */
    AND = 266,                     /* AND  */
    OR = 267,                      /* OR  */
    GT = 268,                      /* GT  */
    GE = 269,                      /* GE  */
    LT = 270,                      /* LT  */
    LE = 271,                      /* LE  */
    EQUALS = 272,                  /* EQUALS  */
    NOT_EQUALS = 273,              /* NOT_EQUALS  */
    EQ = 274,                      /* EQ  */
    NE = 275,                      /* NE  */
    UMINUS = 276,                  /* UMINUS  */
    FUNCTION_CALL = 277            /* FUNCTION_CALL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 61 "libs/bison-input.ypp"

   int     int_val;
   double  double_val;
   char   *string_val;
   int nothing;

#line 93 "bison-input.tab.hpp"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_BISON_INPUT_TAB_HPP_INCLUDED  */
