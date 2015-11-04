SRC_DIR			:= src

SRC 			:= $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.c))
LEXER			:= minipascal.lexer.c
PARSER 			:= minipascal.parser.tab.h minipascal.parser.tab.c
AST				:= minipascal.ast.c
INCLUDES		:= -Isrc/
INCLUDES		+= $(addprefix -I, $(SRC_DIR))

all: minipascal

$(PARSER): minipascal.parser.y
	bison -d -g -r all -t minipascal.parser.y

$(LEXER): minipascal.lexer.l $(PARSER)
	flex -o minipascal.lexer.c minipascal.lexer.l

graph: minipascal.parser.dot
	dot -Tsvg minipascal.parser.dot -o graph.svg

minipascal: $(PARSER) $(LEXER)
	gcc -w $(LEXER) $(AST) minipascal.parser.tab.c -lfl -o minipascal >& buildlog.txt

clean:
	rm $(LEXER) $(PARSER) buildlog.txt minipascal minipascal.parser.output minipascal.ast.dot graph.svg
