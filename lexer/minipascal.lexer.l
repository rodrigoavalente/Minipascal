%option noyywrap yylineno caseless

%{
    #include <ctype.h>
    #include <string.h>
    #include <assert.h>

    #include "../include/minipascal.parser.tab.h"

    int yycolumn = 1;
    char linebuffer[500];

    void warning(const char* mensage);
    char *trimWhiteSpace(const char *string);
    char *lowerString(const char* string_to_lower);

    #define YY_USER_ACTION yylloc.first_line = yylloc.last_line = yylineno; \
                           yylloc.first_column = yycolumn; yylloc.last_column = yycolumn + yyleng - 1; \
                           yycolumn += yyleng;
%}

NUMBER [0-9]
EXP ([Ee][\-\+]?[0-9]+)

%%
"+"                                                    {yylval.arithmetic = '+'; return SUM_OPERATOR;}
"-"                                                    {yylval.arithmetic = '-'; return SUM_OPERATOR;}
"or"|"OR"                                              {yylval.arithmetic = 'o'; return SUM_OPERATOR;}

"*"                                                    {yylval.arithmetic = '*'; return MULT_OPERATOR;}
"/"                                                    {yylval.arithmetic = '/'; return MULT_OPERATOR;}
"div"|"DIV"                                            {yylval.arithmetic = '%'; return MULT_OPERATOR;}
"and"|"AND"                                            {yylval.arithmetic = '.'; return MULT_OPERATOR;}

"("|")"|","|";"|"."|":"|"{"|"}"                        {return yytext[0];}



">"                                                    {    yylval.relational = 1;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<"                                                    {    yylval.relational = 2;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<>"                                                   {    yylval.relational = 3;
                                                            return RELATIONAL_OPERATOR;
                                                       }
"<="                                                   {    yylval.relational = 4;
                                                            return RELATIONAL_OPERATOR;
                                                       }
">="                                                   {    yylval.relational = 5;
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
{NUMBER}+{EXP}?                                        {
                                                          yylval.const_number = atof(yytext);
                                                          return CONST_NUMBER;
                                                       }
\'[^\"\n]\'                                            {
                                                          yylval.const_char = yytext;
                                                          return CONST_CHAR;
                                                       }
\'[^\"\n]$                                             {
                                                          warning("[WARNING] Untermined char.");
                                                          yylval.const_char = yytext;
                                                          return CONST_CHAR;
                                                       }
\"[^\"\n]*\"                                           {
                                                          yylval.const_string = yytext;
                                                          return CONST_STRING;
                                                       }
\"[^\"\n]*$                                            {
                                                          warning("[WARNING] Untermined string.");
                                                          yylval.const_string = yytext;
                                                          return CONST_STRING;
                                                       }

\{((.*?)|(\.*?))?\}                                    {/* Ignora comentários*/}

[ \t]                                                  {/* Ignora espaçoes em branco*/}
\n                                                     {yycolumn = 1;}
\n.*                                                   {
                                                          strcpy(linebuffer, trimWhiteSpace(yytext + 1));
                                                          yyless(1);
                                                       }
.                                                      {}


%%

void warning(const char *message) {
  printf("%s\n", message);
}

char *trimWhiteSpace(const char *string) {
  char *aux = strdup(string);

  while(isspace(*aux)) {
    aux++;
  }

  return aux;
}

char *lowerString(const char *string_to_lower) {
    char *temp = strdup(string_to_lower);
    unsigned char *tptr = (unsigned char*)temp;

    while (*tptr) {
        *tptr = tolower(*tptr);
        tptr++;
    }

    return temp;
}
