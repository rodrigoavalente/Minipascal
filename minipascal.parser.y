%define parse.lac full

%locations

%{
    #include <stdio.h>
    #include <stdlib.h>

    #include "minipascal.ast.h"

    extern char linebuffer[500];
%}

%union {
    // Definição dos tipos dos operandos
    int relational;
    char arithmetic;

    // Definição dos tipos dos tokens
    char *identifier;

    // Definição dos tipos literais
    int const_boolean;
    char const_char;
    char *const_string;
    double const_number;

    // Definição de tipos
    int type;

    // Tipos abstratos
    struct ast *ast;
    struct symbol *symbol;
    struct symlist *symlist;
}

%token PROGRAM B3GIN END
%token VAR PROCEDURE ARRAY OF RETURN
%token DIV OR NOT AND NUMBER ASSIGNMENT
%token IF WHILE DO
%token <const_char> CONST_CHAR
%token <const_string> CONST_STRING
%token <const_number> CONST_NUMBER
%token <const_boolean>  CONST_BOOLEAN

%token <type> TYPE
%token <arithmetic> SUM_OPERATOR MULT_OPERATOR
%token <relational> RELATIONAL_OPERATOR

%token <symbol> IDENTIFIER

%right THEN ELSE

%type <symlist> identifier_list
%type <ast> expression factor term assignment statement statement_list statement_part program_block strutured_statement



%%
    pascal:
            PROGRAM IDENTIFIER ';' program_block '.'                    {graphEval($4);}
        |   pascal error                                                {yyerrok;}
        ;

    program_block:
            variable_declaration                                        
            statement_part                                              {$$ = $2;}                                           
        ;

    variable_declaration:
            %empty                                                      
        |   VAR identifier_list ':' TYPE  ';' variable_declaration      {newSymbol($2, $4);}        
        ;

    identifier_list:
            IDENTIFIER                                                  {$$ = $1;}
        |   IDENTIFIER ',' identifier_list                              {$$ = newSymList($1,$3);}
        ;

    statement_part:
            B3GIN statement_list END                                    {$$ = $2;}
        ;

    statement_list:
            %empty                                                      {$$ = NULL;}
            |   statement ';' statement_list                            {
                                                                            if ($3 == NULL) {
                                                                                $$ = newAST('L',$1, NULL);
                                                                            } else {
                                                                                $$ = newAST('L', $1, $3);
                                                                            }
                                                                        }
        ;

    statement:
            %empty                                                      {$$ = NULL;}
        |   assignment                                                  {$$ = $1;}
        |   strutured_statement                                         {$$ = $1;}
        ;

    strutured_statement:
            statement_part                                              {$$ = $1;}
        |   IF expression THEN statement                                {$$ = newFlow('I', $2, $4, NULL);}
        |   IF expression THEN statement ELSE statement                 {$$ = newFlow('I', $2, $4, $6);}
        |   WHILE expression DO statement                               {$$ = newFlow('W', $2, $4, NULL);}
        ;

    assignment:
            IDENTIFIER ASSIGNMENT expression                            {
                                                                            Symbol *symbol = getSymbol($1);

                                                                            if ($1 == NULL) {
                                                                                int size;
                                                                                char buffer[50];

                                                                                size = snprintf(buffer, 30, "Undeclared identifier \"%s\"", $1);
                                                                                char temp[size];
                                                                                strcpy(temp, buffer);

                                                                                lyyerror(@1, temp);
                                                                            } else {
                                                                                $$ = newAssignment(symbol, $3);    
                                                                            }                                                                            
                                                                        }
        ;

    expression:
            factor                                                      {$$ = $1;}
        |   expression SUM_OPERATOR factor                              {$$ = newAST($2, $1, $3);}
        |   expression RELATIONAL_OPERATOR factor                       {$$ = newAST($2, $1, $3);}
        ;

    factor:
            term                                                        {$$ = $1;}
        |   factor MULT_OPERATOR term                                   {$$ = newAST($2, $1, $3);}
        ;

    term:
            IDENTIFIER                                                  {$$ = newReference($1);}
        |   CONST_CHAR                                                  {$$ = newCharacter($1);}
        |   CONST_STRING                                                {$$ = newWord($1);}
        |   CONST_NUMBER                                                {$$ = newNumber($1);}
        |   CONST_BOOLEAN                                               {$$ = newBoolean($1);}
        |   '(' expression ')'                                          {$$ = $2;}
        ;
%%

void yyerror(const char *message, ...) {
    int size;
    char buffer[100];

    size = snprintf(buffer, 100, "%s at \"%s\" in line: \"%s\"", message, yytext, linebuffer);

    char aux[size];
    strcpy(aux, buffer);


    va_list arguments;
    va_start(arguments, aux);

    if (yylloc.first_line) {
        fprintf(stderr, "[ERROR] From: %d.%d To: %d.%d: ", yylloc.first_line, yylloc.first_column, yylloc.last_line, yylloc.last_column);
        vfprintf(stderr, aux, arguments);
        fprintf(stderr, "\n");
    }
}

void lyyerror(YYLTYPE type, char *message, ...) {
    int size;
    char buffer[100];

    size = snprintf(buffer, 100, "%s at \"%s\" in line: \"%s\"", message, yytext, linebuffer);

    char aux[size];
    strcpy(aux, buffer);


    va_list arguments;
    va_start(arguments, aux);

    if (type.first_line) {
        fprintf(stderr, "[ERROR] From: %d.%d To: %d.%d: ", type.first_line, type.first_column, type.last_line, type.last_column);
        vfprintf(stderr, aux, arguments);
        fprintf(stderr, "\n");
    }
}