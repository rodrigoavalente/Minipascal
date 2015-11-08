/* A Bison parser, made by GNU Bison 3.0.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2013 Free Software Foundation, Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

#ifndef YY_YY_MINIPASCAL_PARSER_TAB_H_INCLUDED
# define YY_YY_MINIPASCAL_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PROGRAM = 258,
    B3GIN = 259,
    END = 260,
    VAR = 261,
    PROCEDURE = 262,
    ARRAY = 263,
    OF = 264,
    RETURN = 265,
    DIV = 266,
    OR = 267,
    NOT = 268,
    AND = 269,
    NUMBER = 270,
    ASSIGNMENT = 271,
    IF = 272,
    WHILE = 273,
    DO = 274,
    CONST_CHAR = 275,
    CONST_STRING = 276,
    CONST_NUMBER = 277,
    CONST_BOOLEAN = 278,
    TYPE = 279,
    SUM_OPERATOR = 280,
    MULT_OPERATOR = 281,
    RELATIONAL_OPERATOR = 282,
    IDENTIFIER = 283,
    THEN = 284,
    ELSE = 285
  };
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
typedef union YYSTYPE YYSTYPE;
union YYSTYPE
{
#line 14 "minipascal.parser.y" /* yacc.c:1909  */

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

#line 108 "minipascal.parser.tab.h" /* yacc.c:1909  */
};
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif

/* Location type.  */
#if ! defined YYLTYPE && ! defined YYLTYPE_IS_DECLARED
typedef struct YYLTYPE YYLTYPE;
struct YYLTYPE
{
  int first_line;
  int first_column;
  int last_line;
  int last_column;
};
# define YYLTYPE_IS_DECLARED 1
# define YYLTYPE_IS_TRIVIAL 1
#endif


extern YYSTYPE yylval;
extern YYLTYPE yylloc;
int yyparse (void);

#endif /* !YY_YY_MINIPASCAL_PARSER_TAB_H_INCLUDED  */
