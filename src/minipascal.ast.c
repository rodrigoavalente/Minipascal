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
            symbolPointer->value = NULL;
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

AST *newInteger(int value) {
    Integer *integer = malloc(sizeof(Integer));

    if (!integer) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    integer->nodetype = 'Z';
    integer->value = value;

    AST *ast = (AST*)integer;
    ast->lhs = NULL;
    ast->rhs = NULL;

    return ast;
}

AST *newReal(double value) {
	Real *real = malloc(sizeof(Real));

	if (!real) {
		yyerror("[ERROR] Not Enough space!");
		exit(0);
	}

	real->nodetype = 'R';
	real->value = value;

	AST *ast = (AST*)real;
	ast->lhs = NULL;
	ast->rhs = NULL;

	return ast;
}

AST *newBoolean(int value) {
    Boolean *boolean = malloc(sizeof(Boolean));

    if (!boolean) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    boolean->nodetype = 'B';
    boolean->value = value;

    AST *ast = (AST*)boolean;
    ast->lhs = NULL;
    ast->rhs = NULL;

    return ast;
}

AST *newCharacter(char value) {
    Character *character = malloc(sizeof(Character));

    if (!character) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    character->nodetype = 'C';
    character->value = value;

    AST *ast = (AST*)character;
    ast->lhs = NULL;
    ast->rhs = NULL;

    return ast;
}

AST *newWord(char *value) {
    Word *word = malloc(sizeof(Word));

    if (!word) {
        yyerror("[ERROR] Not enough space!");
        exit(0);
    }

    word->nodetype = 'S';
    word->value = value;

    AST *ast = (AST*)word;
    ast->lhs = NULL;
    ast->rhs = NULL;

    return ast;
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
//    FILE *dot = fopen("graph/ast/ast.gv", "w");
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
//        system("dot graph/ast/ast.gv | gvpr -c -fgraph/ast/tree.gv | neato -n -Tsvg -o graph/ast/output/ast.svg");
//    } else {
//        fprintf(stderr, "[ERROR] Failed to create file!");
//    }

    if (ast) {
		char no = ast->nodetype;
        printf("Nó: %c.\n", ast->nodetype);

        AST *lhs = ast->lhs;
        AST *rhs = ast->rhs;

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

AST *eval(AST *ast) {
	if (!ast) {
		yyerror("[ERROR] Internal error! Null evaluation.");
		return NULL;
	}

    char node = ast->nodetype;
    AST *evaluatedAST;

    if (node == 'L') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

		eval(lhs);
		evaluatedAST = eval(rhs);
	} else if (node == 'Z' || node == 'R') {
		evaluatedAST = ast;
	} else if (node == 'C' || node == 'S') {
		evaluatedAST = ast;
	} else if (node == 'B') {
		evaluatedAST = ast;
	} else if (node == 'N') {
		SymRef *symref = (SymRef*)ast;
		Symbol *symbol = symref->symbol;

		evaluatedAST = eval(symbol->value);
    } else if (node == '+') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype != 'Z' || lhs->nodetype != 'R' || lhs->nodetype != 'C' || lhs->nodetype != 'S' || lhs->nodetype != 'B') {
				lhs = eval(lhs);
			}

			if (rhs->nodetype != 'Z' || rhs->nodetype != 'R' || rhs->nodetype != 'C' || rhs->nodetype != 'S' || rhs->nodetype != 'B') {
				rhs = eval(rhs);
			}

			char aux1 = lhs->nodetype, aux2 = rhs->nodetype;

			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newInteger(temp1 + temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

				temp1 = realEval(lhs);
				temp2 = realEval(rhs);

				evaluatedAST = newReal(temp1 + temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 + (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '+' for types 'integer' and 'character'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '+' for types 'integer' and 'string'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newInteger(temp1 + temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				int temp1;
				double temp2;

				temp1 = realEval(rhs);
				temp2 = integerEval(lhs);

                evaluatedAST = newReal(temp1 + temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				int temp1;
				double temp2;

                temp1 = booleanEval(rhs);
                temp2 = realEval(lhs);

                evaluatedAST = newReal(temp1 + temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '+' for types 'real' and 'character'.");
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '+' for types 'real' and 'string'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				yyerror("[ERROR] Invalid operator '+' for types 'character' and 'integer'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				yyerror("[ERROR] Invalid operator '+' for types 'character' and 'real'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '+' for types 'character' and 'character'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '+' for types 'character' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                int size;
                char *temp1, *temp2;
                char buffer[500];

                temp1 = wordEval(lhs);
                temp2 = wordEval(rhs);

                size = snprintf(buffer, 500, "%s%s", temp1, temp2);
                char temp[size];
                strcpy(temp, buffer);

                evaluatedAST = newWord(temp);
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				int size;
                char *temp1;
                char temp2, buffer[100];

                temp1 = wordEval(lhs);
                temp2 = characterEval(rhs);

                size = snprintf(buffer, 500, "%s%c", temp1, temp2);
                char temp[size];
                strcpy(temp, buffer);

                evaluatedAST = newWord(temp);
			}
        }
    } else if (node == '-') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newInteger(temp1 - temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

				temp1 = realEval(lhs);
				temp2 = realEval(rhs);

				evaluatedAST = newReal(temp1 - temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 - (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 + (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '-' for types 'integer' and 'character'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '-' for types 'integer' and 'string'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newInteger(temp1 - temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				int temp1;
				double temp2;

				temp1 = realEval(rhs);
				temp2 = integerEval(lhs);

                evaluatedAST = newReal(temp1 - temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				int temp1;
				double temp2;

                temp1 = booleanEval(rhs);
                temp2 = realEval(lhs);

                evaluatedAST = newReal(temp1 - temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '-' for types 'real' and 'character'.");
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '-' for types 'real' and 'string'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				yyerror("[ERROR] Invalid operator '-' for types 'character' and 'integer'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				yyerror("[ERROR] Invalid operator '-' for types 'character' and 'real'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '-' for types 'character' and 'character'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '-' for types 'character' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                yyerror("[ERROR] Invalid operator '-' for types 'string' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '-' for types 'string' and 'character'.");
			}
        }
    } else if (node == '*') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newInteger(temp1 * temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

				temp1 = realEval(lhs);
				temp2 = realEval(rhs);

				evaluatedAST = newReal(temp1 * temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 + (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '*' for types 'integer' and 'character'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '*' for types 'integer' and 'string'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newInteger(temp1 * temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				int temp1;
				double temp2;

				temp1 = realEval(lhs);
				temp2 = integerEval(rhs);

                evaluatedAST = newReal(temp1 * temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				int temp1;
				double temp2;

                temp1 = booleanEval(rhs);
                temp2 = realEval(lhs);

                evaluatedAST = newReal(temp1 * temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '*' for types 'real' and 'character'.");
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '*' for types 'real' and 'string'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				yyerror("[ERROR] Invalid operator '*' for types 'character' and 'integer'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				yyerror("[ERROR] Invalid operator '*' for types 'character' and 'real'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '*' for types 'character' and 'character'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '*' for types 'character' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                yyerror("[ERROR] Invalid operator '*' for types 'string' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '*' for types 'string' and 'character'.");
			}
		}
	} else if (node == '/') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newInteger(temp1 / temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

				temp1 = realEval(lhs);
				temp2 = realEval(rhs);

				evaluatedAST = newReal(temp1 / temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 + (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '/' for types 'integer' and 'character'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '/' for types 'integer' and 'string'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newInteger(temp1 / temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				int temp1;
				double temp2;

				temp1 = realEval(rhs);
				temp2 = integerEval(lhs);

                evaluatedAST = newReal(temp1 / temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				int temp1;
				double temp2;

                temp1 = booleanEval(rhs);
                temp2 = realEval(lhs);

                evaluatedAST = newReal(temp1 / temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '/' for types 'real' and 'character'.");
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '/' for types 'real' and 'string'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				yyerror("[ERROR] Invalid operator '/' for types 'character' and 'integer'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				yyerror("[ERROR] Invalid operator '/' for types 'character' and 'real'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '/' for types 'character' and 'character'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '/' for types 'character' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                yyerror("[ERROR] Invalid operator '/' for types 'string' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '/' for types 'string' and 'character'.");
			}
		}
	} else if (node == '%') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newInteger(temp1 % temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

				temp1 = realEval(lhs);
				temp2 = realEval(rhs);

				evaluatedAST = newReal((int)temp1 % (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newInteger(temp1 + (int)temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '%' for types 'integer' and 'character'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '%' for types 'integer' and 'string'.");
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newInteger(temp1 % temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				int temp1;
				double temp2;

				temp2 = realEval(lhs);
				temp1 = integerEval(rhs);

                evaluatedAST = newReal((int)temp2 % temp1);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				int temp1;
				double temp2;

                temp1 = booleanEval(rhs);
                temp2 = realEval(lhs);

                evaluatedAST = newReal(temp1 % (int)temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '%' for types 'real' and 'character'.");
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '%' for types 'real' and 'string'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				yyerror("[ERROR] Invalid operator '%' for types 'character' and 'integer'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				yyerror("[ERROR] Invalid operator '%' for types 'character' and 'real'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '%' for types 'character' and 'character'.");
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				yyerror("[ERROR] Invalid operator '%' for types 'character' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                yyerror("[ERROR] Invalid operator '%' for types 'string' and 'string'.");
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				yyerror("[ERROR] Invalid operator '%' for types 'string' and 'character'.");
			}
		}
	} else if (node == 'o') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

                temp1 = realEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				int temp1;
				char temp2;

                temp1 = integerEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				int temp1;
				char *temp2;

                temp1 = integerEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				double temp1;
				int temp2;

                temp1 = realEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				double temp1;
				int temp2;

                temp1 = realEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				double temp1;
				char temp2;

                temp1 = realEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				double temp1;
				char *temp2;

                temp1 = realEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				char temp1;
				int temp2;

                temp1 = characterEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				char temp1;
				double temp2;

                temp1 = characterEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				char temp1, temp2;

                temp1 = characterEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				char temp1;
				int temp2;

                temp1 = characterEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                char *temp1, *temp2;

                temp1 = wordEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				char *temp1;
				char temp2;

                temp1 = characterEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 || temp2);
			}
		}
	} else if (node == '.') {
		AST *lhs, *rhs;

		if (ast->lhs) {
			lhs = ast->lhs;
		}
		if (ast->rhs) {
			rhs = ast->rhs;
		}

        if (lhs && rhs) {
			if (lhs->nodetype == 'Z' && rhs->nodetype == 'Z') {
                int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'R') {
				double temp1, temp2;

                temp1 = realEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'R') {
				int temp1;
				double temp2;

                temp1 = integerEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'C') {
				int temp1;
				char temp2;

                temp1 = integerEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'S') {
				int temp1;
				char *temp2;

                temp1 = integerEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'Z' && rhs->nodetype == 'B') {
				int temp1, temp2;

                temp1 = integerEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'Z') {
				double temp1;
				int temp2;

                temp1 = realEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'B') {
				double temp1;
				int temp2;

                temp1 = realEval(lhs);
                temp2 = booleanEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'C') {
				double temp1;
				char temp2;

                temp1 = realEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'R' && rhs->nodetype == 'S') {
				double temp1;
				char *temp2;

                temp1 = realEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'Z') {
				char temp1;
				int temp2;

                temp1 = characterEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'R') {
				char temp1;
				double temp2;

                temp1 = characterEval(lhs);
                temp2 = realEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'C') {
				char temp1, temp2;

                temp1 = characterEval(lhs);
                temp2 = characterEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'C' && rhs->nodetype == 'S') {
				char temp1;
				int temp2;

                temp1 = characterEval(lhs);
                temp2 = integerEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'S') {
                char *temp1, *temp2;

                temp1 = wordEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			} else if (lhs->nodetype == 'S' && rhs->nodetype == 'C') {
				char *temp1;
				char temp2;

                temp1 = characterEval(lhs);
                temp2 = wordEval(rhs);

                evaluatedAST = newBoolean(temp1 && temp2);
			}
		}
	} else if (node == 'I') {
        AST *condition = ((Flow*)ast)->condition;
        AST *result = eval(condition);

        if (((Boolean*)result)->value) {
			AST *then_part;
			if (then_part = ((Flow*)ast)->then_part) {
				evaluatedAST = eval(then_part);
			} else {
				evaluatedAST = NULL;
			}
        } else {
			AST *else_part;
			if (else_part = ((Flow*)ast)->else_part) {
				evaluatedAST = eval(else_part);
			} else {
				evaluatedAST = NULL;
			}
        }
	} else if (node == 'W') {
        AST *then_part;
        AST *condition = ((Flow*)ast)->condition;

        if (then_part = ((Flow*)ast)->then_part) {
			while (((Boolean*)eval(condition))->value) {
				evaluatedAST = eval(then_part);
            }
        }
	} else if (node == '=') {
        Symbol *symbol = ((SymAssignment*)ast)->symbol;
        AST *value = ((SymAssignment*)ast)->value;

		evaluatedAST = eval(value);

		char *symbolType, *valueType;

		int type = symbol->type;

        switch (symbol->type) {
            case 1:
				symbolType = "integer";
				break;

			case 2:
				symbolType = "real";
				break;

			case 3:
				symbolType = "char";
				break;

			case 4:
				symbolType = "string";
				break;

			case 5:
				symbolType = "boolean";
				break;
        }

        switch (evaluatedAST->nodetype) {
			case 'Z':
				valueType = "integer";
				break;

			case 'R':
				valueType = "real";
				break;

			case 'C':
				valueType = "char";
				break;

			case 'S':
				valueType = "string";
				break;

			case 'B':
				valueType = "boolean";
				break;
        }

        if (symbolType == valueType) {
            symbol->value = evaluatedAST;
        } else {
            yyerror("[ERROR] Cannot cast types: '%s' to '%s'.", symbolType, valueType);
        }

	} else if (node == '1') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
                evaluatedAST = newBoolean(((Integer*)temp1)->value > ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value > ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value > ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) > sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '>' to types '%s' and '%s'", lhsType, rhsType);
        }
	} else if (node == '2') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
                evaluatedAST = newBoolean(((Integer*)temp1)->value < ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value < ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value < ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) < sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '<' to types '%s' and '%s'", lhsType, rhsType);
        }
	} else if (node == '3') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
                evaluatedAST = newBoolean(((Integer*)temp1)->value != ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value != ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value != ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) != sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '<>' to types '%s' and '%s'", lhsType, rhsType);
        }
	} else if (node == '4') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
                evaluatedAST = newBoolean(((Integer*)temp1)->value <= ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value <= ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value <= ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) <= sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '<=' to types '%s' and '%s'", lhsType, rhsType);
        }
	} else if (node == '5') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
                evaluatedAST = newBoolean(((Integer*)temp1)->value >= ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value >= ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value >= ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) >= sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '>=' to types '%s' and '%s'", lhsType, rhsType);
        }
	} else if (node == '6') {
        AST *temp1, *temp2;

        temp1 = eval(ast->lhs);
        temp2 = eval(ast->rhs);

        if (temp1->nodetype == temp2->nodetype) {
			char node = temp1->nodetype;

            if (node == 'Z') {
				int x = ((Integer*)temp1)->value;
				int y = ((Integer*)temp2)->value;
				int aux = x == y;
                evaluatedAST = newBoolean(((Integer*)temp1)->value == ((Integer*)temp2)->value);
            } else if (node == 'R') {
				evaluatedAST = newBoolean(((Real*)temp1)->value == ((Real*)temp2)->value);
            } else if (node == 'C') {
				evaluatedAST = newBoolean(((Character*)temp1)->value == ((Character*)temp2)->value);
            } else if (node == 'S') {
				evaluatedAST = newBoolean(sizeof(((Word*)temp1)->value) == sizeof(((Word*)temp2)->value));
            }
        } else {
			char *lhsType, *rhsType;

			switch (temp1->nodetype) {
				case 'Z':
				lhsType = "integer";
				break;

				case 'R':
					lhsType = "real";
					break;

				case 'C':
					lhsType = "char";
					break;

				case 'S':
					lhsType = "string";
					break;

				case 'B':
					lhsType = "boolean";
					break;
			}

			switch(temp2->nodetype) {
				case 'Z':
					rhsType = "integer";
					break;

				case 'R':
					rhsType = "real";
					break;

				case 'C':
					rhsType = "char";
					break;

				case 'S':
					rhsType = "string";
					break;

				case 'B':
					rhsType = "boolean";
					break;
			}

			yyerror("[ERROR]Invalid operator '==' to types '%s' and '%s'", lhsType, rhsType);
        }
	}
}

int booleanEval(AST *ast) {
	return ((Boolean*)ast)->value;
}

int integerEval(AST *ast) {
	return ((Integer*)ast)->value;
}

double realEval(AST *ast) {
	return ((Real*)ast)->value;
}

char characterEval(AST *ast) {
	return ((Character*)ast)->value;
}

char *wordEval(AST *ast) {
	return ((Word*)ast)->value;
}


int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: minipascal <file path>\n");
    } else {
        yyin = fopen(argv[1], "r");

        if (yyin) {
            do {
                yyparse();
            } while (!feof(yyin));
        } else {
            printf("[ERROR] Failed to open file!\n");
        }
    }
}
