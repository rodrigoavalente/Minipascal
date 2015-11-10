SRC_DIR			:= src
INCLUDE_DIR     := include/
INCLUDE			:= $(addprefix -I, $(INCLUDE_DIR))
LEXER			:= src/minipascal.lexer.c
PARSER 			:= include/minipascal.parser.tab.h src/minipascal.parser.tab.c

all: minipascal

$(PARSER): parser/minipascal.parser.y
	bison -d --graph=graph/parser/minipascal.parser.dot \
		   -r all --report-file=parser/minipascal.parser.output parser/minipascal.parser.y
	mv minipascal.parser.tab.h include/
	mv minipascal.parser.tab.c src/

$(LEXER): lexer/minipascal.lexer.l $(PARSER)
	flex -o src/minipascal.lexer.c lexer/minipascal.lexer.l

minipascal_graph: graph/parser/minipascal.parser.dot
	dot -Tsvg grap/parser/minipascal.parser.dot -o graph/parser/minipascal.parser.svg

minipascal_release: $(LEXER) $(PARSER)
	gcc $(INCLUDE) -w src/minipascal.parser.tab.c src/minipascal.lexer.c src/minipascal.ast.c -lfl -o bin/release/minipascal >& bin/buildlog.txt

minipascal_debug: $(LEXER) $(PARSER)
	gcc $(INCLUDE) -w -ggdb src/minipascal.parser.tab.c src/minipascal.lexer.c src/minipascal.ast.c -lfl -o bin/debug/minipascal >& bin/buildlog.txt

clean:
	rm $(LEXER) $(PARSER) bin/buildlog.txt bin/minipascal parser/minipascal.parser.output graph/parser/minipascal.ast.dot graph/parser/minipascal.parser.svg
