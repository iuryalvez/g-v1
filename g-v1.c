/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison implementation for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

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

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

/* All symbols defined below should begin with yy or YY, to avoid
   infringing on user name space.  This should be done even for local
   variables, as they might otherwise be expanded by user macros.
   There are some unavoidable exceptions within include files to
   define necessary library symbols; they are noted "INFRINGES ON
   USER NAME SPACE" below.  */

/* Identify Bison output, and Bison version.  */
#define YYBISON 30802

/* Bison version string.  */
#define YYBISON_VERSION "3.8.2"

/* Skeleton name.  */
#define YYSKELETON_NAME "yacc.c"

/* Pure parsers.  */
#define YYPURE 0

/* Push parsers.  */
#define YYPUSH 0

/* Pull parsers.  */
#define YYPULL 1




/* First part of user prologue.  */
#line 14 "g-v1.y"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ast.h"  /* tipos e funções da AST */

/* Variáveis externas definidas pelo código gerado pelo Flex */
extern int yylineno;   /* número da linha atual no arquivo de entrada */
extern char *yytext;   /* texto do último token reconhecido            */
extern int yylex();    /* função do analisador léxico gerado pelo Flex */
extern FILE *yyin;     /* ponteiro para o arquivo de entrada           */

/* Raiz da AST — preenchida ao final da análise sintática bem-sucedida */
No *raizAST = NULL;

/* Declaração antecipada da função de erro do Bison */
void yyerror(const char *s);

#line 90 "g-v1.c"

# ifndef YY_CAST
#  ifdef __cplusplus
#   define YY_CAST(Type, Val) static_cast<Type> (Val)
#   define YY_REINTERPRET_CAST(Type, Val) reinterpret_cast<Type> (Val)
#  else
#   define YY_CAST(Type, Val) ((Type) (Val))
#   define YY_REINTERPRET_CAST(Type, Val) ((Type) (Val))
#  endif
# endif
# ifndef YY_NULLPTR
#  if defined __cplusplus
#   if 201103L <= __cplusplus
#    define YY_NULLPTR nullptr
#   else
#    define YY_NULLPTR 0
#   endif
#  else
#   define YY_NULLPTR ((void*)0)
#  endif
# endif

#include "g-v1.h"
/* Symbol kind.  */
enum yysymbol_kind_t
{
  YYSYMBOL_YYEMPTY = -2,
  YYSYMBOL_YYEOF = 0,                      /* "end of file"  */
  YYSYMBOL_YYerror = 1,                    /* error  */
  YYSYMBOL_YYUNDEF = 2,                    /* "invalid token"  */
  YYSYMBOL_IDENTIFICADOR = 3,              /* IDENTIFICADOR  */
  YYSYMBOL_CADEIACARACTERES = 4,           /* CADEIACARACTERES  */
  YYSYMBOL_CARCONST = 5,                   /* CARCONST  */
  YYSYMBOL_INTCONST = 6,                   /* INTCONST  */
  YYSYMBOL_PRINCIPAL = 7,                  /* PRINCIPAL  */
  YYSYMBOL_INT = 8,                        /* INT  */
  YYSYMBOL_CAR = 9,                        /* CAR  */
  YYSYMBOL_LEIA = 10,                      /* LEIA  */
  YYSYMBOL_ESCREVA = 11,                   /* ESCREVA  */
  YYSYMBOL_NOVALINHA = 12,                 /* NOVALINHA  */
  YYSYMBOL_ENQUANTO = 13,                  /* ENQUANTO  */
  YYSYMBOL_SE = 14,                        /* SE  */
  YYSYMBOL_ENTAO = 15,                     /* ENTAO  */
  YYSYMBOL_SENAO = 16,                     /* SENAO  */
  YYSYMBOL_FIMSE = 17,                     /* FIMSE  */
  YYSYMBOL_MENORIGUAL = 18,                /* MENORIGUAL  */
  YYSYMBOL_MAIORIGUAL = 19,                /* MAIORIGUAL  */
  YYSYMBOL_IGUAL = 20,                     /* IGUAL  */
  YYSYMBOL_DIFERENTE = 21,                 /* DIFERENTE  */
  YYSYMBOL_E = 22,                         /* E  */
  YYSYMBOL_OU = 23,                        /* OU  */
  YYSYMBOL_24_ = 24,                       /* '{'  */
  YYSYMBOL_25_ = 25,                       /* '}'  */
  YYSYMBOL_26_ = 26,                       /* ':'  */
  YYSYMBOL_27_ = 27,                       /* ';'  */
  YYSYMBOL_28_ = 28,                       /* ','  */
  YYSYMBOL_29_ = 29,                       /* '('  */
  YYSYMBOL_30_ = 30,                       /* ')'  */
  YYSYMBOL_31_ = 31,                       /* '='  */
  YYSYMBOL_32_ = 32,                       /* '<'  */
  YYSYMBOL_33_ = 33,                       /* '>'  */
  YYSYMBOL_34_ = 34,                       /* '+'  */
  YYSYMBOL_35_ = 35,                       /* '-'  */
  YYSYMBOL_36_ = 36,                       /* '*'  */
  YYSYMBOL_37_ = 37,                       /* '/'  */
  YYSYMBOL_38_ = 38,                       /* '!'  */
  YYSYMBOL_YYACCEPT = 39,                  /* $accept  */
  YYSYMBOL_Programa = 40,                  /* Programa  */
  YYSYMBOL_DeclPrograma = 41,              /* DeclPrograma  */
  YYSYMBOL_Bloco = 42,                     /* Bloco  */
  YYSYMBOL_VarSection = 43,                /* VarSection  */
  YYSYMBOL_ListaDeclVar = 44,              /* ListaDeclVar  */
  YYSYMBOL_DeclVar = 45,                   /* DeclVar  */
  YYSYMBOL_Tipo = 46,                      /* Tipo  */
  YYSYMBOL_ListaComando = 47,              /* ListaComando  */
  YYSYMBOL_Comando = 48,                   /* Comando  */
  YYSYMBOL_Expr = 49,                      /* Expr  */
  YYSYMBOL_OrExpr = 50,                    /* OrExpr  */
  YYSYMBOL_AndExpr = 51,                   /* AndExpr  */
  YYSYMBOL_EqExpr = 52,                    /* EqExpr  */
  YYSYMBOL_DesigExpr = 53,                 /* DesigExpr  */
  YYSYMBOL_AddExpr = 54,                   /* AddExpr  */
  YYSYMBOL_MulExpr = 55,                   /* MulExpr  */
  YYSYMBOL_UnExpr = 56,                    /* UnExpr  */
  YYSYMBOL_PrimExpr = 57                   /* PrimExpr  */
};
typedef enum yysymbol_kind_t yysymbol_kind_t;




#ifdef short
# undef short
#endif

/* On compilers that do not define __PTRDIFF_MAX__ etc., make sure
   <limits.h> and (if available) <stdint.h> are included
   so that the code can choose integer types of a good width.  */

#ifndef __PTRDIFF_MAX__
# include <limits.h> /* INFRINGES ON USER NAME SPACE */
# if defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stdint.h> /* INFRINGES ON USER NAME SPACE */
#  define YY_STDINT_H
# endif
#endif

/* Narrow types that promote to a signed type and that can represent a
   signed or unsigned integer of at least N bits.  In tables they can
   save space and decrease cache pressure.  Promoting to a signed type
   helps avoid bugs in integer arithmetic.  */

#ifdef __INT_LEAST8_MAX__
typedef __INT_LEAST8_TYPE__ yytype_int8;
#elif defined YY_STDINT_H
typedef int_least8_t yytype_int8;
#else
typedef signed char yytype_int8;
#endif

#ifdef __INT_LEAST16_MAX__
typedef __INT_LEAST16_TYPE__ yytype_int16;
#elif defined YY_STDINT_H
typedef int_least16_t yytype_int16;
#else
typedef short yytype_int16;
#endif

/* Work around bug in HP-UX 11.23, which defines these macros
   incorrectly for preprocessor constants.  This workaround can likely
   be removed in 2023, as HPE has promised support for HP-UX 11.23
   (aka HP-UX 11i v2) only through the end of 2022; see Table 2 of
   <https://h20195.www2.hpe.com/V2/getpdf.aspx/4AA4-7673ENW.pdf>.  */
#ifdef __hpux
# undef UINT_LEAST8_MAX
# undef UINT_LEAST16_MAX
# define UINT_LEAST8_MAX 255
# define UINT_LEAST16_MAX 65535
#endif

#if defined __UINT_LEAST8_MAX__ && __UINT_LEAST8_MAX__ <= __INT_MAX__
typedef __UINT_LEAST8_TYPE__ yytype_uint8;
#elif (!defined __UINT_LEAST8_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST8_MAX <= INT_MAX)
typedef uint_least8_t yytype_uint8;
#elif !defined __UINT_LEAST8_MAX__ && UCHAR_MAX <= INT_MAX
typedef unsigned char yytype_uint8;
#else
typedef short yytype_uint8;
#endif

#if defined __UINT_LEAST16_MAX__ && __UINT_LEAST16_MAX__ <= __INT_MAX__
typedef __UINT_LEAST16_TYPE__ yytype_uint16;
#elif (!defined __UINT_LEAST16_MAX__ && defined YY_STDINT_H \
       && UINT_LEAST16_MAX <= INT_MAX)
typedef uint_least16_t yytype_uint16;
#elif !defined __UINT_LEAST16_MAX__ && USHRT_MAX <= INT_MAX
typedef unsigned short yytype_uint16;
#else
typedef int yytype_uint16;
#endif

#ifndef YYPTRDIFF_T
# if defined __PTRDIFF_TYPE__ && defined __PTRDIFF_MAX__
#  define YYPTRDIFF_T __PTRDIFF_TYPE__
#  define YYPTRDIFF_MAXIMUM __PTRDIFF_MAX__
# elif defined PTRDIFF_MAX
#  ifndef ptrdiff_t
#   include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  endif
#  define YYPTRDIFF_T ptrdiff_t
#  define YYPTRDIFF_MAXIMUM PTRDIFF_MAX
# else
#  define YYPTRDIFF_T long
#  define YYPTRDIFF_MAXIMUM LONG_MAX
# endif
#endif

#ifndef YYSIZE_T
# ifdef __SIZE_TYPE__
#  define YYSIZE_T __SIZE_TYPE__
# elif defined size_t
#  define YYSIZE_T size_t
# elif defined __STDC_VERSION__ && 199901 <= __STDC_VERSION__
#  include <stddef.h> /* INFRINGES ON USER NAME SPACE */
#  define YYSIZE_T size_t
# else
#  define YYSIZE_T unsigned
# endif
#endif

#define YYSIZE_MAXIMUM                                  \
  YY_CAST (YYPTRDIFF_T,                                 \
           (YYPTRDIFF_MAXIMUM < YY_CAST (YYSIZE_T, -1)  \
            ? YYPTRDIFF_MAXIMUM                         \
            : YY_CAST (YYSIZE_T, -1)))

#define YYSIZEOF(X) YY_CAST (YYPTRDIFF_T, sizeof (X))


/* Stored state numbers (used for stacks). */
typedef yytype_int8 yy_state_t;

/* State numbers in computations.  */
typedef int yy_state_fast_t;

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


#ifndef YY_ATTRIBUTE_PURE
# if defined __GNUC__ && 2 < __GNUC__ + (96 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_PURE __attribute__ ((__pure__))
# else
#  define YY_ATTRIBUTE_PURE
# endif
#endif

#ifndef YY_ATTRIBUTE_UNUSED
# if defined __GNUC__ && 2 < __GNUC__ + (7 <= __GNUC_MINOR__)
#  define YY_ATTRIBUTE_UNUSED __attribute__ ((__unused__))
# else
#  define YY_ATTRIBUTE_UNUSED
# endif
#endif

/* Suppress unused-variable warnings by "using" E.  */
#if ! defined lint || defined __GNUC__
# define YY_USE(E) ((void) (E))
#else
# define YY_USE(E) /* empty */
#endif

/* Suppress an incorrect diagnostic about yylval being uninitialized.  */
#if defined __GNUC__ && ! defined __ICC && 406 <= __GNUC__ * 100 + __GNUC_MINOR__
# if __GNUC__ * 100 + __GNUC_MINOR__ < 407
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")
# else
#  define YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN                           \
    _Pragma ("GCC diagnostic push")                                     \
    _Pragma ("GCC diagnostic ignored \"-Wuninitialized\"")              \
    _Pragma ("GCC diagnostic ignored \"-Wmaybe-uninitialized\"")
# endif
# define YY_IGNORE_MAYBE_UNINITIALIZED_END      \
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

#if defined __cplusplus && defined __GNUC__ && ! defined __ICC && 6 <= __GNUC__
# define YY_IGNORE_USELESS_CAST_BEGIN                          \
    _Pragma ("GCC diagnostic push")                            \
    _Pragma ("GCC diagnostic ignored \"-Wuseless-cast\"")
# define YY_IGNORE_USELESS_CAST_END            \
    _Pragma ("GCC diagnostic pop")
#endif
#ifndef YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_BEGIN
# define YY_IGNORE_USELESS_CAST_END
#endif


#define YY_ASSERT(E) ((void) (0 && (E)))

#if !defined yyoverflow

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
#endif /* !defined yyoverflow */

#if (! defined yyoverflow \
     && (! defined __cplusplus \
         || (defined YYSTYPE_IS_TRIVIAL && YYSTYPE_IS_TRIVIAL)))

/* A type that is properly aligned for any stack member.  */
union yyalloc
{
  yy_state_t yyss_alloc;
  YYSTYPE yyvs_alloc;
};

/* The size of the maximum gap between one aligned stack and the next.  */
# define YYSTACK_GAP_MAXIMUM (YYSIZEOF (union yyalloc) - 1)

/* The size of an array large to enough to hold all stacks, each with
   N elements.  */
# define YYSTACK_BYTES(N) \
     ((N) * (YYSIZEOF (yy_state_t) + YYSIZEOF (YYSTYPE)) \
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
        YYPTRDIFF_T yynewbytes;                                         \
        YYCOPY (&yyptr->Stack_alloc, Stack, yysize);                    \
        Stack = &yyptr->Stack_alloc;                                    \
        yynewbytes = yystacksize * YYSIZEOF (*Stack) + YYSTACK_GAP_MAXIMUM; \
        yyptr += yynewbytes / YYSIZEOF (*yyptr);                        \
      }                                                                 \
    while (0)

#endif

#if defined YYCOPY_NEEDED && YYCOPY_NEEDED
/* Copy COUNT objects from SRC to DST.  The source and destination do
   not overlap.  */
# ifndef YYCOPY
#  if defined __GNUC__ && 1 < __GNUC__
#   define YYCOPY(Dst, Src, Count) \
      __builtin_memcpy (Dst, Src, YY_CAST (YYSIZE_T, (Count)) * sizeof (*(Src)))
#  else
#   define YYCOPY(Dst, Src, Count)              \
      do                                        \
        {                                       \
          YYPTRDIFF_T yyi;                      \
          for (yyi = 0; yyi < (Count); yyi++)   \
            (Dst)[yyi] = (Src)[yyi];            \
        }                                       \
      while (0)
#  endif
# endif
#endif /* !YYCOPY_NEEDED */

/* YYFINAL -- State number of the termination state.  */
#define YYFINAL  7
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   121

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  39
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  19
/* YYNRULES -- Number of rules.  */
#define YYNRULES  51
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  103

/* YYMAXUTOK -- Last valid token kind.  */
#define YYMAXUTOK   278


/* YYTRANSLATE(TOKEN-NUM) -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, with out-of-bounds checking.  */
#define YYTRANSLATE(YYX)                                \
  (0 <= (YYX) && (YYX) <= YYMAXUTOK                     \
   ? YY_CAST (yysymbol_kind_t, yytranslate[YYX])        \
   : YYSYMBOL_YYUNDEF)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex.  */
static const yytype_int8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    38,     2,     2,     2,     2,     2,     2,
      29,    30,    36,    34,    28,    35,     2,    37,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    26,    27,
      32,    31,    33,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,    24,     2,    25,     2,     2,     2,     2,
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
      15,    16,    17,    18,    19,    20,    21,    22,    23
};

#if YYDEBUG
/* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_int16 yyrline[] =
{
       0,    68,    68,    77,    87,    89,    98,   113,   122,   139,
     140,   154,   155,   163,   165,   174,   178,   182,   190,   194,
     202,   207,   212,   217,   221,   232,   234,   247,   249,   257,
     259,   267,   269,   271,   279,   281,   283,   285,   287,   295,
     297,   299,   307,   309,   311,   320,   322,   324,   332,   334,
     336,   338
};
#endif

/** Accessing symbol of state STATE.  */
#define YY_ACCESSING_SYMBOL(State) YY_CAST (yysymbol_kind_t, yystos[State])

#if YYDEBUG || 0
/* The user-facing name of the symbol whose (internal) number is
   YYSYMBOL.  No bounds checking.  */
static const char *yysymbol_name (yysymbol_kind_t yysymbol) YY_ATTRIBUTE_UNUSED;

/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "\"end of file\"", "error", "\"invalid token\"", "IDENTIFICADOR",
  "CADEIACARACTERES", "CARCONST", "INTCONST", "PRINCIPAL", "INT", "CAR",
  "LEIA", "ESCREVA", "NOVALINHA", "ENQUANTO", "SE", "ENTAO", "SENAO",
  "FIMSE", "MENORIGUAL", "MAIORIGUAL", "IGUAL", "DIFERENTE", "E", "OU",
  "'{'", "'}'", "':'", "';'", "','", "'('", "')'", "'='", "'<'", "'>'",
  "'+'", "'-'", "'*'", "'/'", "'!'", "$accept", "Programa", "DeclPrograma",
  "Bloco", "VarSection", "ListaDeclVar", "DeclVar", "Tipo", "ListaComando",
  "Comando", "Expr", "OrExpr", "AndExpr", "EqExpr", "DesigExpr", "AddExpr",
  "MulExpr", "UnExpr", "PrimExpr", YY_NULLPTR
};

static const char *
yysymbol_name (yysymbol_kind_t yysymbol)
{
  return yytname[yysymbol];
}
#endif

#define YYPACT_NINF (-24)

#define yypact_value_is_default(Yyn) \
  ((Yyn) == YYPACT_NINF)

#define YYTABLE_NINF (-10)

#define yytable_value_is_error(Yyn) \
  0

/* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
   STATE-NUM.  */
static const yytype_int8 yypact[] =
{
      -6,   -18,    12,   -24,    -3,   -24,    -7,   -24,    52,   -24,
     -24,    37,    10,    -2,    36,    59,   -24,    31,    48,    48,
     -24,    56,    66,    17,    67,    76,    78,    22,   -14,    24,
      25,   -24,   -24,    17,    98,    31,    77,    75,    73,    80,
      81,   -24,    31,    31,    82,   -24,   -24,   -24,   -24,   -24,
     -24,   -24,    44,    44,    44,    44,    44,    44,    44,    44,
      44,    44,    44,    44,    84,    83,   -24,    62,   -24,   -24,
     -24,    85,    86,   -24,    78,    22,   -14,   -14,    24,    24,
      24,    24,    25,    25,   -24,   -24,   -24,   -24,   -24,   -24,
      87,    17,    90,   103,   -24,    17,    83,   -24,    58,    17,
     -24,    93,   -24
};

/* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
   Performed when YYTABLE does not specify something else to do.  Zero
   means the default is an error.  */
static const yytype_int8 yydefact[] =
{
       0,     0,     0,     2,     0,     3,     0,     1,    48,    49,
      50,     0,     0,     0,     0,     0,    15,     0,     0,     0,
      24,     0,     0,    13,     0,    25,    28,    30,    33,    38,
      41,    44,    47,     0,     0,     0,     0,     0,    48,     0,
       0,    20,     0,     0,     0,    48,    45,    46,     6,     4,
      14,    16,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     9,    26,     0,    17,    19,
      18,     0,     0,    51,    27,    29,    31,    32,    37,    36,
      34,    35,    39,    40,    42,    43,     5,    10,    11,    12,
       0,     0,     0,     8,    23,     0,     9,     7,     0,     0,
      21,     0,    22
};

/* YYPGOTO[NTERM-NUM].  */
static const yytype_int8 yypgoto[] =
{
     -24,   -24,   -24,   112,   -24,    26,    53,   -24,    34,   -23,
      21,   -24,    65,    68,    35,    28,    32,    33,    79
};

/* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int8 yydefgoto[] =
{
       0,     2,     3,    20,     6,    21,    36,    90,    22,    23,
      24,    25,    26,    27,    28,    29,    30,    31,    32
};

/* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
   positive, shift that token.  If negative, reduce the rule whose
   number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int8 yytable[] =
{
       8,     1,     9,    10,    56,    57,     4,    11,    12,    13,
      14,    15,     7,    38,    39,     9,    10,    33,    58,    59,
      38,     4,     9,    10,    16,    41,    17,    11,    12,    13,
      14,    15,    18,    40,    38,    19,     9,    10,    44,    17,
      37,     4,    54,    55,    16,    18,    17,    45,    19,     9,
      10,    45,    18,     9,    10,    19,    66,    50,    60,    61,
      17,    62,    63,    71,    72,    42,    18,    64,    94,    19,
      88,    89,    98,    17,    99,   100,   101,    17,    -9,    18,
      34,    48,    19,    35,    78,    79,    80,    81,    43,    76,
      77,    49,    82,    83,    51,    84,    85,    46,    47,    52,
      53,    65,    68,    67,    35,    95,    96,    69,    70,    86,
     102,    34,    73,     5,    93,    91,    92,    74,    87,    97,
       0,    75
};

static const yytype_int8 yycheck[] =
{
       3,     7,     5,     6,    18,    19,    24,    10,    11,    12,
      13,    14,     0,     3,     4,     5,     6,    24,    32,    33,
       3,    24,     5,     6,    27,    27,    29,    10,    11,    12,
      13,    14,    35,    12,     3,    38,     5,     6,    17,    29,
       3,    24,    20,    21,    27,    35,    29,     3,    38,     5,
       6,     3,    35,     5,     6,    38,    35,    23,    34,    35,
      29,    36,    37,    42,    43,    29,    35,    33,    91,    38,
       8,     9,    95,    29,    16,    17,    99,    29,    26,    35,
      28,    25,    38,    31,    56,    57,    58,    59,    29,    54,
      55,    25,    60,    61,    27,    62,    63,    18,    19,    23,
      22,     3,    27,    26,    31,    15,     3,    27,    27,    25,
      17,    28,    30,     1,    27,    30,    30,    52,    65,    93,
      -1,    53
};

/* YYSTOS[STATE-NUM] -- The symbol kind of the accessing symbol of
   state STATE-NUM.  */
static const yytype_int8 yystos[] =
{
       0,     7,    40,    41,    24,    42,    43,     0,     3,     5,
       6,    10,    11,    12,    13,    14,    27,    29,    35,    38,
      42,    44,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    24,    28,    31,    45,     3,     3,     4,
      49,    27,    29,    29,    49,     3,    57,    57,    25,    25,
      47,    27,    23,    22,    20,    21,    18,    19,    32,    33,
      34,    35,    36,    37,    47,     3,    49,    26,    27,    27,
      27,    49,    49,    30,    51,    52,    53,    53,    54,    54,
      54,    54,    55,    55,    56,    56,    25,    45,     8,     9,
      46,    30,    30,    27,    48,    15,     3,    44,    48,    16,
      17,    48,    17
};

/* YYR1[RULE-NUM] -- Symbol kind of the left-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr1[] =
{
       0,    39,    40,    41,    42,    42,    43,    44,    44,    45,
      45,    46,    46,    47,    47,    48,    48,    48,    48,    48,
      48,    48,    48,    48,    48,    49,    49,    50,    50,    51,
      51,    52,    52,    52,    53,    53,    53,    53,    53,    54,
      54,    54,    55,    55,    55,    56,    56,    56,    57,    57,
      57,    57
};

/* YYR2[RULE-NUM] -- Number of symbols on the right-hand side of rule RULE-NUM.  */
static const yytype_int8 yyr2[] =
{
       0,     2,     1,     2,     3,     4,     3,     6,     5,     0,
       3,     1,     1,     1,     2,     1,     2,     3,     3,     3,
       2,     7,     9,     5,     1,     1,     3,     3,     1,     3,
       1,     3,     3,     1,     3,     3,     3,     3,     1,     3,
       3,     1,     3,     3,     1,     2,     2,     1,     1,     1,
       1,     3
};


enum { YYENOMEM = -2 };

#define yyerrok         (yyerrstatus = 0)
#define yyclearin       (yychar = YYEMPTY)

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYNOMEM         goto yyexhaustedlab


#define YYRECOVERING()  (!!yyerrstatus)

#define YYBACKUP(Token, Value)                                    \
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
        yyerror (YY_("syntax error: cannot back up")); \
        YYERROR;                                                  \
      }                                                           \
  while (0)

/* Backward compatibility with an undocumented macro.
   Use YYerror or YYUNDEF. */
#define YYERRCODE YYUNDEF


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




# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)                    \
do {                                                                      \
  if (yydebug)                                                            \
    {                                                                     \
      YYFPRINTF (stderr, "%s ", Title);                                   \
      yy_symbol_print (stderr,                                            \
                  Kind, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*-----------------------------------.
| Print this symbol's value on YYO.  |
`-----------------------------------*/

static void
yy_symbol_value_print (FILE *yyo,
                       yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  FILE *yyoutput = yyo;
  YY_USE (yyoutput);
  if (!yyvaluep)
    return;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/*---------------------------.
| Print this symbol on YYO.  |
`---------------------------*/

static void
yy_symbol_print (FILE *yyo,
                 yysymbol_kind_t yykind, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyo, "%s %s (",
             yykind < YYNTOKENS ? "token" : "nterm", yysymbol_name (yykind));

  yy_symbol_value_print (yyo, yykind, yyvaluep);
  YYFPRINTF (yyo, ")");
}

/*------------------------------------------------------------------.
| yy_stack_print -- Print the state stack from its BOTTOM up to its |
| TOP (included).                                                   |
`------------------------------------------------------------------*/

static void
yy_stack_print (yy_state_t *yybottom, yy_state_t *yytop)
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
yy_reduce_print (yy_state_t *yyssp, YYSTYPE *yyvsp,
                 int yyrule)
{
  int yylno = yyrline[yyrule];
  int yynrhs = yyr2[yyrule];
  int yyi;
  YYFPRINTF (stderr, "Reducing stack by rule %d (line %d):\n",
             yyrule - 1, yylno);
  /* The symbols being reduced.  */
  for (yyi = 0; yyi < yynrhs; yyi++)
    {
      YYFPRINTF (stderr, "   $%d = ", yyi + 1);
      yy_symbol_print (stderr,
                       YY_ACCESSING_SYMBOL (+yyssp[yyi + 1 - yynrhs]),
                       &yyvsp[(yyi + 1) - (yynrhs)]);
      YYFPRINTF (stderr, "\n");
    }
}

# define YY_REDUCE_PRINT(Rule)          \
do {                                    \
  if (yydebug)                          \
    yy_reduce_print (yyssp, yyvsp, Rule); \
} while (0)

/* Nonzero means print parse trace.  It is left uninitialized so that
   multiple parsers can coexist.  */
int yydebug;
#else /* !YYDEBUG */
# define YYDPRINTF(Args) ((void) 0)
# define YY_SYMBOL_PRINT(Title, Kind, Value, Location)
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






/*-----------------------------------------------.
| Release the memory associated to this symbol.  |
`-----------------------------------------------*/

static void
yydestruct (const char *yymsg,
            yysymbol_kind_t yykind, YYSTYPE *yyvaluep)
{
  YY_USE (yyvaluep);
  if (!yymsg)
    yymsg = "Deleting";
  YY_SYMBOL_PRINT (yymsg, yykind, yyvaluep, yylocationp);

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  YY_USE (yykind);
  YY_IGNORE_MAYBE_UNINITIALIZED_END
}


/* Lookahead token kind.  */
int yychar;

/* The semantic value of the lookahead symbol.  */
YYSTYPE yylval;
/* Number of syntax errors so far.  */
int yynerrs;




/*----------.
| yyparse.  |
`----------*/

int
yyparse (void)
{
    yy_state_fast_t yystate = 0;
    /* Number of tokens to shift before error messages enabled.  */
    int yyerrstatus = 0;

    /* Refer to the stacks through separate pointers, to allow yyoverflow
       to reallocate them elsewhere.  */

    /* Their size.  */
    YYPTRDIFF_T yystacksize = YYINITDEPTH;

    /* The state stack: array, bottom, top.  */
    yy_state_t yyssa[YYINITDEPTH];
    yy_state_t *yyss = yyssa;
    yy_state_t *yyssp = yyss;

    /* The semantic value stack: array, bottom, top.  */
    YYSTYPE yyvsa[YYINITDEPTH];
    YYSTYPE *yyvs = yyvsa;
    YYSTYPE *yyvsp = yyvs;

  int yyn;
  /* The return value of yyparse.  */
  int yyresult;
  /* Lookahead symbol kind.  */
  yysymbol_kind_t yytoken = YYSYMBOL_YYEMPTY;
  /* The variables used to return semantic value and location from the
     action routines.  */
  YYSTYPE yyval;



#define YYPOPSTACK(N)   (yyvsp -= (N), yyssp -= (N))

  /* The number of symbols on the RHS of the reduced rule.
     Keep to zero when no symbol should be popped.  */
  int yylen = 0;

  YYDPRINTF ((stderr, "Starting parse\n"));

  yychar = YYEMPTY; /* Cause a token to be read.  */

  goto yysetstate;


/*------------------------------------------------------------.
| yynewstate -- push a new state, which is found in yystate.  |
`------------------------------------------------------------*/
yynewstate:
  /* In all cases, when you get here, the value and location stacks
     have just been pushed.  So pushing a state here evens the stacks.  */
  yyssp++;


/*--------------------------------------------------------------------.
| yysetstate -- set current state (the top of the stack) to yystate.  |
`--------------------------------------------------------------------*/
yysetstate:
  YYDPRINTF ((stderr, "Entering state %d\n", yystate));
  YY_ASSERT (0 <= yystate && yystate < YYNSTATES);
  YY_IGNORE_USELESS_CAST_BEGIN
  *yyssp = YY_CAST (yy_state_t, yystate);
  YY_IGNORE_USELESS_CAST_END
  YY_STACK_PRINT (yyss, yyssp);

  if (yyss + yystacksize - 1 <= yyssp)
#if !defined yyoverflow && !defined YYSTACK_RELOCATE
    YYNOMEM;
#else
    {
      /* Get the current used size of the three stacks, in elements.  */
      YYPTRDIFF_T yysize = yyssp - yyss + 1;

# if defined yyoverflow
      {
        /* Give user a chance to reallocate the stack.  Use copies of
           these so that the &'s don't force the real ones into
           memory.  */
        yy_state_t *yyss1 = yyss;
        YYSTYPE *yyvs1 = yyvs;

        /* Each stack pointer address is followed by the size of the
           data in use in that stack, in bytes.  This used to be a
           conditional around just the two extra args, but that might
           be undefined if yyoverflow is a macro.  */
        yyoverflow (YY_("memory exhausted"),
                    &yyss1, yysize * YYSIZEOF (*yyssp),
                    &yyvs1, yysize * YYSIZEOF (*yyvsp),
                    &yystacksize);
        yyss = yyss1;
        yyvs = yyvs1;
      }
# else /* defined YYSTACK_RELOCATE */
      /* Extend the stack our own way.  */
      if (YYMAXDEPTH <= yystacksize)
        YYNOMEM;
      yystacksize *= 2;
      if (YYMAXDEPTH < yystacksize)
        yystacksize = YYMAXDEPTH;

      {
        yy_state_t *yyss1 = yyss;
        union yyalloc *yyptr =
          YY_CAST (union yyalloc *,
                   YYSTACK_ALLOC (YY_CAST (YYSIZE_T, YYSTACK_BYTES (yystacksize))));
        if (! yyptr)
          YYNOMEM;
        YYSTACK_RELOCATE (yyss_alloc, yyss);
        YYSTACK_RELOCATE (yyvs_alloc, yyvs);
#  undef YYSTACK_RELOCATE
        if (yyss1 != yyssa)
          YYSTACK_FREE (yyss1);
      }
# endif

      yyssp = yyss + yysize - 1;
      yyvsp = yyvs + yysize - 1;

      YY_IGNORE_USELESS_CAST_BEGIN
      YYDPRINTF ((stderr, "Stack size increased to %ld\n",
                  YY_CAST (long, yystacksize)));
      YY_IGNORE_USELESS_CAST_END

      if (yyss + yystacksize - 1 <= yyssp)
        YYABORT;
    }
#endif /* !defined yyoverflow && !defined YYSTACK_RELOCATE */


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

  /* YYCHAR is either empty, or end-of-input, or a valid lookahead.  */
  if (yychar == YYEMPTY)
    {
      YYDPRINTF ((stderr, "Reading a token\n"));
      yychar = yylex ();
    }

  if (yychar <= YYEOF)
    {
      yychar = YYEOF;
      yytoken = YYSYMBOL_YYEOF;
      YYDPRINTF ((stderr, "Now at end of input.\n"));
    }
  else if (yychar == YYerror)
    {
      /* The scanner already issued an error message, process directly
         to error recovery.  But do not keep the error token as
         lookahead, it is too special and may lead us to an endless
         loop in error recovery. */
      yychar = YYUNDEF;
      yytoken = YYSYMBOL_YYerror;
      goto yyerrlab1;
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
  yystate = yyn;
  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END

  /* Discard the shifted token.  */
  yychar = YYEMPTY;
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
| yyreduce -- do a reduction.  |
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
  case 2: /* Programa: DeclPrograma  */
#line 69 "g-v1.y"
        { raizAST = novoNo1(AST_PROGRAMA, yylineno, (yyvsp[0].no)); (yyval.no) = raizAST; }
#line 1200 "g-v1.c"
    break;

  case 3: /* DeclPrograma: PRINCIPAL Bloco  */
#line 78 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1206 "g-v1.c"
    break;

  case 4: /* Bloco: '{' ListaComando '}'  */
#line 88 "g-v1.y"
        { (yyval.no) = novoNo2(AST_BLOCO, yylineno, NULL, (yyvsp[-1].no)); }
#line 1212 "g-v1.c"
    break;

  case 5: /* Bloco: VarSection '{' ListaComando '}'  */
#line 90 "g-v1.y"
        { (yyval.no) = novoNo2(AST_BLOCO, yylineno, (yyvsp[-3].no), (yyvsp[-1].no)); }
#line 1218 "g-v1.c"
    break;

  case 6: /* VarSection: '{' ListaDeclVar '}'  */
#line 99 "g-v1.y"
        { (yyval.no) = (yyvsp[-1].no); }
#line 1224 "g-v1.c"
    break;

  case 7: /* ListaDeclVar: IDENTIFICADOR DeclVar ':' Tipo ';' ListaDeclVar  */
#line 114 "g-v1.y"
        {
            /* Nó DECL_VAR: filho1 = nome, filho2 = tipo, filho3 = extras (vírgula) */
            No *decl = novoNo2(AST_DECL_VAR, yylineno,
                               novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[-5].str)), (yyvsp[-2].no));
            decl->filho3 = (yyvsp[-4].no); /* variáveis extras separadas por vírgula */
            (yyval.no) = novoNo2(AST_LISTA_DECL_VAR, yylineno, decl, (yyvsp[0].no));
            free((yyvsp[-5].str)); /* libera a string copiada pelo Flex */
        }
#line 1237 "g-v1.c"
    break;

  case 8: /* ListaDeclVar: IDENTIFICADOR DeclVar ':' Tipo ';'  */
#line 123 "g-v1.y"
        {
            No *decl = novoNo2(AST_DECL_VAR, yylineno,
                               novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[-4].str)), (yyvsp[-1].no));
            decl->filho3 = (yyvsp[-3].no);
            (yyval.no) = novoNo1(AST_LISTA_DECL_VAR, yylineno, decl);
            free((yyvsp[-4].str));
        }
#line 1249 "g-v1.c"
    break;

  case 9: /* DeclVar: %empty  */
#line 139 "g-v1.y"
        { (yyval.no) = NULL; }
#line 1255 "g-v1.c"
    break;

  case 10: /* DeclVar: ',' IDENTIFICADOR DeclVar  */
#line 141 "g-v1.y"
        {
            /* cria nó para este identificador extra e encadeia com o próximo */
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[-1].str));
            (yyval.no) = novoNo2(AST_DECL_VAR, yylineno, id, (yyvsp[0].no));
            free((yyvsp[-1].str));
        }
#line 1266 "g-v1.c"
    break;

  case 11: /* Tipo: INT  */
#line 154 "g-v1.y"
           { (yyval.no) = novoNo(AST_TIPO_INT, yylineno, NULL); }
#line 1272 "g-v1.c"
    break;

  case 12: /* Tipo: CAR  */
#line 155 "g-v1.y"
           { (yyval.no) = novoNo(AST_TIPO_CAR, yylineno, NULL); }
#line 1278 "g-v1.c"
    break;

  case 13: /* ListaComando: Comando  */
#line 164 "g-v1.y"
        { (yyval.no) = novoNo1(AST_LISTA_COMANDO, yylineno, (yyvsp[0].no)); }
#line 1284 "g-v1.c"
    break;

  case 14: /* ListaComando: Comando ListaComando  */
#line 166 "g-v1.y"
        { (yyval.no) = novoNo2(AST_LISTA_COMANDO, yylineno, (yyvsp[-1].no), (yyvsp[0].no)); }
#line 1290 "g-v1.c"
    break;

  case 15: /* Comando: ';'  */
#line 175 "g-v1.y"
        { (yyval.no) = novoNo(AST_CMD_VAZIO, yylineno, NULL); }
#line 1296 "g-v1.c"
    break;

  case 16: /* Comando: Expr ';'  */
#line 179 "g-v1.y"
        { (yyval.no) = novoNo1(AST_CMD_EXPR, yylineno, (yyvsp[-1].no)); }
#line 1302 "g-v1.c"
    break;

  case 17: /* Comando: LEIA IDENTIFICADOR ';'  */
#line 183 "g-v1.y"
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[-1].str));
            (yyval.no) = novoNo1(AST_CMD_LEIA, yylineno, id);
            free((yyvsp[-1].str));
        }
#line 1312 "g-v1.c"
    break;

  case 18: /* Comando: ESCREVA Expr ';'  */
#line 191 "g-v1.y"
        { (yyval.no) = novoNo1(AST_CMD_ESCREVA_EXPR, yylineno, (yyvsp[-1].no)); }
#line 1318 "g-v1.c"
    break;

  case 19: /* Comando: ESCREVA CADEIACARACTERES ';'  */
#line 195 "g-v1.y"
        {
            /* o valor da string é guardado diretamente no nó (nó folha) */
            (yyval.no) = novoNo(AST_CMD_ESCREVA_STR, yylineno, (yyvsp[-1].str));
            free((yyvsp[-1].str));
        }
#line 1328 "g-v1.c"
    break;

  case 20: /* Comando: NOVALINHA ';'  */
#line 203 "g-v1.y"
        { (yyval.no) = novoNo(AST_CMD_NOVALINHA, yylineno, NULL); }
#line 1334 "g-v1.c"
    break;

  case 21: /* Comando: SE '(' Expr ')' ENTAO Comando FIMSE  */
#line 208 "g-v1.y"
        { (yyval.no) = novoNo2(AST_CMD_SE, yylineno, (yyvsp[-4].no), (yyvsp[-1].no)); }
#line 1340 "g-v1.c"
    break;

  case 22: /* Comando: SE '(' Expr ')' ENTAO Comando SENAO Comando FIMSE  */
#line 213 "g-v1.y"
        { (yyval.no) = novoNo3(AST_CMD_SE_SENAO, yylineno, (yyvsp[-6].no), (yyvsp[-3].no), (yyvsp[-1].no)); }
#line 1346 "g-v1.c"
    break;

  case 23: /* Comando: ENQUANTO '(' Expr ')' Comando  */
#line 218 "g-v1.y"
        { (yyval.no) = novoNo2(AST_CMD_ENQUANTO, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1352 "g-v1.c"
    break;

  case 24: /* Comando: Bloco  */
#line 222 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1358 "g-v1.c"
    break;

  case 25: /* Expr: OrExpr  */
#line 233 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1364 "g-v1.c"
    break;

  case 26: /* Expr: IDENTIFICADOR '=' Expr  */
#line 235 "g-v1.y"
        {
            No *id = novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[-2].str));
            (yyval.no) = novoNo2(AST_ATRIB, yylineno, id, (yyvsp[0].no));
            free((yyvsp[-2].str));
        }
#line 1374 "g-v1.c"
    break;

  case 27: /* OrExpr: OrExpr OU AndExpr  */
#line 248 "g-v1.y"
        { (yyval.no) = novoNo2(AST_OU, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1380 "g-v1.c"
    break;

  case 28: /* OrExpr: AndExpr  */
#line 250 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1386 "g-v1.c"
    break;

  case 29: /* AndExpr: AndExpr E EqExpr  */
#line 258 "g-v1.y"
        { (yyval.no) = novoNo2(AST_E, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1392 "g-v1.c"
    break;

  case 30: /* AndExpr: EqExpr  */
#line 260 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1398 "g-v1.c"
    break;

  case 31: /* EqExpr: EqExpr IGUAL DesigExpr  */
#line 268 "g-v1.y"
        { (yyval.no) = novoNo2(AST_IGUAL, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1404 "g-v1.c"
    break;

  case 32: /* EqExpr: EqExpr DIFERENTE DesigExpr  */
#line 270 "g-v1.y"
        { (yyval.no) = novoNo2(AST_DIFERENTE, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1410 "g-v1.c"
    break;

  case 33: /* EqExpr: DesigExpr  */
#line 272 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1416 "g-v1.c"
    break;

  case 34: /* DesigExpr: DesigExpr '<' AddExpr  */
#line 280 "g-v1.y"
        { (yyval.no) = novoNo2(AST_MENOR, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1422 "g-v1.c"
    break;

  case 35: /* DesigExpr: DesigExpr '>' AddExpr  */
#line 282 "g-v1.y"
        { (yyval.no) = novoNo2(AST_MAIOR, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1428 "g-v1.c"
    break;

  case 36: /* DesigExpr: DesigExpr MAIORIGUAL AddExpr  */
#line 284 "g-v1.y"
        { (yyval.no) = novoNo2(AST_MAIORIGUAL, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1434 "g-v1.c"
    break;

  case 37: /* DesigExpr: DesigExpr MENORIGUAL AddExpr  */
#line 286 "g-v1.y"
        { (yyval.no) = novoNo2(AST_MENORIGUAL, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1440 "g-v1.c"
    break;

  case 38: /* DesigExpr: AddExpr  */
#line 288 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1446 "g-v1.c"
    break;

  case 39: /* AddExpr: AddExpr '+' MulExpr  */
#line 296 "g-v1.y"
        { (yyval.no) = novoNo2(AST_SOMA, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1452 "g-v1.c"
    break;

  case 40: /* AddExpr: AddExpr '-' MulExpr  */
#line 298 "g-v1.y"
        { (yyval.no) = novoNo2(AST_SUB, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1458 "g-v1.c"
    break;

  case 41: /* AddExpr: MulExpr  */
#line 300 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1464 "g-v1.c"
    break;

  case 42: /* MulExpr: MulExpr '*' UnExpr  */
#line 308 "g-v1.y"
        { (yyval.no) = novoNo2(AST_MUL, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1470 "g-v1.c"
    break;

  case 43: /* MulExpr: MulExpr '/' UnExpr  */
#line 310 "g-v1.y"
        { (yyval.no) = novoNo2(AST_DIV, yylineno, (yyvsp[-2].no), (yyvsp[0].no)); }
#line 1476 "g-v1.c"
    break;

  case 44: /* MulExpr: UnExpr  */
#line 312 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1482 "g-v1.c"
    break;

  case 45: /* UnExpr: '-' PrimExpr  */
#line 321 "g-v1.y"
        { (yyval.no) = novoNo1(AST_NEG, yylineno, (yyvsp[0].no)); }
#line 1488 "g-v1.c"
    break;

  case 46: /* UnExpr: '!' PrimExpr  */
#line 323 "g-v1.y"
        { (yyval.no) = novoNo1(AST_NAO, yylineno, (yyvsp[0].no)); }
#line 1494 "g-v1.c"
    break;

  case 47: /* UnExpr: PrimExpr  */
#line 325 "g-v1.y"
        { (yyval.no) = (yyvsp[0].no); }
#line 1500 "g-v1.c"
    break;

  case 48: /* PrimExpr: IDENTIFICADOR  */
#line 333 "g-v1.y"
        { (yyval.no) = novoNo(AST_IDENTIFICADOR, yylineno, (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1506 "g-v1.c"
    break;

  case 49: /* PrimExpr: CARCONST  */
#line 335 "g-v1.y"
        { (yyval.no) = novoNo(AST_CARCONST, yylineno, (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1512 "g-v1.c"
    break;

  case 50: /* PrimExpr: INTCONST  */
#line 337 "g-v1.y"
        { (yyval.no) = novoNo(AST_INTCONST, yylineno, (yyvsp[0].str)); free((yyvsp[0].str)); }
#line 1518 "g-v1.c"
    break;

  case 51: /* PrimExpr: '(' Expr ')'  */
#line 339 "g-v1.y"
        { (yyval.no) = (yyvsp[-1].no); }
#line 1524 "g-v1.c"
    break;


#line 1528 "g-v1.c"

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
  YY_SYMBOL_PRINT ("-> $$ =", YY_CAST (yysymbol_kind_t, yyr1[yyn]), &yyval, &yyloc);

  YYPOPSTACK (yylen);
  yylen = 0;

  *++yyvsp = yyval;

  /* Now 'shift' the result of the reduction.  Determine what state
     that goes to, based on the state we popped back to and the rule
     number reduced by.  */
  {
    const int yylhs = yyr1[yyn] - YYNTOKENS;
    const int yyi = yypgoto[yylhs] + *yyssp;
    yystate = (0 <= yyi && yyi <= YYLAST && yycheck[yyi] == *yyssp
               ? yytable[yyi]
               : yydefgoto[yylhs]);
  }

  goto yynewstate;


/*--------------------------------------.
| yyerrlab -- here on detecting error.  |
`--------------------------------------*/
yyerrlab:
  /* Make sure we have latest lookahead translation.  See comments at
     user semantic actions for why this is necessary.  */
  yytoken = yychar == YYEMPTY ? YYSYMBOL_YYEMPTY : YYTRANSLATE (yychar);
  /* If not already recovering from an error, report this error.  */
  if (!yyerrstatus)
    {
      ++yynerrs;
      yyerror (YY_("syntax error"));
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
                      yytoken, &yylval);
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
  /* Pacify compilers when the user code never invokes YYERROR and the
     label yyerrorlab therefore never appears in user code.  */
  if (0)
    YYERROR;
  ++yynerrs;

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

  /* Pop stack until we find a state that shifts the error token.  */
  for (;;)
    {
      yyn = yypact[yystate];
      if (!yypact_value_is_default (yyn))
        {
          yyn += YYSYMBOL_YYerror;
          if (0 <= yyn && yyn <= YYLAST && yycheck[yyn] == YYSYMBOL_YYerror)
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
                  YY_ACCESSING_SYMBOL (yystate), yyvsp);
      YYPOPSTACK (1);
      yystate = *yyssp;
      YY_STACK_PRINT (yyss, yyssp);
    }

  YY_IGNORE_MAYBE_UNINITIALIZED_BEGIN
  *++yyvsp = yylval;
  YY_IGNORE_MAYBE_UNINITIALIZED_END


  /* Shift the error token.  */
  YY_SYMBOL_PRINT ("Shifting", YY_ACCESSING_SYMBOL (yyn), yyvsp, yylsp);

  yystate = yyn;
  goto yynewstate;


/*-------------------------------------.
| yyacceptlab -- YYACCEPT comes here.  |
`-------------------------------------*/
yyacceptlab:
  yyresult = 0;
  goto yyreturnlab;


/*-----------------------------------.
| yyabortlab -- YYABORT comes here.  |
`-----------------------------------*/
yyabortlab:
  yyresult = 1;
  goto yyreturnlab;


/*-----------------------------------------------------------.
| yyexhaustedlab -- YYNOMEM (memory exhaustion) comes here.  |
`-----------------------------------------------------------*/
yyexhaustedlab:
  yyerror (YY_("memory exhausted"));
  yyresult = 2;
  goto yyreturnlab;


/*----------------------------------------------------------.
| yyreturnlab -- parsing is finished, clean up and return.  |
`----------------------------------------------------------*/
yyreturnlab:
  if (yychar != YYEMPTY)
    {
      /* Make sure we have latest lookahead translation.  See comments at
         user semantic actions for why this is necessary.  */
      yytoken = YYTRANSLATE (yychar);
      yydestruct ("Cleanup: discarding lookahead",
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  YY_ACCESSING_SYMBOL (+*yyssp), yyvsp);
      YYPOPSTACK (1);
    }
#ifndef yyoverflow
  if (yyss != yyssa)
    YYSTACK_FREE (yyss);
#endif

  return yyresult;
}

#line 343 "g-v1.y"


/*
 * yyerror — chamada pelo Bison ao detectar um erro sintático.
 *
 * Imprime a mensagem de erro no formato exigido pelo trabalho:
 * "ERRO: <mensagem> proximo a '<token>' - linha: <número>"
 */
void yyerror(const char *s) {
    printf("ERRO: %s proximo a '%s' - linha: %d\n", s, yytext, yylineno);
}

/*
 * main — ponto de entrada do compilador.
 *
 * Abre o arquivo passado como argumento, executa a análise sintática
 * (que também constrói a AST), imprime a AST para depuração e libera
 * a memória ao final.
 *
 * Uso: ./g-v1 <arquivo.gv1>
 */
int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Uso: %s <arquivo.gv1>\n", argv[0]);
        return 1;
    }

    /* abre o arquivo de entrada — yyin é usado internamente pelo Flex */
    if (!(yyin = fopen(argv[1], "r"))) {
        perror(argv[1]);
        return 1;
    }

    /* yyparse() dispara a análise sintática e, a cada regra reconhecida,
       as ações semânticas constroem os nós da AST */
    if (yyparse() == 0) {
        printf("Analise sintatica concluida com sucesso.\n");
        printf("\n--- AST ---\n");
        imprimirAST(raizAST, 0); /* imprime a árvore para depuração */
        liberarAST(raizAST);    /* libera toda a memória da AST     */
    }

    fclose(yyin);
    return 0;
}
