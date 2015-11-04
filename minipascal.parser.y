%define parse.lac full

%{
    #include <stdio.h>
    #include <stdlib.h>

    #include "minipascal.ast.h"
%}

%union {
    // Definição dos tipos dos operandos
    int relational;
    int arithmetic;

    // Definição dos tipos dos tokens
    char *identifier;

    // Definição dos tipos literais
    int const_boolean;
    char *string_const;
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
%token IF THEN ELSE WHILE DO
%token <const_number> CONST_NUMBER
%token CONST_CHAR CONST_STRING CONST_BOOLEAN

%token <type> TYPE
%token <arithmetic> SUM_OPERATOR MULT_OPERATOR
%token <relational> RELATIONAL_OPERATOR

%token <symbol> IDENTIFIER

%type <symlist> identifier_list
%type <ast> expression factor term assignment statement statement_list statement_part program_block variable_declaration



%%
    pascal:
            PROGRAM IDENTIFIER ';' program_block '.'                    {eval($4);}
        |   pascal error                                                {yyerrok;}
        ;

    program_block:
            variable_declaration                                        
            statement_part                                              {
                                                                            if (!$1) {
                                                                                $$ = $2;
                                                                            } else {
                                                                                $$ = newAST('R', $1, $2);
                                                                            }
                                                                        }                                           
        ;

    variable_declaration:
            %empty                                                      {$$ = NULL;}
        |   VAR identifier_list ':' TYPE  ';' variable_declaration      {
                                                                            doDeclaration($2, $4);
                                                                            $$ = newAST('D', $2, NULL);
                                                                        }        
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
                                                                                $$ = $1;
                                                                            } else {
                                                                                $$ = newAST('L', $1, $3);
                                                                            }
                                                                        }
        ;

    statement:
            %empty                                                      {$$ = NULL;}
        |   assignment                                                  {$$ = $1;}
        ;

    assignment:
            IDENTIFIER ASSIGNMENT expression                            {$$ = newAssignment($1, $3);}
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
        |   CONST_NUMBER                                                {$$ = newNumber($1);}
        |   '(' expression ')'                                          {$$ = $2;}
        ;
%%