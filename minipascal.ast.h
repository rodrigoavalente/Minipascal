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

extern int yylex();
extern int yyparse();
void yyerror(const char *message, ...);

// Definição dos tipos


typedef struct symbol {
    char *name;
    int type;
    void *typePointer;
} Symbol;

Symbol *newSymbol(const char *name, int type);

// Tabela simples de símbolos de tamanho fixo.
#define NHASH 9997
Symbol symbolTable[NHASH];
Symbol *lookup(char *symbol);

static unsigned symbolHash(char *symbol);

typedef struct symlist {
    Symbol *symbol;
    struct symlist *next;
} SymList;

SymList *newSymList(Symbol *symbol, SymList *next);
void symlistFree(SymList *symlist);

/*
    Tipos de nós:
        N: referência a símbolo
        L: expressão
        B: constante booleana
        K: constante numérica
        C: caracter
        S: cadeia de caracteres
        D: declaração de varíaveis
        R: root
        '=': Atribuição
 */

typedef struct ast {
    int nodetype;
    struct ast *lhs;
    struct ast *rhs;
} AST;

typedef struct number {
    int nodetype;
    double value;
} Number;

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

typedef struct symref {
    int nodetype;
    Symbol *symbol
} SymRef;

typedef struct symassgn {
    int nodetype;
    Symbol *symbol;
    AST *value;
} SymAssignment;

// Constrói uma AST
AST *newAST(int nodetype, AST *lhs, AST *rhs);
AST *newReference(Symbol *symbol);
AST *newAssignment(Symbol *symbol, AST *value);
AST *newNumber(double value);
AST *newBoolean(int value);
AST *newCharacter(char value);
AST *newWord(char *value);

// Declara variavéis
void doDeclaration(const char *name, int type);

// Avalia uma AST
void eval(AST *ast);

// Libera uma árvore
void treefree(AST *ast);

#endif // MINIPASCAL_AST_H