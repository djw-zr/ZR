/* A Bison parser, made by GNU Bison 3.0.4.  */

/* Bison implementation for Yacc-like parsers in C

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

/* C LALR(1) parser skeleton written by Richard Stallman, by
   simplifying the original so-called "semantic" parser.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output.  */
#define YYBISON 1

/* Bison version.  */
#define YYBISON_VERSION "3.0.4"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* Copy the first part of user declarations.  */
#line 49 "bas.y" /* yacc.c:339  */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include "sigscr.h"

/*
 *  Prototypes
 */

/*  Routines defined below used to construct the Tree */

nodeType *opr(int oper, int noops, ... ) ;
nodeType *id(int i) ;
nodeType *con(int value) ;
nodeType *str(char *name) ;
int      ip = 0 ;

int  yylex(void);
void yyerror(nodeType **nPtr, char *s) ;
int  print_sigscr_node(nodeType *p) ;

#line 90 "y.tab.c" /* yacc.c:339  */

# ifndef YY_NULLPTR
#  if defined __cplusplus && 201103L <= __cplusplus
#   define YY_NULLPTR nullptr
#  else
#   define YY_NULLPTR 0
#  endif
# endif

/* Enabling verbose error messages.  */
#ifdef YYERROR_VERBOSE
# undef YYERROR_VERBOSE
# define YYERROR_VERBOSE 1
#else
# define YYERROR_VERBOSE 0
#endif

/* In a future release of Bison, this section will be replaced
   by #include "y.tab.h".  */
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
    SC_POW = 315
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
#define SC_POW 315

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 73 "bas.y" /* yacc.c:355  */

int   iValue  ;    /* Value of integer */
float fValue  ;    /* Value of Real */
char  *sValue ;    /* Value of string */
char  sIndex  ;    /* symbol table index */
nodeType *nPtr;    /* node pointer */

#line 258 "y.tab.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (nodeType **sTree);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 275 "y.tab.c" /* yacc.c:358  */

#ifdef short
# undef short
#endif

#ifdef YYTYPE_UINT8
typedef YYTYPE_UINT8 yytype_uint8;
#else
typedef unsigned char yytype_uint8;
#endif

#ifdef YYTYPE_INT8
typedef YYTYPE_INT8 yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef YYTYPE_UINT16
typedef YYTYPE_UINT16 yytype_uint16;
#else
typedef unsigned short int yytype_uint16;
#endif

#ifdef YYTYPE_INT16
typedef YYTYPE_INT16 yytype_int16;
#else
typedef short int yytype_int16;
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif ! defined YYSIZE_T
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned int
# endif
#endif

#define YYSIZE_MAXIMUM ((YYSIZE_T) -1)

#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> /* INFRINGES ON USER NAME SPACE */
#   define YY_(Msgid) dgettext ("bison-runtime", Msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(Msgid) Msgid
# endif
#endif

#ifndef YY_ATTRIBUTE
# if (defined __GNUC__                                               \
      && (2 < __GNUC__ || (__GNUC__ == 2 && 96 <= __GNUC_MINOR__)))  \
     || defined __SUNPRO_C && 0x5110 <= __SUNPRO_C
#  define YY_ATTRIBUTE(Spec) __attribute__(Spec)
# else
#  define YY_ATTRIBUTE(Spec) /* empty */
# endif
#endif

#ifndef YY_ATTRIBUTE_PURE
# define YY_ATTRIBUTE_PURE   YY_ATTRIBUTE ((__pure__))
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# define YY_ATTRIBUTE_UNUSED YY_ATTRIBUTE ((__unused__))
#endif

#if !defined _Noreturn \
     && (!defined __STDC_VERSION__ || __STDC_VERSION__ < 201112)
# if defined _MSC_VER && 1200 <= _MSC_VER
#  define _Noreturn __declspec (noreturn)
# else
#  define _Noreturn YY_ATTRIBUTE ((__noreturn__))
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YYUSE(E) ((void) (E))
#else
# define YYUSE(E) /* empty */
#endif

#if defined __GNUC__ && 407 <= __GNUC__ * 100 + __GNUC_MINOR__
/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN \
    _Pragma ("GCC diagnostic push") \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")\
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# define YY_IGNORE_MAYBE_UNINITIALIZED_END \
    _Pragma ("GCC diagnostic pop")
#else
# define YY_INITIAL_VALUE(Value) Value
#endif
#ifndef YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
# define YY_IGNORE_MAYBE_UNINITIALIZED_END
#endif
#ifndef YY_INITIAL_VALUE
# define YY_INITIAL_VALUE(Value) /* Nothing. */
#endif


#if ! defined yyoverflow || YYERROR_VERBOSE

/* The parser invokes alloca or malloc; define the necessary symbols.  */

# ifdef YYSTACK_USE_ALLOCA
#  if YYSTACK_USE_ALLOCA
#   ifdef __GNUC__
#    define YYSTACK_ALLOC __builtin_alloca
#   elif defined __BUILTIN_VA_ARG_INCR
#    include <alloca.h> /* INFRINGES ON USER NAME SPACE */
#   elif defined _AIX
#    define YYSTACK_ALLOC __alloca
#   elif defined _MSC_VER
#    include <malloc.h> /* INFRINGES ON USER NAME SPACE */
#    define alloca _alloca
#   else
#    define YYSTACK_ALLOC alloca
#    if ! defined _ALLOCA_H && ! defined EXIT_SUCCESS
#     include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
      /* Use EXIT_SUCCESS as a witness for stdlib.h.  */
#     ifndef EXIT_SUCCESS
#      define EXIT_SUCCESS 0
#     endif
#    endif
#   endif
#  endif
# endif

# ifdef YYSTACK_ALLOC
   /* Pacify GCC's 'empty if-body' warning.  */
#  define YYSTACK_FREE(Ptr) do { /* empty */; } while (0)
#  ifndef YYSTACK_ALLOC_MAXIMUM
    /* The OS might guarantee only one guard page at the bottom of the stack,
       and a page size can be as small as 4096 bytes.  So we cannot safely
       invoke alloca (N) if N exceeds 4096.  Use a slightly smaller number
       to allow for a few compiler-allocated temporary stack slots.  */
#   define YYSTACK_ALLOC_MAXIMUM 4032 /* reasonable circa 2006 */
#  endif
# else
#  define YYSTACK_ALLOC YYMALLOC
#  define YYSTACK_FREE YYFREE
#  ifndef YYSTACK_ALLOC_MAXIMUM
#   define YYSTACK_ALLOC_MAXIMUM YYSIZE_MAXIMUM
#  endif
#  if (defined __cplusplus && ! defined EXIT_SUCCESS \
       && ! ((defined YYMALLOC || defined malloc) \
             && (defined YYFREE || defined free)))
#   include <stdlib.h> /* INFRINGES ON USER NAME SPACE */
#   ifndef EXIT_SUCCESS
#    define EXIT_SUCCESS 0
#   endif
#  endif
#  ifndef YYMALLOC
#   define YYMALLOC malloc
#   if ! defined malloc && ! defined EXIT_SUCCESS
void *malloc (YYSIZE_T); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
#  ifndef YYFREE
#   define YYFREE free
#   if ! defined free && ! defined EXIT_SUCCESS
void free (void *); /* INFRINGES ON USER NAME SPACE */
#   endif
#  endif
# endif
#endif /* ! defined yyoverflow || YYERROR_VERBOSE */


#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yytype_int16 yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (sizeof (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (sizeof (yytype_int16) + sizeof (YYSTYPE)) \
      + YYSTACK_GAP_MAXIMUM)

# define YYCOPY_NEEDED 1

/* Relocate STACK from its old location to the new one.  The
   local variables YYSIZE and YYSTACKSIZE give the old and new number of
   elements in the stack, and YYPTR gives the new location of the
   stack.  Advance YYPTR to a properly aligned location for the next
   stack.  */
# define YYSTACK_RELOCATE(Stack_alloc, Stack)                           \
    do                                                                  \
      {                                                                 \
        YYSIZE_T yynewbytes;                                            \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * sizeof (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / sizeof (*yyptr);                          \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, (Count) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYSIZE_T yyi;                         \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  9
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   553

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  76
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  11
/* YYNRULES -- Number of rules.  */
#define YYNRULES  62
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  132

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   315

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    68,     2,    69,     2,     2,     2,     2,
      71,    72,    66,    64,     2,    65,     2,    67,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,    70,
      62,    73,    61,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,    63,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    74,     2,    75,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32,    33,    34,
      35,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint8 yyrline[] =
{
       0,   134,   134,   142,   146,   151,   156,   158,   162,   164,
     167,   170,   174,   175,   176,   177,   178,   179,   180,   181,
     182,   183,   184,   185,   186,   187,   188,   192,   193,   196,
     197,   198,   199,   200,   201,   202,   203,   204,   205,   206,
     207,   208,   209,   210,   211,   212,   213,   214,   215,   216,
     217,   218,   219,   220,   221,   222,   223,   224,   228,   229,
     233,   234,   235
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "SC_PROGRAM", "SC_FUNCTION",
  "SC_VARIABLE", "SC_NAME", "SC_IVALUE", "SC_FVALUE", "SC_SCRIPT",
  "SC_EXTERN", "SC_FLOAT", "SC_IF", "SC_ELSE", "SC_FOR", "SC_WHILE",
  "SC_DO", "SC_BREAK", "SC_CONTINUE", "SC_RETURN", "SC_PRINT", "SC_UMINUS",
  "SC_SCRIPT_LIST", "SC_DEF_LIST", "SC_STMT_LIST", "SC_DEF_EFUN",
  "SC_DEF_EVAR", "SC_DEF_VAR", "SC_EXIT", "SC_ASSIGN", "SC_IAS", "SC_PE",
  "SC_ME", "SC_XE", "SC_DE", "SC_IASD", "SC_AMD", "SC_IFX", "SC_BB",
  "SC_NO_OP", "SC_NEG", "SC_COMMA", "SC_EXPR_LIST", "SC_AND", "SC_OR",
  "SC_EOR", "SC_NOT", "SC_INT", "SC_GT", "SC_LT", "SC_GE", "SC_LE",
  "SC_EQ", "SC_NE", "SC_IGT", "SC_ILT", "SC_IGE", "SC_ILE", "SC_IEQ",
  "SC_INE", "SC_POW", "'>'", "'<'", "'^'", "'+'", "'-'", "'*'", "'/'",
  "'!'", "'#'", "';'", "'('", "')'", "'='", "'{'", "'}'", "$accept",
  "program", "script_list", "script", "defn_list", "defn", "stmt",
  "stmt_list", "expr", "expr_list", "function", YY_NULLPTR
};
#endif

# ifdef YYPRINT
/* YYTOKNUM[NUM] -- (External) token number corresponding to the
   (internal) symbol number NUM (which must be that of a token).  */
static const yytype_uint16 yytoknum[] =
{
       0,   256,   257,   258,   259,   260,   261,   262,   263,   264,
     265,   266,   267,   268,   269,   270,   271,   272,   273,   274,
     275,   276,   277,   278,   279,   280,   281,   282,   283,   284,
     285,   286,   287,   288,   289,   290,   291,   292,   293,   294,
     295,   296,   297,   298,   299,   300,   301,   302,   303,   304,
     305,   306,   307,   308,   309,   310,   311,   312,   313,   314,
     315,    62,    60,    94,    43,    45,    42,    47,    33,    35,
      59,    40,    41,    61,   123,   125
};
# endif

#define YYPACT_NINF -57

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-57)))

#define YYTABLE_NINF -1

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
      13,     2,    30,    13,   -57,    20,    26,    65,   -57,   -57,
     -57,    29,    -9,   134,   -57,   -57,    -8,    -7,   106,   -57,
     106,   106,   106,   106,   -57,   106,    80,   -57,   -57,    80,
     216,   -57,   -37,   -57,   106,   106,   106,   106,   106,   106,
     -57,    91,   106,   106,   106,   -11,   244,   -57,   -57,   -57,
     -57,   136,    50,   -57,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   106,   106,   106,   106,   106,   106,
     106,   106,   106,   106,   -57,    -4,   -57,    -5,   272,   300,
     328,   356,   384,   412,   -57,   468,   -13,   440,   166,   191,
     -57,   -57,   -57,   486,   486,   486,   -56,   -56,   -56,   -56,
     -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -56,   -52,
     -52,   -57,   -57,   -57,    -2,   -57,   -57,   -57,   -57,   -57,
     -57,   106,   -57,   -57,    80,    80,   -57,   468,    56,   -57,
      80,   -57
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       0,     0,     0,     2,     3,     0,     0,     0,     6,     1,
       4,     0,     0,    29,    30,    31,     0,     0,     0,    26,
       0,     0,     0,     0,    12,     0,     0,     7,    27,     5,
       0,    33,     0,    11,     0,     0,     0,     0,     0,     0,
      60,     0,     0,     0,     0,    29,     0,    52,    32,    51,
      53,     0,     0,    28,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,    13,     0,    10,     0,     0,     0,
       0,     0,     0,     0,    61,    58,     0,     0,     0,     0,
      14,    50,    25,    54,    55,    56,    40,    41,    43,    42,
      44,    45,    46,    47,    48,    49,    39,    38,    57,    34,
      35,    36,    37,     8,     0,    16,    17,    18,    19,    20,
      21,     0,    62,    15,     0,     0,     9,    59,    23,    22,
       0,    24
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -57,   -57,   -57,    71,   -57,    72,   -23,    55,   -18,   -57,
     -57
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
      -1,     2,     3,     4,     7,     8,    28,    29,    30,    86,
      31
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_uint8 yytable[] =
{
      46,    75,    47,    48,    49,    50,    53,    51,    70,    71,
      72,    73,     5,     6,    72,    73,    78,    79,    80,    81,
      82,    83,     1,    85,    87,    88,    89,    40,   121,    53,
       9,    11,    12,    76,    77,    32,    93,    94,    95,    96,
      97,    98,    99,   100,   101,   102,   103,   104,   105,   106,
     107,   108,   109,   110,   111,   112,    13,    14,    15,   122,
      41,    33,    16,    43,    44,    17,   113,   114,   126,   130,
      18,    13,    14,    15,    10,     5,     6,    16,    19,    27,
      17,    52,     0,     0,     0,    18,    13,    14,    15,     0,
       0,     0,    16,    19,     0,    17,    20,    45,    14,    15,
      18,   128,   129,   127,     0,     0,     0,   131,    19,     0,
       0,    20,    45,    14,    15,    21,     0,     0,    22,    23,
      24,    25,     0,     0,    26,    92,    20,     0,     0,     0,
      21,     0,     0,    22,    23,    24,    25,    20,     0,    26,
       0,     0,     0,     0,     0,    21,     0,     0,    22,    23,
      24,    25,    20,     0,    26,     0,    21,     0,     0,    22,
      23,     0,    25,    84,    34,    35,    36,    37,    38,    39,
       0,    21,    40,     0,    22,    23,     0,    25,     0,    54,
      55,    56,     0,     0,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,    68,    69,
      70,    71,    72,    73,     0,    41,     0,    42,    91,    54,
      55,    56,     0,     0,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,    68,    69,
      70,    71,    72,    73,    54,    55,    56,     0,   124,     0,
       0,    57,    58,    59,    60,    61,    62,    63,    64,    65,
      66,     0,    67,    68,    69,    70,    71,    72,    73,    54,
      55,    56,     0,   125,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,    68,    69,
      70,    71,    72,    73,     0,     0,    74,    54,    55,    56,
       0,     0,     0,     0,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    67,    68,    69,    70,    71,
      72,    73,     0,     0,    90,    54,    55,    56,     0,     0,
       0,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,    68,    69,    70,    71,    72,    73,
       0,     0,   115,    54,    55,    56,     0,     0,     0,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
       0,    67,    68,    69,    70,    71,    72,    73,     0,     0,
     116,    54,    55,    56,     0,     0,     0,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,     0,    67,
      68,    69,    70,    71,    72,    73,     0,     0,   117,    54,
      55,    56,     0,     0,     0,     0,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,    68,    69,
      70,    71,    72,    73,     0,     0,   118,    54,    55,    56,
       0,     0,     0,     0,    57,    58,    59,    60,    61,    62,
      63,    64,    65,    66,     0,    67,    68,    69,    70,    71,
      72,    73,     0,     0,   119,    54,    55,    56,     0,     0,
       0,     0,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,     0,    67,    68,    69,    70,    71,    72,    73,
       0,     0,   120,    54,    55,    56,     0,     0,     0,     0,
      57,    58,    59,    60,    61,    62,    63,    64,    65,    66,
       0,    67,    68,    69,    70,    71,    72,    73,     0,     0,
     123,    54,    55,    56,     0,     0,     0,     0,    57,    58,
      59,    60,    61,    62,    63,    64,    65,    66,     0,    67,
      68,    69,    70,    71,    72,    73,    57,    58,    59,    60,
      61,    62,    63,    64,    65,    66,     0,    67,    68,    69,
      70,    71,    72,    73
};

static const yytype_int16 yycheck[] =
{
      18,    38,    20,    21,    22,    23,    29,    25,    64,    65,
      66,    67,    10,    11,    66,    67,    34,    35,    36,    37,
      38,    39,     9,    41,    42,    43,    44,    38,    41,    52,
       0,    11,     6,    70,    71,     6,    54,    55,    56,    57,
      58,    59,    60,    61,    62,    63,    64,    65,    66,    67,
      68,    69,    70,    71,    72,    73,     6,     7,     8,    72,
      71,    70,    12,    71,    71,    15,    70,    72,    70,    13,
      20,     6,     7,     8,     3,    10,    11,    12,    28,     7,
      15,    26,    -1,    -1,    -1,    20,     6,     7,     8,    -1,
      -1,    -1,    12,    28,    -1,    15,    46,     6,     7,     8,
      20,   124,   125,   121,    -1,    -1,    -1,   130,    28,    -1,
      -1,    46,     6,     7,     8,    65,    -1,    -1,    68,    69,
      70,    71,    -1,    -1,    74,    75,    46,    -1,    -1,    -1,
      65,    -1,    -1,    68,    69,    70,    71,    46,    -1,    74,
      -1,    -1,    -1,    -1,    -1,    65,    -1,    -1,    68,    69,
      70,    71,    46,    -1,    74,    -1,    65,    -1,    -1,    68,
      69,    -1,    71,    72,    30,    31,    32,    33,    34,    35,
      -1,    65,    38,    -1,    68,    69,    -1,    71,    -1,    43,
      44,    45,    -1,    -1,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    -1,    71,    -1,    73,    72,    43,
      44,    45,    -1,    -1,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    43,    44,    45,    -1,    72,    -1,
      -1,    50,    51,    52,    53,    54,    55,    56,    57,    58,
      59,    -1,    61,    62,    63,    64,    65,    66,    67,    43,
      44,    45,    -1,    72,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    -1,    -1,    70,    43,    44,    45,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    61,    62,    63,    64,    65,
      66,    67,    -1,    -1,    70,    43,    44,    45,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    61,    62,    63,    64,    65,    66,    67,
      -1,    -1,    70,    43,    44,    45,    -1,    -1,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    62,    63,    64,    65,    66,    67,    -1,    -1,
      70,    43,    44,    45,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    61,
      62,    63,    64,    65,    66,    67,    -1,    -1,    70,    43,
      44,    45,    -1,    -1,    -1,    -1,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67,    -1,    -1,    70,    43,    44,    45,
      -1,    -1,    -1,    -1,    50,    51,    52,    53,    54,    55,
      56,    57,    58,    59,    -1,    61,    62,    63,    64,    65,
      66,    67,    -1,    -1,    70,    43,    44,    45,    -1,    -1,
      -1,    -1,    50,    51,    52,    53,    54,    55,    56,    57,
      58,    59,    -1,    61,    62,    63,    64,    65,    66,    67,
      -1,    -1,    70,    43,    44,    45,    -1,    -1,    -1,    -1,
      50,    51,    52,    53,    54,    55,    56,    57,    58,    59,
      -1,    61,    62,    63,    64,    65,    66,    67,    -1,    -1,
      70,    43,    44,    45,    -1,    -1,    -1,    -1,    50,    51,
      52,    53,    54,    55,    56,    57,    58,    59,    -1,    61,
      62,    63,    64,    65,    66,    67,    50,    51,    52,    53,
      54,    55,    56,    57,    58,    59,    -1,    61,    62,    63,
      64,    65,    66,    67
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,     9,    77,    78,    79,    10,    11,    80,    81,     0,
      79,    11,     6,     6,     7,     8,    12,    15,    20,    28,
      46,    65,    68,    69,    70,    71,    74,    81,    82,    83,
      84,    86,     6,    70,    30,    31,    32,    33,    34,    35,
      38,    71,    73,    71,    71,     6,    84,    84,    84,    84,
      84,    84,    83,    82,    43,    44,    45,    50,    51,    52,
      53,    54,    55,    56,    57,    58,    59,    61,    62,    63,
      64,    65,    66,    67,    70,    38,    70,    71,    84,    84,
      84,    84,    84,    84,    72,    84,    85,    84,    84,    84,
      70,    72,    75,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    70,    72,    70,    70,    70,    70,    70,
      70,    41,    72,    70,    72,    72,    70,    84,    82,    82,
      13,    82
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,    76,    77,    78,    78,    79,    80,    80,    81,    81,
      81,    81,    82,    82,    82,    82,    82,    82,    82,    82,
      82,    82,    82,    82,    82,    82,    82,    83,    83,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    84,    84,
      84,    84,    84,    84,    84,    84,    84,    84,    85,    85,
      86,    86,    86
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     1,     1,     2,     3,     1,     2,     5,     6,
       4,     3,     1,     2,     3,     4,     4,     4,     4,     4,
       4,     4,     5,     5,     7,     3,     1,     1,     2,     1,
       1,     1,     2,     1,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     2,     2,     2,     3,     3,     3,     3,     1,     3,
       2,     3,     4
};


#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)
#define YYEMPTY         (-2)
#define YYEOF           0

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                  \
do                                                              \
  if (yychar == YYEMPTY)                                        \
    {                                                           \
      yychar = (Token);                                         \
      yylval = (Value);                                         \
      YYPOPSTACK (yylen);                                       \
      yystate = *yyssp;                                         \
      goto yybackup;                                            \
    }                                                           \
  else                                                          \
    {                                                           \
      yyerror (sTree, YY_("syntax error: cannot back up")); \
      YYERROR;                                                  \
    }                                                           \
while (0)

/* Error token number */
#define YYTERROR        1
#define YYERRCODE       256



/* Enable debugging if requested.  */
#if YYDEBUG

# ifndef YYFPRINTF
#  include <stdio.h> /* INFRINGES ON USER NAME SPACE */
#  define YYFPRINTF fprintf
# endif

# define YYDPRINTF(Args)                        \
do {                                            \
  if (yydebug)                                  \
    YYFPRINTF Args;                             \
} while (0)

/* This macro is provided for backward compatibility. */
#ifndef YY_LOCATION_PRINT
# define YY_LOCATION_PRINT(File, Loc) ((void) 0)
#endif


# define YY_SYMBOL_PRINT(Title, Type, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Type, Value, sTree); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, nodeType **sTree)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
  YYUSE (sTree);
  if (!yyvaluep)
    return;
# ifdef YYPRINT
  if (yytype < YYNTOKENS)
    YYPRINT (yyoutput, yytoknum[yytype], *yyvaluep);
# endif
  YYUSE (yytype);
}


/*--------------------------------.
| Print this symbol on YYOUTPUT.  |
`--------------------------------*/

static void
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep, nodeType **sTree)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep, sTree);
  YYFPRINTF (yyoutput, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yytype_int16 *yybottom, yytype_int16 *yytop)
{
  YYFPRINTF (stderr, "Stack now");
  for (; yybottom <= yytop; yybottom++)
    {
      int yybot = *yybottom;
      YYFPRINTF (stderr, " %d", yybot);
    }
  YYFPRINTF (stderr, "\n");
}

# define YY_STACK_PRINT(Bottom, Top)                            \
do {                                                            \
  if (yydebug)                                                  \
    yy_stack_print ((Bottom), (Top));                           \
} while (0)


/*------------------------------------------------.
| Report that the YYRULE is going to be reduced.  |
`------------------------------------------------*/

static void
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule, nodeType **sTree)
{
  unsigned long int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %lu):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       yystos[yyssp[yyi + 1 - yynrhs]],
                       &(yyvsp[(yyi + 1) - (yynrhs)])
                                              , sTree);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule, sTree); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args)
# define YY_SYMBOL_PRINT(Title, Type, Value, Location)
# define YY_STACK_PRINT(Bottom, Top)
# define YY_REDUCE_PRINT(Rule)
#endif /* !YYDEBUG */


/* YYINITDEPTH -- initial size of the parser's stacks.  */
#ifndef YYINITDEPTH
# define YYINITDEPTH 200
#endif

/* YYMAXDEPTH -- maximum size the stacks can grow to (effective only
   if the built-in stack extension method is used).

   Do not make this value too large; the results are undefined if
   YYSTACK_ALLOC_MAXIMUM < YYSTACK_BYTES (YYMAXDEPTH)
   evaluated with infinite-precision integer arithmetic.  */

#ifndef YYMAXDEPTH
# define YYMAXDEPTH 10000
#endif


#if YYERROR_VERBOSE

# ifndef yystrlen
#  if defined __GLIBC__ && defined _STRING_H
#   define yystrlen strlen
#  else
/* Return the length of YYSTR.  */
static YYSIZE_T
yystrlen (const char *yystr)
{
  YYSIZE_T yylen;
  for (yylen = 0; yystr[yylen]; yylen++)
    continue;
  return yylen;
}
#  endif
# endif

# ifndef yystpcpy
#  if defined __GLIBC__ && defined _STRING_H && defined _GNU_SOURCE
#   define yystpcpy stpcpy
#  else
/* Copy YYSRC to YYDEST, returning the address of the terminating '\0' in
   YYDEST.  */
static char *
yystpcpy (char *yydest, const char *yysrc)
{
  char *yyd = yydest;
  const char *yys = yysrc;

  while ((*yyd++ = *yys++) != '\0')
    continue;

  return yyd - 1;
}
#  endif
# endif

# ifndef yytnamerr
/* Copy to YYRES the contents of YYSTR after stripping away unnecessary
   quotes and backslashes, so that it's suitable for yyerror.  The
   heuristic is that double-quoting is unnecessary unless the string
   contains an apostrophe, a comma, or backslash (other than
   backslash-backslash).  YYSTR is taken from yytname.  If YYRES is
   null, do not copy; instead, return the length of what the result
   would have been.  */
static YYSIZE_T
yytnamerr (char *yyres, const char *yystr)
{
  if (*yystr == '"')
    {
      YYSIZE_T yyn = 0;
      char const *yyp = yystr;

      for (;;)
        switch (*++yyp)
          {
          case '\'':
          case ',':
            goto do_not_strip_quotes;

          case '\\':
            if (*++yyp != '\\')
              goto do_not_strip_quotes;
            /* Fall through.  */
          default:
            if (yyres)
              yyres[yyn] = *yyp;
            yyn++;
            break;

          case '"':
            if (yyres)
              yyres[yyn] = '\0';
            return yyn;
          }
    do_not_strip_quotes: ;
    }

  if (! yyres)
    return yystrlen (yystr);

  return yystpcpy (yyres, yystr) - yyres;
}
# endif

/* Copy into *YYMSG, which is of size *YYMSG_ALLOC, an error message
   about the unexpected token YYTOKEN for the state stack whose top is
   YYSSP.

   Return 0 if *YYMSG was successfully written.  Return 1 if *YYMSG is
   not large enough to hold the message.  In that case, also set
   *YYMSG_ALLOC to the required number of bytes.  Return 2 if the
   required number of bytes is too large to store.  */
static int
yysyntax_error (YYSIZE_T *yymsg_alloc, char **yymsg,
                yytype_int16 *yyssp, int yytoken)
{
  YYSIZE_T yysize0 = yytnamerr (YY_NULLPTR, yytname[yytoken]);
  YYSIZE_T yysize = yysize0;
  enum { YYERROR_VERBOSE_ARGS_MAXIMUM = 5 };
  /* Internationalized format string. */
  const char *yyformat = YY_NULLPTR;
  /* Arguments of yyformat. */
  char const *yyarg[YYERROR_VERBOSE_ARGS_MAXIMUM];
  /* Number of reported tokens (one for the "unexpected", one per
     "expected"). */
  int yycount = 0;

  /* There are many possibilities here to consider:
     - If this state is a consistent state with a default action, then
       the only way this function was invoked is if the default action
       is an error action.  In that case, don't check for expected
       tokens because there are none.
     - The only way there can be no lookahead present (in yychar) is if
       this state is a consistent state with a default action.  Thus,
       detecting the absence of a lookahead is sufficient to determine
       that there is no unexpected or expected token to report.  In that
       case, just report a simple "syntax error".
     - Don't assume there isn't a lookahead just because this state is a
       consistent state with a default action.  There might have been a
       previous inconsistent state, consistent state with a non-default
       action, or user semantic action that manipulated yychar.
     - Of course, the expected token list depends on states to have
       correct lookahead information, and it depends on the parser not
       to perform extra reductions after fetching a lookahead from the
       scanner and before detecting a syntax error.  Thus, state merging
       (from LALR or IELR) and default reductions corrupt the expected
       token list.  However, the list is correct for canonical LR with
       one exception: it will still contain any token that will not be
       accepted due to an error action in a later state.
  */
  if (yytoken != YYEMPTY)
    {
      int yyn = yypact[*yyssp];
      yyarg[yycount++] = yytname[yytoken];
      if (!yypact_value_is_default (yyn))
        {
          /* Start YYX at -YYN if negative to avoid negative indexes in
             YYCHECK.  In other words, skip the first -YYN actions for
             this state because they are default actions.  */
          int yyxbegin = yyn < 0 ? -yyn : 0;
          /* Stay within bounds of both yycheck and yytname.  */
          int yychecklim = YYLAST - yyn + 1;
          int yyxend = yychecklim < YYNTOKENS ? yychecklim : YYNTOKENS;
          int yyx;

          for (yyx = yyxbegin; yyx < yyxend; ++yyx)
            if (yycheck[yyx + yyn] == yyx && yyx != YYTERROR
                && !yytable_value_is_error (yytable[yyx + yyn]))
              {
                if (yycount == YYERROR_VERBOSE_ARGS_MAXIMUM)
                  {
                    yycount = 1;
                    yysize = yysize0;
                    break;
                  }
                yyarg[yycount++] = yytname[yyx];
                {
                  YYSIZE_T yysize1 = yysize + yytnamerr (YY_NULLPTR, yytname[yyx]);
                  if (! (yysize <= yysize1
                         && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
                    return 2;
                  yysize = yysize1;
                }
              }
        }
    }

  switch (yycount)
    {
# define YYCASE_(N, S)                      \
      case N:                               \
        yyformat = S;                       \
      break
      YYCASE_(0, YY_("syntax error"));
      YYCASE_(1, YY_("syntax error, unexpected %s"));
      YYCASE_(2, YY_("syntax error, unexpected %s, expecting %s"));
      YYCASE_(3, YY_("syntax error, unexpected %s, expecting %s or %s"));
      YYCASE_(4, YY_("syntax error, unexpected %s, expecting %s or %s or %s"));
      YYCASE_(5, YY_("syntax error, unexpected %s, expecting %s or %s or %s or %s"));
# undef YYCASE_
    }

  {
    YYSIZE_T yysize1 = yysize + yystrlen (yyformat);
    if (! (yysize <= yysize1 && yysize1 <= YYSTACK_ALLOC_MAXIMUM))
      return 2;
    yysize = yysize1;
  }

  if (*yymsg_alloc < yysize)
    {
      *yymsg_alloc = 2 * yysize;
      if (! (yysize <= *yymsg_alloc
             && *yymsg_alloc <= YYSTACK_ALLOC_MAXIMUM))
        *yymsg_alloc = YYSTACK_ALLOC_MAXIMUM;
      return 1;
    }

  /* Avoid sprintf, as that infringes on the user's name space.
     Don't have undefined behavior even if the translation
     produced a string with the wrong number of "%s"s.  */
  {
    char *yyp = *yymsg;
    int yyi = 0;
    while ((*yyp = *yyformat) != '\0')
      if (*yyp == '%' && yyformat[1] == 's' && yyi < yycount)
        {
          yyp += yytnamerr (yyp, yyarg[yyi++]);
          yyformat += 2;
        }
      else
        {
          yyp++;
          yyformat++;
        }
  }
  return 0;
}
#endif /* YYERROR_VERBOSE */

/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep, nodeType **sTree)
{
  YYUSE (yyvaluep);
  YYUSE (sTree);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yytype, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YYUSE (yytype);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}




/* The lookahead symbol.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;


/*----------.
| yyparse.  |
`----------*/

int
yyparse (nodeType **sTree)
{
    int yystate;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus;

    /* The stacks and their tools:
       'yyss': related to states.
       'yyvs': related to semantic values.

       Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* The state stack.  */
    yytype_int16 yyssa[YYINITDEPTH];
    yytype_int16 *yyss;
    yytype_int16 *yyssp;

    /* The semantic value stack.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs;
    YYSTYPE *yyvsp;

    YYSIZE_T yystacksize;

  int yyn;
  int yyresult;
  /* Lookahead token as an internal (translated) token number.  */
  int yytoken = 0;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;

#if YYERROR_VERBOSE
  /* Buffer for error messages, and its allocated size.  */
  char yymsgbuf[128];
  char *yymsg = yymsgbuf;
  YYSIZE_T yymsg_alloc = sizeof yymsgbuf;
#endif

#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  yyssp = yyss = yyssa;
  yyvsp = yyvs = yyvsa;
  yystacksize = YYINITDEPTH;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yystate = 0;
  yyerrstatus = 0;
  yynerrs = 0;
  yychar = YYEMPTY; /* Cause a token to be read.  */
  goto yysetstate;

/*------------------------------------------------------------.
| yynewstate -- Push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
 yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;

 yysetstate:
  *yyssp = yystate;

  if (yyss + yystacksize - 1 <= yyssp)
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYSIZE_T yysize = yyssp - yyss + 1;

#ifdef yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        YYSTYPE *yyvs1 = yyvs;
        yytype_int16 *yyss1 = yyss;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * sizeof (*yyssp),
                    &yyvs1, yysize * sizeof (*yyvsp),
                    &yystacksize);

        yyss = yyss1;
        yyvs = yyvs1;
      }
#else /* no yyoverflow */
# ifndef YYSTACK_RELOCATE
      goto yyexhaustedlab;
# else
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        goto yyexhaustedlab;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yytype_int16 *yyss1 = yyss;
        union yyalloc *yyptr =
          (union yyalloc *) YYSTACK_ALLOC (YYSTACK_BYTES (yystacksize));
        if (! yyptr)
          goto yyexhaustedlab;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif
#endif /* no yyoverflow */

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YYDPRINTF ((stderr, "Stack size increased to %lu\n",
                  (unsigned long int) yystacksize));

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }

  YYDPRINTF ((stderr, "Entering state %d\n", yystate));

  if (yystate == YYFINAL)
    YYACCEPT;

  goto yybackup;

/*-----------.
| yybackup.  |
`-----------*/
yybackup:

  /* Do appropriate processing given the current state.  Read a
     lookahead token if we need one and don't already have one.  */

  /* First try to decide what to do without reference to lookahead token.  */
  yyn = yypact[yystate];
  if (yypact_value_is_default (yyn))
    goto yydefault;

  /* Not known => get a lookahead token if don't already have one.  */

  /* YYCHAR is either YYEMPTY or YYEOF or a valid lookahead symbol.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token: "));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = yytoken = YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else
    {
      yytoken = YYTRANSLATE (yychar);
      YY_SYMBOL_PRINT ("Next token is", yytoken, &yylval, &yylloc);
    }

  /* If the proper action on seeing token YYTOKEN is to reduce or to
     detect an error, take that action.  */
  yyn += yytoken;
  if (yyn < 0 || YYLAST < yyn || yycheck[yyn] != yytoken)
    goto yydefault;
  yyn = yytable[yyn];
  if (yyn <= 0)
    {
      if (yytable_value_is_error (yyn))
        goto yyerrlab;
      yyn = -yyn;
      goto yyreduce;
    }

  /* Count tokens shifted since error; after three, turn off error
     status.  */
  if (yyerrstatus)
    yyerrstatus--;

  /* Shift the lookahead token.  */
  YY_SYMBOL_PRINT ("Shifting", yytoken, &yylval, &yylloc);

  /* Discard the shifted token.  */
  yychar = YYEMPTY;

  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  goto yynewstate;


/*-----------------------------------------------------------.
| yydefault -- do the default action for the current state.  |
`-----------------------------------------------------------*/
yydefault:
  yyn = yydefact[yystate];
  if (yyn == 0)
    goto yyerrlab;
  goto yyreduce;


/*-----------------------------.
| yyreduce -- Do a reduction.  |
`-----------------------------*/
yyreduce:
  /* yyn is the number of a rule to reduce with.  */
  yylen = yyr2[yyn];

  /* If YYLEN is nonzero, implement the default value of the action:
     '$$ = $1'.

     Otherwise, the following line sets YYVAL to garbage.
     This behavior is undocumented and Bison
     users should not rely upon it.  Assigning to YYVAL
     unconditionally makes the parser a bit smaller, and it avoids a
     GCC warning that YYVAL may be used uninitialized.  */
  yyval = yyvsp[1-yylen];


  YY_REDUCE_PRINT (yyn);
  switch (yyn)
    {
        case 2:
#line 134 "bas.y" /* yacc.c:1646  */
    {
                                   if(ip)printf("\n BISON: EXIT MSTS COMPILER ROUTINE\n");
                                   (yyval.nPtr) = opr(SC_PROGRAM,1,(yyvsp[0].nPtr)) ;
                                   *sTree = (yyval.nPtr) ;
                                   return 0; /*exit(0)*/; }
#line 1540 "y.tab.c" /* yacc.c:1646  */
    break;

  case 3:
#line 142 "bas.y" /* yacc.c:1646  */
    {
                                   if(ip)printf("\n BISON: Found SC_SCRIPT_LIST 1\n");
                                   (yyval.nPtr) = (yyvsp[0].nPtr);                }
#line 1548 "y.tab.c" /* yacc.c:1646  */
    break;

  case 4:
#line 146 "bas.y" /* yacc.c:1646  */
    {
                                   if(ip)printf("\n BISON: Found SC_SCRIPT_LIST 2\n");
                                   (yyval.nPtr) = opr(SC_SCRIPT_LIST, 2, (yyvsp[-1].nPtr), (yyvsp[0].nPtr)); }
#line 1556 "y.tab.c" /* yacc.c:1646  */
    break;

  case 5:
#line 151 "bas.y" /* yacc.c:1646  */
    {
                                   if(ip)printf("\n BISON: Found SC_SCRIPT\n");
                                   (yyval.nPtr) = opr(SC_SCRIPT,3,str((yyvsp[-2].sValue)),(yyvsp[-1].nPtr),(yyvsp[0].nPtr)) ; }
#line 1564 "y.tab.c" /* yacc.c:1646  */
    break;

  case 6:
#line 156 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found DEFINITION\n");
                                   (yyval.nPtr) = (yyvsp[0].nPtr);                            }
#line 1571 "y.tab.c" /* yacc.c:1646  */
    break;

  case 7:
#line 158 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found DEFINITION LIST\n");
                                   (yyval.nPtr) = opr(SC_DEF_LIST, 2, (yyvsp[-1].nPtr), (yyvsp[0].nPtr));   }
#line 1578 "y.tab.c" /* yacc.c:1646  */
    break;

  case 8:
#line 162 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found EXTERN FLOAT FUNCTION\n");
                                   (yyval.nPtr) = opr(SC_DEF_EFUN, 1, str((yyvsp[-2].sValue)));  }
#line 1585 "y.tab.c" /* yacc.c:1646  */
    break;

  case 9:
#line 164 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found EXTERN FLOAT FUNCTION\n");
                                   (yyval.nPtr) = opr(SC_DEF_EFUN, 1, str((yyvsp[-3].sValue)));  }
#line 1592 "y.tab.c" /* yacc.c:1646  */
    break;

  case 10:
#line 167 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found EXTERN FLOAT VARIABLE\n");
                                   (yyval.nPtr) = opr(SC_DEF_EVAR, 1, str((yyvsp[-1].sValue)));  }
#line 1599 "y.tab.c" /* yacc.c:1646  */
    break;

  case 11:
#line 170 "bas.y" /* yacc.c:1646  */
    { if(ip)printf("\n BISON: Found EXTERN FLOAT VARIABLE\n");
                                   (yyval.nPtr) = opr(SC_DEF_VAR, 1, str((yyvsp[-1].sValue)));    }
#line 1606 "y.tab.c" /* yacc.c:1646  */
    break;

  case 12:
#line 174 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_NO_OP, 2, NULL, NULL);   }
#line 1612 "y.tab.c" /* yacc.c:1646  */
    break;

  case 13:
#line 175 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[-1].nPtr) ;                            }
#line 1618 "y.tab.c" /* yacc.c:1646  */
    break;

  case 14:
#line 176 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_PRINT, 1, (yyvsp[-1].nPtr)) ;          }
#line 1624 "y.tab.c" /* yacc.c:1646  */
    break;

  case 15:
#line 177 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_ASSIGN, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr)); }
#line 1630 "y.tab.c" /* yacc.c:1646  */
    break;

  case 16:
#line 178 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IAS, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));    }
#line 1636 "y.tab.c" /* yacc.c:1646  */
    break;

  case 17:
#line 179 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_PE, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));     }
#line 1642 "y.tab.c" /* yacc.c:1646  */
    break;

  case 18:
#line 180 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_ME, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));     }
#line 1648 "y.tab.c" /* yacc.c:1646  */
    break;

  case 19:
#line 181 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_XE, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));     }
#line 1654 "y.tab.c" /* yacc.c:1646  */
    break;

  case 20:
#line 182 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_DE, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));     }
#line 1660 "y.tab.c" /* yacc.c:1646  */
    break;

  case 21:
#line 183 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IASD, 2, str((yyvsp[-3].sValue)), (yyvsp[-1].nPtr));   }
#line 1666 "y.tab.c" /* yacc.c:1646  */
    break;

  case 22:
#line 184 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_WHILE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));       }
#line 1672 "y.tab.c" /* yacc.c:1646  */
    break;

  case 23:
#line 185 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IF, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));      }
#line 1678 "y.tab.c" /* yacc.c:1646  */
    break;

  case 24:
#line 186 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IF, 3, (yyvsp[-4].nPtr), (yyvsp[-2].nPtr), (yyvsp[0].nPtr));  }
#line 1684 "y.tab.c" /* yacc.c:1646  */
    break;

  case 25:
#line 187 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[-1].nPtr);                             }
#line 1690 "y.tab.c" /* yacc.c:1646  */
    break;

  case 26:
#line 188 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_EXIT, 1, NULL);          }
#line 1696 "y.tab.c" /* yacc.c:1646  */
    break;

  case 27:
#line 192 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[0].nPtr);                             }
#line 1702 "y.tab.c" /* yacc.c:1646  */
    break;

  case 28:
#line 193 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_STMT_LIST, 2, (yyvsp[-1].nPtr), (yyvsp[0].nPtr));   }
#line 1708 "y.tab.c" /* yacc.c:1646  */
    break;

  case 29:
#line 196 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_VARIABLE,1,str((yyvsp[0].sValue))) ;    }
#line 1714 "y.tab.c" /* yacc.c:1646  */
    break;

  case 30:
#line 197 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IVALUE,1,str((yyvsp[0].sValue))) ;      }
#line 1720 "y.tab.c" /* yacc.c:1646  */
    break;

  case 31:
#line 198 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_FVALUE,1,str((yyvsp[0].sValue))) ;      }
#line 1726 "y.tab.c" /* yacc.c:1646  */
    break;

  case 32:
#line 199 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_UMINUS, 1, (yyvsp[0].nPtr));          }
#line 1732 "y.tab.c" /* yacc.c:1646  */
    break;

  case 33:
#line 200 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[0].nPtr) ;                            }
#line 1738 "y.tab.c" /* yacc.c:1646  */
    break;

  case 34:
#line 201 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('+', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1744 "y.tab.c" /* yacc.c:1646  */
    break;

  case 35:
#line 202 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('-', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1750 "y.tab.c" /* yacc.c:1646  */
    break;

  case 36:
#line 203 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('*', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1756 "y.tab.c" /* yacc.c:1646  */
    break;

  case 37:
#line 204 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('/', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1762 "y.tab.c" /* yacc.c:1646  */
    break;

  case 38:
#line 205 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('<', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1768 "y.tab.c" /* yacc.c:1646  */
    break;

  case 39:
#line 206 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr('>', 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));            }
#line 1774 "y.tab.c" /* yacc.c:1646  */
    break;

  case 40:
#line 207 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_GE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));          }
#line 1780 "y.tab.c" /* yacc.c:1646  */
    break;

  case 41:
#line 208 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_LE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));          }
#line 1786 "y.tab.c" /* yacc.c:1646  */
    break;

  case 42:
#line 209 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_NE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));          }
#line 1792 "y.tab.c" /* yacc.c:1646  */
    break;

  case 43:
#line 210 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_EQ, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));          }
#line 1798 "y.tab.c" /* yacc.c:1646  */
    break;

  case 44:
#line 211 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IGT, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1804 "y.tab.c" /* yacc.c:1646  */
    break;

  case 45:
#line 212 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_ILT, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1810 "y.tab.c" /* yacc.c:1646  */
    break;

  case 46:
#line 213 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IGE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1816 "y.tab.c" /* yacc.c:1646  */
    break;

  case 47:
#line 214 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_ILE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1822 "y.tab.c" /* yacc.c:1646  */
    break;

  case 48:
#line 215 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_IEQ, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1828 "y.tab.c" /* yacc.c:1646  */
    break;

  case 49:
#line 216 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_INE, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1834 "y.tab.c" /* yacc.c:1646  */
    break;

  case 50:
#line 217 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[-1].nPtr) ;                            }
#line 1840 "y.tab.c" /* yacc.c:1646  */
    break;

  case 51:
#line 218 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_NOT, 1, (yyvsp[0].nPtr));             }
#line 1846 "y.tab.c" /* yacc.c:1646  */
    break;

  case 52:
#line 219 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_NOT, 1, (yyvsp[0].nPtr));             }
#line 1852 "y.tab.c" /* yacc.c:1646  */
    break;

  case 53:
#line 220 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_INT, 1, (yyvsp[0].nPtr));             }
#line 1858 "y.tab.c" /* yacc.c:1646  */
    break;

  case 54:
#line 221 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_AND, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1864 "y.tab.c" /* yacc.c:1646  */
    break;

  case 55:
#line 222 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_OR, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));          }
#line 1870 "y.tab.c" /* yacc.c:1646  */
    break;

  case 56:
#line 223 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_EOR, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1876 "y.tab.c" /* yacc.c:1646  */
    break;

  case 57:
#line 224 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_EOR, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));         }
#line 1882 "y.tab.c" /* yacc.c:1646  */
    break;

  case 58:
#line 228 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = (yyvsp[0].nPtr) ;                            }
#line 1888 "y.tab.c" /* yacc.c:1646  */
    break;

  case 59:
#line 229 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_EXPR_LIST, 2, (yyvsp[-2].nPtr), (yyvsp[0].nPtr));   }
#line 1894 "y.tab.c" /* yacc.c:1646  */
    break;

  case 60:
#line 233 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_FUNCTION,1,str((yyvsp[-1].sValue))) ;    }
#line 1900 "y.tab.c" /* yacc.c:1646  */
    break;

  case 61:
#line 234 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_FUNCTION,1,str((yyvsp[-2].sValue))) ;    }
#line 1906 "y.tab.c" /* yacc.c:1646  */
    break;

  case 62:
#line 235 "bas.y" /* yacc.c:1646  */
    { (yyval.nPtr) = opr(SC_FUNCTION,2,str((yyvsp[-3].sValue)),(yyvsp[-1].nPtr)) ; }
#line 1912 "y.tab.c" /* yacc.c:1646  */
    break;


#line 1916 "y.tab.c" /* yacc.c:1646  */
      default: break;
    }
  /* User semantic actions sometimes alter yychar, and that requires
     that yytoken be updated with the new translation.  We take the
     approach of translating immediately before every use of yytoken.
     One alternative is translating here after every semantic action,
     but that translation would be missed if the semantic action invokes
     YYABORT, YYACCEPT, or YYERROR immediately after altering yychar or
     if it invokes YYBACKUP.  In the case of YYABORT or YYACCEPT, an
     incorrect destructor might then be invoked immediately.  In the
     case of YYERROR or YYBACKUP, subsequent parser actions might lead
     to an incorrect destructor call or verbose syntax error message
     before the lookahead is translated.  */
  YY_SYMBOL_PRINT ("-> $$ =", yyr1[yyn], &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */

  yyn = yyr1[yyn];

  yystate = yypgoto[yyn - YYNTOKENS] + *yyssp;
  if (0 <= yystate && yystate <= YYLAST && yycheck[yystate] == *yyssp)
    yystate = yytable[yystate];
  else
    yystate = yydefgoto[yyn - YYNTOKENS];

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYEMPTY : YYTRANSLATE (yychar);

  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
#if ! YYERROR_VERBOSE
      yyerror (sTree, YY_("syntax error"));
#else
# define YYSYNTAX_ERROR yysyntax_error (&yymsg_alloc, &yymsg, \
                                        yyssp, yytoken)
      {
        char const *yymsgp = YY_("syntax error");
        int yysyntax_error_status;
        yysyntax_error_status = YYSYNTAX_ERROR;
        if (yysyntax_error_status == 0)
          yymsgp = yymsg;
        else if (yysyntax_error_status == 1)
          {
            if (yymsg != yymsgbuf)
              YYSTACK_FREE (yymsg);
            yymsg = (char *) YYSTACK_ALLOC (yymsg_alloc);
            if (!yymsg)
              {
                yymsg = yymsgbuf;
                yymsg_alloc = sizeof yymsgbuf;
                yysyntax_error_status = 2;
              }
            else
              {
                yysyntax_error_status = YYSYNTAX_ERROR;
                yymsgp = yymsg;
              }
          }
        yyerror (sTree, yymsgp);
        if (yysyntax_error_status == 2)
          goto yyexhaustedlab;
      }
# undef YYSYNTAX_ERROR
#endif
    }



  if (yyerrstatus == 3)
    {
      /* If just tried and failed to reuse lookahead token after an
         error, discard it.  */

      if (yychar <= YYEOF)
        {
          /* Return failure if at end of input.  */
          if (yychar == YYEOF)
            YYABORT;
        }
      else
        {
          yydestruct ("Error: discarding",
                      yytoken, &yylval, sTree);
          yychar = YYEMPTY;
        }
    }

  /* Else will try to reuse lookahead token after shifting the error
     token.  */
  goto yyerrlab1;


/*---------------------------------------------------.
| yyerrorlab -- error raised explicitly by YYERROR.  |
`---------------------------------------------------*/
yyerrorlab:

  /* Pacify compilers like GCC when the user code never invokes
     YYERROR and the label yyerrorlab therefore never appears in user
     code.  */
  if (/*CONSTCOND*/ 0)
     goto yyerrorlab;

  /* Do not reclaim the symbols of the rule whose action triggered
     this YYERROR.  */
  YYPOPSTACK (yylen);
  yylen = 0;
  YY_STACK_PRINT (yyss, yyssp);
  yystate = *yyssp;
  goto yyerrlab1;


/*-------------------------------------------------------------.
| yyerrlab1 -- common code for both syntax error and YYERROR.  |
`-------------------------------------------------------------*/
yyerrlab1:
  yyerrstatus = 3;      /* Each real token shifted decrements this.  */

  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYTERROR;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYTERROR)
            {
              yyn = yytable[yyn];
              if (0 < yyn)
                break;
            }
        }

      /* Pop the current state because it cannot handle the error token.  */
      if (yyssp == yyss)
        YYABORT;


      yydestruct ("Error: popping",
                  yystos[yystate], yyvsp, sTree);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", yystos[yyn], yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturn;

/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturn;

#if !defined yyoverflow || YYERROR_VERBOSE
/*-------------------------------------------------.
| yyexhaustedlab -- memory exhaustion comes here.  |
`-------------------------------------------------*/
yyexhaustedlab:
  yyerror (sTree, YY_("memory exhausted"));
  yyresult = 2;
  /* Fall through.  */
#endif

yyreturn:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval, sTree);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp, sTree);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif
#if YYERROR_VERBOSE
  if (yymsg != yymsgbuf)
    YYSTACK_FREE (yymsg);
#endif
  return yyresult;
}
#line 238 "bas.y" /* yacc.c:1906  */

/*
 *  C Routines used to generate tree
 */
nodeType *con(int value) {
nodeType *p ;
int      ip = 0 ;
/* allocate node */
      if ((p = malloc(sizeof(conNodeType))) == NULL) yyerror(&p,"out of memory");
/* copy information */
        p->type      = typeCon;
        p->con.value = value;
        if(ip){
          printf(" con :: p    = %p\n", (void *)p)   ;
          printf(" con :: type = %i\n", p->type)     ;
          printf(" con :: value= %i\n", p->con.value) ;
        }
       return p;
}

nodeType *id(int i) {
nodeType *p ;
int      ip = 0 ;
/* allocate node */
      if ((p = malloc(sizeof(idNodeType))) == NULL) yyerror(&p,"out of memory");
/* copy information */
      p->type = typeId;
      p->id.i = i;
      if(ip){
        printf(" id  :: p    = %p\n", (void *)p) ;
        printf(" id  :: type = %i\n", p->type)   ;
        printf(" di  :: i    = %i\n", p->id.i)   ;
      }
      return p;
}

nodeType *str(char *name){
nodeType *p ;
int      ip = 0 ;
/* allocate node */
      if ((p = malloc(sizeof(strNodeType))) == NULL) yyerror(&p,"out of memory");
/* copy information */
      p->type     = typeStr      ;
      p->str.name = strdup(name) ;
      if(ip){
        printf(" str :: p    = %p\n", (void *)p)   ;
        printf(" str :: type = %i\n", p->type)     ;
        printf(" str :: name = %s\n", p->str.name) ;
      }
      return p ;
}

nodeType *opr(int oper, int nops, ...) {
va_list   ap  ;
nodeType *p   ;
size_t   size ;
int      i, ip = 0;
/* allocate node */
      size = sizeof(oprNodeType) + (nops - 1) * sizeof(nodeType*);
      if ((p = malloc(size)) == NULL) yyerror(&p,"out of memory");
/* copy information */
      p->type = typeOpr;
      p->opr.oper = oper;
      p->opr.nops = nops;
      va_start(ap, nops);
      for (i = 0; i < nops; i++) p->opr.op[i] = va_arg(ap, nodeType*);
      va_end(ap);
      if(ip){
        printf(" opr :: p    = %p\n", (void *)p)   ;
        printf(" opr :: type = %i\n", p->type)     ;
        printf(" opr :: oper = %i\n", p->opr.oper) ;
        printf(" opr :: nops = %i\n", p->opr.nops) ;
        for (i = 0; i < nops; i++){
          printf(" opr :: %i  %p\n",i,(void *)p->opr.op[i]);
        }
        print_sigscr_node(p) ;
      }
      return p;
}

void freeNode(nodeType *p) {
int i;
      if (!p) return;
      if (p->type == typeOpr) {
        for (i = 0; i < p->opr.nops; i++) freeNode(p->opr.op[i]);
      }
      free (p);
}

void yyerror(nodeType **p, char *s) {
      fprintf(stdout, "yyerror %s\n", s);
}

