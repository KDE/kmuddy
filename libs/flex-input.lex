%option noyywrap
%{
// Authors:
// Alex Bache <alexbache@ntlworld.com>, 2005
// Tomas Mecir <kmuddy@kmuddy.com>, 2005
//
// Copyright: See COPYING file that comes with this distribution
//
//
    #include "cvalue.h"
    #include "bison-input.tab.hpp"
   
   void yyerror(char *);
   
   using namespace std;
   
   // Use global variables from Bison module
   extern const char *source;
   extern bool        new_source;
   extern bool        token_error;
%}

DIGIT [0-9]
LETTER [A-Za-z_]
SPACE [ ]

%%

   if (new_source)
   {
      // cout << "Switching to new buffer\n";
      // cout.flush();
      
      yy_delete_buffer(YY_CURRENT_BUFFER);
      yy_switch_to_buffer(yy_scan_string(source));
      new_source = false;
   }
   

{DIGIT}+  {
             // Integer
             yylval.int_val = atoi(yytext); 
             return INTEGER;
          }

{DIGIT}+"."{DIGIT}+  {
                        // Double
                        yylval.double_val = QString(yytext).toDouble();
                        return DOUBLE_V;
                     }

${LETTER}({LETTER}|{DIGIT})*  {  
                                 // variable name
                                 yylval.string_val = strdup(yytext+1); 
                                 return STRING_V;
                              }
${DIGIT}+                     {  
                                 // variable name looking like $number
                                 yylval.string_val = strdup(yytext+1); 
                                 return STRING_V;
                              }

{LETTER}({LETTER}|{DIGIT})*{SPACE}*\(   {  
                                   // Function name
                                   // strip the parenthesis
                                   yytext[yyleng-1]  = '\0';
                                   int len = yyleng-2;
                                   // strip the spaces too
                                   for (; len >= 0; --len)
                                     if (yytext[len] == ' ')
                                       yytext[len] = '\0';
                                     else
                                       break;
                                   yylval.string_val = strdup(yytext);
                                   return FUNCTION_NAME;
                                }

\"[^"]*\"  {
              // String literal
              yytext[yyleng-1] = '\0';
              
              yylval.string_val = strdup(yytext+1);
              return STRING_LITERAL;
           }
                              
"(int)"    { return INT_TYPECAST;    }
"(double)" { return DOUBLE_TYPECAST; }
"(string)" { return STRING_TYPECAST; }

"&&" { return AND; }
"||" { return OR;  }
">"  { return GT;  }
">=" { return GE;  }
"<"  { return LT;  }
"<=" { return LE;  }
"==" { return EQ;  }
"!=" { return NE;  }

[-+*/()\.,!]  { return *yytext; }

[ \t\n]    ;

.          { token_error = true; yyterminate(); }

%%
