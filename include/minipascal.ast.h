#ifndef MINIPASCAL_AST_H
#define MINIPASCAL_AST_H

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

// Interface para o flex e bison.

int error_count;
int warning_count;

extern FILE *yyin;
extern int yylineno;
extern char *yytext;
extern char linebuffer[500];

extern int yylex();
extern int yyparse();
void yyerror(const char *message, ...);

struct YYLTYPE;

// Definição dos tipos


typedef struct symbol {
    char *name;
    int type;
    struct ast *value;
} Symbol;

// Tabela simples de símbolos de tamanho fixo.
#define NHASH 9997
Symbol symbolTable[NHASH];
Symbol *lookup(char *symbol);

static unsigned symbolHash(char *symbol);

typedef struct symlist {
    Symbol *symbol;
    struct symlist *next;
} SymList;

void symlistFree(SymList *symlist);
void newSymbol(SymList *symlist, int type);
SymList *newSymList(const char *name, SymList *next);

/*
    Tipos de nós:
        N: referência a símbolo
        L: expressão
        B: constante booleana
        Z: constante inteira
        R: constante real
        C: caracter
        S: cadeia de caracteres
        '=': Atribuição
        I: if
        W: while
 */

typedef enum builtin {
    builtin_write = 1,
    builtin_writeln,
    builtin_read
} BuiltIn;

typedef struct ast {
    int nodetype;
    struct ast *lhs;
    struct ast *rhs;
} AST;

typedef struct integer {
    int nodetype;
    int value;
} Integer;

typedef struct real {
    int nodetype;
    double value;
} Real;

typedef struct boolean {
    int nodetype;
    int value;
} Boolean;

typedef struct character {
    int nodetype;
    char value;
} Character;

// String
typedef struct word {
    int nodetype;
    char *value;
} Word;

typedef struct flow {
    int nodetype;
    AST *condition;
    AST *then_part;
    AST *else_part;
} Flow;

typedef struct symref {
    int nodetype;
    Symbol *symbol
} SymRef;

typedef struct symassgn {
    int nodetype;
    Symbol *symbol;
    AST *value;
} SymAssignment;

typedef struct function_call {
    int nodetype;
    AST *arguments;
    BuiltIn function_type;
} FunctionCall;

// Constrói uma AST
AST *newAST(int nodetype, AST *lhs, AST *rhs);
AST *newReference(Symbol *symbol);
AST *newAssignment(Symbol *symbol, AST *value);
AST *newInteger(int value);
AST *newReal(double value);
AST *newBoolean(int value);
AST *newCharacter(char value);
AST *newWord(const char *value);
AST *newComparison(int comparison_type, AST *lhs, AST *rhs);
AST *newFlow(int nodetype, AST *condition, AST *then_part, AST *else_part);
AST *newFunction(int function_type, AST *arguments);


// Avalia uma AST
int booleanEval(AST *ast);
int integerEval(AST *ast);
double realEval(AST *ast);
char characterEval(AST *ast);
char *wordEval(AST *ast);
AST *evalExpression(AST *ast);

// Libera uma árvore
void treefree(AST *ast);

#endif // MINIPASCAL_AST_H
