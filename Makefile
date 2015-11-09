SRC_DIR			:= src
INCLUDE_DIR     := include/
SRC 			:= $(foreach sdir, $(SRC_DIR), $(wildcard $(sdir)/*.c))
INCLUDE			:= $(addprefix -I, $(INCLUDE_DIR))
LEXER			:= src/minipascal.lexer.c
PARSER 			:= include/minipascal.parser.tab.h src/minipascal.parser.tab.c

all: minipascal

$(PARSER): parser/minipascal.parser.y
	bison -d --graph=graph/parser/minipascal.parser.dot \
		   -r all --report-file=parser/minipascal.parser.output parser/minipascal.parser.y
	@mv minipascal.parser.tab.h include/
	@mv minipascal.parser.tab.c src/

$(LEXER): lexer/minipascal.lexer.l $(PARSER)
	@flex -o src/minipascal.lexer.c lexer/minipascal.lexer.l

minipascal_graph: graph/parser/minipascal.parser.dot
	@dot -Tsvg grap/parser/minipascal.parser.dot -o graph/parser/minipascal.parser.svg

minipascal: $(LEXER)
	gcc $(INCLUDE) -w $(SRC) -lfl -o bin/minipascal >& bin/buildlog.txt

clean:
	@rm $(LEXER) $(PARSER) bin/buildlog.txt bin/minipascal parser/minipascal.parser.output graph/parser/minipascal.ast.dot graph/parser/minipascal.parser.svg
