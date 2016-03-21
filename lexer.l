%{
#include "parser.h"
%}

%option reentrant
%option noyywrap
%option noinput
%option nounput

%%

"null"              return NUL;
"true"              return TRUE;
"false"             return FALSE;

"{"                 return LBRACE;
"}"                 return RBRACE;
"["                 return LBRACKET;
"]"                 return RBRACKET;
","                 return COMMA;
"."                 return DOT;
":"                 return COLON;
"-"                 return MINUS;

(e|e\+|e-|E|E\+|E-) return EXP;
\"[^"]*\"           return STRING;
([0-9]|[1-9][0-9]+) return DIGITS;

[ \t\n]+            ;

%%
