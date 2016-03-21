%{
#include "parser.h"
%}

%option reentrant
%option noyywrap
%option noinput
%option nounput

%%

"null"                                              return NUL;
"true"                                              return TRUE;
"false"                                             return FALSE;

"{"                                                 return LBRACE;
"}"                                                 return RBRACE;
"["                                                 return LBRACKET;
"]"                                                 return RBRACKET;
","                                                 return COMMA;
":"                                                 return COLON;

\"[^"]*\"                                           return STRING;
-?([0-9]|[1-9][0-9]*(\.[0-9]*)?([eE][+-]?[0-9]+)?)  return NUMBER;

[ \t\n]+                                            ;

%%
