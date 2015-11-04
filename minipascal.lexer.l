%option noyywrap yylineno caseless

%{
    #include <ctype.h>
    #include <string.h>

    #include "minipascal.parser.tab.h"

    char *lowerString(const char* string_to_lower);   
%}

NUMBER [0-9]
EXP ([Ee][\-\+]?[0-9]+)

%%
"+"                                                    {yylval.arithmetic = 1; return SUM_OPERATOR;}
"-"                                                    {yylval.arithmetic = 2; return SUM_OPERATOR;}
"or"|"OR"                                              {yylval.arithmetic = 3; return SUM_OPERATOR;}

"*"                                                    {yylval.arithmetic = 4; return MULT_OPERATOR;}
"/"                                                    {yylval.arithmetic = 5; return MULT_OPERATOR;}
"div"|"DIV"                                            {yylval.arithmetic = 6; return MULT_OPERATOR;}
"and"|"AND"                                            {yylval.arithmetic = 7; return MULT_OPERATOR;}

"("|")"|","|";"|"."|":"|"{"|"}"                        {return yytext[0];}



">"                                                    {    yylval.relational = 8;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<"                                                    {    yylval.relational = 9;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<>"                                                   {    yylval.relational = 10;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<="                                                   {    yylval.relational = 11;
                                                            return RELATIONAL_OPERATOR;
                                                       }                            
">="                                                   {    yylval.relational = 12;
                                                            return RELATIONAL_OPERATOR;
                                                       }

":="                                                   {return ASSIGNMENT;}

"program"|"PROGRAM"                                    {return PROGRAM;}
"begin"|"BEGIN"                                        {return B3GIN;}
"end"|"END"                                            {return END;}
"var"|"VAR"                                            {return VAR;}
"procedure"|"PROCEDURE"                                {return PROCEDURE;}
"array"|"ARRAY"                                        {return ARRAY;}
"of"|"OF"                                              {return OF;}
"integer"|"INTEGER"                                    {yylval.type = 1; return TYPE;}
"real"|"REAL"                                          {yylval.type = 2; return TYPE;}
"char"|"CHAR"                                          {yylval.type = 3; return TYPE;}
"string"|"STRING"                                      {yylval.type = 4; return TYPE;}
"boolean"|"BOOLEAN"                                    {yylval.type = 5; return TYPE;}
"if"|"IF"                                              {return IF;}
"then"|"THEN"                                          {return THEN;}
"else"|"ELSE"                                          {return ELSE;}
"while"|"WHILE"                                        {return WHILE;}
"do"|"DO"                                              {return DO;}

"not"|"NOT"                                            {return NOT;}

"true"|"TRUE"                                          {return CONST_BOOLEAN;}
"false"|"FALSE"                                        {return CONST_BOOLEAN;}
[a-zA-Z]*?_?([a-zA-Z|0-9])*                            {
                                                            yylval.identifier = lowerString(yytext); 
                                                            return IDENTIFIER;
                                                       }
{NUMBER}+{EXP}?                                        {return CONST_NUMBER;}
"'"(\n)"'"|"'"(.)"'"                                   {return CONST_CHAR;}
"\""(\n)"\""|"\""(.)"\""                               {return CONST_STRING;}

\{((.*?)|(\.*?))?\}                                    {/* Ignora comentários*/}

[ \t\n]                                                {/* Ignora espaçoes em branco*/}
.                                                      {}


%%

char *lowerString(const char* string_to_lower) {
    char *temp = strdup(string_to_lower);
    unsigned char *tptr = (unsigned char*)temp;

    while (*tptr) {
        *tptr = tolower(*tptr);
        tptr++;
    }

    return temp;
}