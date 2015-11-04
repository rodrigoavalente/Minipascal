#include "minipascal.ast.h"

static unsigned symbolHash(char *symbol) {
    unsigned c, hash = 0;

    while (c = *symbol++) {
        hash = hash * 9 ^ c;
    }

    return hash;
}

Symbol *lookup(char *symbol) {
    Symbol *symbolPointer = &symbolTable[symbolHash(symbol)%NHASH];
    int symbolCount = NHASH;

    while (--symbolCount >= 0) {
        if (symbolPointer->name && !strcmp(symbolPointer->name, symbol)) {
            yyerror("[ERROR] Redifining %s!", symbol);
            exit(0);
        }

        if (!symbolPointer->name) {
            symbolPointer->name = strdup(symbol);
            symbolPointer->type = -1;
            symbolPointer->typePointer = NULL;
            return symbolPointer;
        }

        if (++symbolPointer >= symbolTable + NHASH) {
            symbolPointer = symbolTable;
        }
    }

    yyerror("[ERROR] Symbol table overflow!\n");
    abort();
}

AST *newAST(int nodetype, AST *lhs, AST *rhs) {
    AST *ast = malloc(sizeof(AST));

    if (!ast) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    ast->nodetype = nodetype;
    ast->lhs = lhs;
    ast->rhs = rhs;

    return ast;
}

AST *newNumber(double value) {
    Number *number = malloc(sizeof(Number));

    if (!number) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    number->nodetype = 'K';
    number->value = value;

    return (AST*)number;
}

AST *newBoolean(int value) {
    Boolean *boolean = malloc(sizeof(Boolean));

    if (!boolean) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    boolean->nodetype = 'B';
    boolean->value = value;

    return (AST*)boolean;
}

AST *newCharacter(char value) {
    Character *character = malloc(sizeof(Character));

    if (!character) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    character->nodetype = 'C';
    character->value = value;

    return (AST*)character;
}

AST *newWord(char *value) {
    Word *word = malloc(sizeof(Word));

    if (!word) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    word->nodetype = 'S';
    word->value = value;

    return (AST*)word;
}

AST *newRef(Symbol *symbol) {
    SymRef *symref = malloc(sizeof(SymRef));

    if (!symref) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symref->nodetype = 'N';
    symref->symbol = symbol;

    return (AST*)symref;
}

AST *newAssignment(Symbol *symbol, AST *value) {
    SymAssignment *symassign = malloc(sizeof(SymAssignment));

    if (!symassign) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symassign->nodetype = '=';
    symassign->symbol = symbol;
    symassign->value = value;

    return (AST*)symassign;
}

void treefree(AST *ast) {
    switch (ast->nodetype) {
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
        case '10':
        case '11':
        case '12':
            treefree(ast->rhs);

        case 'K':
        case 'B':
        case 'C':
        case 'S':
            break;

        case '=':
            free(((SymAssignment*)ast)->value);
            break;
    }
}

AST *newReference(Symbol *symbol) {
    SymRef *symref = malloc(sizeof(SymRef));

    if (!symref) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symref->nodetype = 'N';
    symref->symbol = symbol;

    return (AST*)symbol;
}

Symbol *newSymbol(const char *name, int type) {
    Symbol *symbol = malloc(sizeof(Symbol));

    if (!symbol) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symbol->name = name;
    symbol->type = type;

    return symbol;
};

SymList *newSymList(Symbol *symbol, SymList *next) {
    SymList *symlist = malloc(sizeof(SymList));

    if (!symlist) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symlist->symbol = symbol;
    symlist->next = next;

    return symlist;
}

void doDeclaration(const char *name, int type) {
    Symbol *symbol = lookup(name);

    if (!symbol) {
        symbol->type = type;

        switch (type) {
            case 1:
            case 2:
                ((Number*)symbol->typePointer);
                break;
            case 3:
                ((Character*)symbol->typePointer);
                break;
            case 4:
                ((Word*)symbol->typePointer);
                break;
            case 5:
                ((Boolean*)symbol->typePointer);
                break;
            default:
                yyerror("[ERROR] Unknown given type.");
                break;
        }
    }
}

void symlistFree(SymList *symlist) {
    SymList *aux;

    while (symlist) {
        aux = symlist->next;
        free(symlist);
        symlist = aux;
    }
}

void eval(AST *ast) {
    // static FILE *graph;

    
    // graph = fopen("minipascal.ast.dot", "w");
    // fputs("// Minipascal AST\n", graph);
    // fputs("rankdir=LR\n", graph);
    // fputs("digraph AST {", graph);

    // fputs("\t", graph);
    // fputs(ast->nodetype, graph);
    
    if (ast) {
        printf("Existe ast. Nó: %c\n", ast->nodetype);
        eval(ast->lhs);
        eval(ast->rhs);
    } else {
        printf("Não existe ast.\n");
    }
}

void yyerror(const char *message, ...) {
    va_list arguments;
    va_start(arguments, message);

    fprintf(stderr, "Line: %d. ", yylineno);
    vfprintf(stderr, message, arguments);
    fprintf(stderr, "\n");
}

int main(int argc, char** argv) {
    if (argc < 2) {
        printf("Usage: minipascal <file path>\n");
    } else {
        yyin = fopen(argv[argc - 1], "r");

        if (yyin) {
            do {
                yyparse();
            } while (!feof(yyin));    
        } else {
            printf("[ERROR] Failed to open file!\n");
        }        
    }
}