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
            yyerror("[ERROR] Redifining \"%s\"!", symbol);
            exit(0);
        }

        if (!symbolPointer->name) {
            symbolPointer->name = strdup(symbol);
            symbolPointer->type = 0;
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

Symbol *getSymbol(char *symbol_name) {
    Symbol *symbol = &symbolTable[symbolHash(symbol_name)%NHASH];

    if (symbol->name && !strcmp(symbol->name, symbol_name)) {
		return symbol;
    } else {
		return NULL;
    }
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

    return (AST*)symref;
}

void newSymbol(SymList *symlist, int type) {
    symlist->symbol->type = type;

    if (symlist->next) {
        newSymbol(symlist->next, type);
    }

    int aux = symlist->symbol->type;
};

SymList *newSymList(const char *name, SymList *next) {
    Symbol *symbol = lookup(name);
    SymList *symlist = malloc(sizeof(SymList));


    if (!symlist) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    symlist->symbol = symbol;
    symlist->next = next;

    return symlist;
}

AST *newComparison(int comparison_type, AST *lhs, AST *rhs) {
    AST *ast = malloc(sizeof(ast));

    if (!ast) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    ast->nodetype = '0' + comparison_type;
    ast->lhs = lhs;
    ast->rhs = rhs;

    return ast;
}

AST *newFlow(int nodetype, AST *condition, AST *then_part, AST *else_part) {
    Flow *flow = malloc(sizeof(Flow));

    if (!flow) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    flow->nodetype = nodetype;
    flow->condition = condition;
    flow->then_part = then_part;
    flow->else_part = else_part;

    return (AST*)flow;
}

void symlistFree(SymList *symlist) {
    SymList *aux;

    while (symlist) {
        aux = symlist->next;
        free(symlist);
        symlist = aux;
    }
}

void graphEval(AST *ast) {
//    FILE *dot = fopen("ast.gv", "w");
//
//    if(dot) {
//        int size;
//        char start[69] = "digraph AST {\n\tnode [fontname = \"Arial\"];\n\tgraph [ordering = \"out\"];\n";
//
//        fwrite(start, 1, 69, dot);
//        if (!ast) {
//            fwrite("\n", sizeof(char), 1, dot);
//        } else if (!ast->lhs && !ast->rhs) {
//            int size = printf("%c;\n", ast->nodetype);
//            char aux[size];
//            snprintf(aux, sizeof(aux), "%c;\n", ast->nodetype);
//            fwrite(aux, sizeof(char), sizeof(aux), dot);
//        } else {
//            auxEval(ast, dot);
//        }
//
//        char end[2] = "}""\n";
//        fwrite(end, 1, 2, dot);
//        fclose(dot);
//        dot = NULL;
//
//        system("dot ast.gv | gvpr -c -ftree.gv | neato -n -Tsvg -o ast.svg");
//    } else {
//        fprintf(stderr, "[ERROR] Failed to create file!");
//    }

    if (ast) {
        printf("Nó: %c.\n", ast->nodetype);
        if (ast->lhs) {
            graphEval(ast->lhs);
        }
        if (ast->rhs) {
            graphEval(ast->rhs);
        }
    }

}

void auxEval(AST *ast, FILE *file) {
    FILE *dot = file;

    if (ast->lhs) {
        int size;
        char buffer[50];

        size = snprintf(buffer, 50, "\t\"%c\" -> \"%c\";\n", ast->nodetype, ast->lhs->nodetype);

        char temp[size];
        strcpy(temp, buffer);

        fwrite(temp, 1, size, dot);
        auxEval(ast->lhs, dot);
    }

    if (ast->rhs) {
        int size;
        char buffer[50];

        size = snprintf(buffer, 50, "\t\"%c\" -> \"%c\";\n", ast->nodetype, ast->rhs->nodetype);

        char temp[size];
        strcpy(temp, buffer);

        fwrite(temp, 1, size, dot);
        auxEval(ast->rhs, dot);
    }
}

int main(int argc, char **argv) {
    if (argc < 1) {
        printf("Usage: minipascal <file path>\n");
    } else {
        yyin = fopen("test files/test.pas", "r");

        if (yyin) {
            do {
                yyparse();
            } while (!feof(yyin));
        } else {
            printf("[ERROR] Failed to open file!\n");
        }
    }
}
