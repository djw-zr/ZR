/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015 Free Software Foundation, Inc.

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

#ifndef YY_YY_Y_TAB_H_INCLUDED
# define YY_YY_Y_TAB_H_INCLUDED
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
    SC_PROGRAM = 258,
    SC_FUNCTION = 259,
    SC_VARIABLE = 260,
    SC_NAME = 261,
    SC_IVALUE = 262,
    SC_FVALUE = 263,
    SC_SCRIPT = 264,
    SC_EXTERN = 265,
    SC_FLOAT = 266,
    SC_IF = 267,
    SC_ELSE = 268,
    SC_FOR = 269,
    SC_WHILE = 270,
    SC_DO = 271,
    SC_BREAK = 272,
    SC_CONTINUE = 273,
    SC_RETURN = 274,
    SC_PRINT = 275,
    SC_UMINUS = 276,
    SC_SCRIPT_LIST = 277,
    SC_DEF_LIST = 278,
    SC_STMT_LIST = 279,
    SC_DEF_EFUN = 280,
    SC_DEF_EVAR = 281,
    SC_DEF_VAR = 282,
    SC_EXIT = 283,
    SC_ASSIGN = 284,
    SC_IAS = 285,
    SC_PE = 286,
    SC_ME = 287,
    SC_XE = 288,
    SC_DE = 289,
    SC_IASD = 290,
    SC_AMD = 291,
    SC_IFX = 292,
    SC_BB = 293,
    SC_NO_OP = 294,
    SC_NEG = 295,
    SC_COMMA = 296,
    SC_EXPR_LIST = 297,
    SC_AND = 298,
    SC_OR = 299,
    SC_EOR = 300,
    SC_NOT = 301,
    SC_INT = 302,
    SC_GT = 303,
    SC_LT = 304,
    SC_GE = 305,
    SC_LE = 306,
    SC_EQ = 307,
    SC_NE = 308,
    SC_IGT = 309,
    SC_ILT = 310,
    SC_IGE = 311,
    SC_ILE = 312,
    SC_IEQ = 313,
    SC_INE = 314,
    SC_PLUS = 315,
    SC_POW = 316
  };
#endif
/* Tokens.  */
#define SC_PROGRAM 258
#define SC_FUNCTION 259
#define SC_VARIABLE 260
#define SC_NAME 261
#define SC_IVALUE 262
#define SC_FVALUE 263
#define SC_SCRIPT 264
#define SC_EXTERN 265
#define SC_FLOAT 266
#define SC_IF 267
#define SC_ELSE 268
#define SC_FOR 269
#define SC_WHILE 270
#define SC_DO 271
#define SC_BREAK 272
#define SC_CONTINUE 273
#define SC_RETURN 274
#define SC_PRINT 275
#define SC_UMINUS 276
#define SC_SCRIPT_LIST 277
#define SC_DEF_LIST 278
#define SC_STMT_LIST 279
#define SC_DEF_EFUN 280
#define SC_DEF_EVAR 281
#define SC_DEF_VAR 282
#define SC_EXIT 283
#define SC_ASSIGN 284
#define SC_IAS 285
#define SC_PE 286
#define SC_ME 287
#define SC_XE 288
#define SC_DE 289
#define SC_IASD 290
#define SC_AMD 291
#define SC_IFX 292
#define SC_BB 293
#define SC_NO_OP 294
#define SC_NEG 295
#define SC_COMMA 296
#define SC_EXPR_LIST 297
#define SC_AND 298
#define SC_OR 299
#define SC_EOR 300
#define SC_NOT 301
#define SC_INT 302
#define SC_GT 303
#define SC_LT 304
#define SC_GE 305
#define SC_LE 306
#define SC_EQ 307
#define SC_NE 308
#define SC_IGT 309
#define SC_ILT 310
#define SC_IGE 311
#define SC_ILE 312
#define SC_IEQ 313
#define SC_INE 314
#define SC_PLUS 315
#define SC_POW 316

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 73 "bas.y" /* yacc.c:1909  */

int   iValue  ;    /* Value of integer */
float fValue  ;    /* Value of Real */
char  *sValue ;    /* Value of string */
char  sIndex  ;    /* symbol table index */
nodeType *nPtr;    /* node pointer */

#line 184 "y.tab.h" /* yacc.c:1909  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (nodeType **sTree);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */
