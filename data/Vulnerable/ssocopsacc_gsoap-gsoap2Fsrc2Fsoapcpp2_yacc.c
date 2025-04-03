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
#line 64 "soapcpp2_yacc.y" /* yacc.c:339  */


#include "soapcpp2.h"

#ifdef WIN32
#ifndef __STDC__
#define __STDC__
#endif
#define YYINCLUDED_STDLIB_H
#ifdef WIN32_WITHOUT_SOLARIS_FLEX
extern int soapcpp2lex();
#else
extern int yylex();
#endif
#else
extern int yylex();
#endif

extern int is_XML(Tnode*);

#define MAXNEST 16	/* max. nesting depth of scopes */

struct Scope
{	Table	*table;
	Entry	*entry;
	Node	node;
	LONG64	val;
	int	offset;
	Bool	grow;	/* true if offset grows with declarations */
	Bool	mask;	/* true if enum is mask */
}	stack[MAXNEST],	/* stack of tables and offsets */
	*sp;		/* current scope stack pointer */

Table	*classtable = (Table*)0,
	*enumtable = (Table*)0,
	*typetable = (Table*)0,
	*booltable = (Table*)0,
	*templatetable = (Table*)0;

char	*namespaceid = NULL;
int	transient = 0;
int	permission = 0;
int	custom_header = 1;
int	custom_fault = 1;
Pragma	*pragmas = NULL;
Tnode	*qname = NULL;
Tnode	*xml = NULL;

/* function prototypes for support routine section */
static Entry	*undefined(Symbol*);
static Tnode	*mgtype(Tnode*, Tnode*);
static Node	op(const char*, Node, Node), iop(const char*, Node, Node), relop(const char*, Node, Node);
static void	mkscope(Table*, int), enterscope(Table*, int), exitscope();
static int	integer(Tnode*), real(Tnode*), numeric(Tnode*);
static void	add_soap(), add_XML(), add_qname(), add_header(Table*), add_fault(Table*), add_response(Entry*, Entry*), add_result(Tnode*);
extern char	*c_storage(Storage), *c_type(Tnode*), *c_ident(Tnode*);
extern int	is_primitive_or_string(Tnode*), is_stdstr(Tnode*), is_binary(Tnode*), is_external(Tnode*), is_mutable(Tnode*);

/* Temporaries used in semantic rules */
int	i;
char	*s, *s1, *s2;
Symbol	*sym;
Entry	*p, *q;
Tnode	*t;
Node	tmp, c;
Pragma	**pp;


#line 135 "soapcpp2_yacc.c" /* yacc.c:339  */

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
# define YYDEBUG 0
#endif
#if YYDEBUG
extern int yydebug;
#endif

/* Token type.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    PRAGMA = 258,
    AUTO = 259,
    DOUBLE = 260,
    INT = 261,
    STRUCT = 262,
    BREAK = 263,
    ELSE = 264,
    LONG = 265,
    SWITCH = 266,
    CASE = 267,
    ENUM = 268,
    REGISTER = 269,
    TYPEDEF = 270,
    CHAR = 271,
    EXTERN = 272,
    RETURN = 273,
    UNION = 274,
    CONST = 275,
    FLOAT = 276,
    SHORT = 277,
    UNSIGNED = 278,
    CONTINUE = 279,
    FOR = 280,
    SIGNED = 281,
    VOID = 282,
    DEFAULT = 283,
    GOTO = 284,
    SIZEOF = 285,
    VOLATILE = 286,
    DO = 287,
    IF = 288,
    STATIC = 289,
    WHILE = 290,
    CLASS = 291,
    PRIVATE = 292,
    PROTECTED = 293,
    PUBLIC = 294,
    VIRTUAL = 295,
    INLINE = 296,
    OPERATOR = 297,
    LLONG = 298,
    BOOL = 299,
    CFALSE = 300,
    CTRUE = 301,
    WCHAR = 302,
    TIME = 303,
    USING = 304,
    NAMESPACE = 305,
    ULLONG = 306,
    MUSTUNDERSTAND = 307,
    SIZE = 308,
    FRIEND = 309,
    TEMPLATE = 310,
    EXPLICIT = 311,
    TYPENAME = 312,
    RESTRICT = 313,
    null = 314,
    UCHAR = 315,
    USHORT = 316,
    UINT = 317,
    ULONG = 318,
    NONE = 319,
    ID = 320,
    LAB = 321,
    TYPE = 322,
    LNG = 323,
    DBL = 324,
    CHR = 325,
    STR = 326,
    PA = 327,
    NA = 328,
    TA = 329,
    DA = 330,
    MA = 331,
    AA = 332,
    XA = 333,
    OA = 334,
    LA = 335,
    RA = 336,
    OR = 337,
    AN = 338,
    EQ = 339,
    NE = 340,
    LE = 341,
    GE = 342,
    LS = 343,
    RS = 344,
    AR = 345,
    PP = 346,
    NN = 347
  };
#endif
/* Tokens.  */
#define PRAGMA 258
#define AUTO 259
#define DOUBLE 260
#define INT 261
#define STRUCT 262
#define BREAK 263
#define ELSE 264
#define LONG 265
#define SWITCH 266
#define CASE 267
#define ENUM 268
#define REGISTER 269
#define TYPEDEF 270
#define CHAR 271
#define EXTERN 272
#define RETURN 273
#define UNION 274
#define CONST 275
#define FLOAT 276
#define SHORT 277
#define UNSIGNED 278
#define CONTINUE 279
#define FOR 280
#define SIGNED 281
#define VOID 282
#define DEFAULT 283
#define GOTO 284
#define SIZEOF 285
#define VOLATILE 286
#define DO 287
#define IF 288
#define STATIC 289
#define WHILE 290
#define CLASS 291
#define PRIVATE 292
#define PROTECTED 293
#define PUBLIC 294
#define VIRTUAL 295
#define INLINE 296
#define OPERATOR 297
#define LLONG 298
#define BOOL 299
#define CFALSE 300
#define CTRUE 301
#define WCHAR 302
#define TIME 303
#define USING 304
#define NAMESPACE 305
#define ULLONG 306
#define MUSTUNDERSTAND 307
#define SIZE 308
#define FRIEND 309
#define TEMPLATE 310
#define EXPLICIT 311
#define TYPENAME 312
#define RESTRICT 313
#define null 314
#define UCHAR 315
#define USHORT 316
#define UINT 317
#define ULONG 318
#define NONE 319
#define ID 320
#define LAB 321
#define TYPE 322
#define LNG 323
#define DBL 324
#define CHR 325
#define STR 326
#define PA 327
#define NA 328
#define TA 329
#define DA 330
#define MA 331
#define AA 332
#define XA 333
#define OA 334
#define LA 335
#define RA 336
#define OR 337
#define AN 338
#define EQ 339
#define NE 340
#define LE 341
#define GE 342
#define LS 343
#define RS 344
#define AR 345
#define PP 346
#define NN 347

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED

union YYSTYPE
{
#line 138 "soapcpp2_yacc.y" /* yacc.c:355  */
	Symbol	*sym;
	LONG64	i;
	double	r;
	char	c;
	char	*s;
	Tnode	*typ;
	Storage	sto;
	Node	rec;
	Entry	*e;

#line 370 "soapcpp2_yacc.c" /* yacc.c:355  */
};

typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;

int yyparse (void);

#endif /* !YY_YY_Y_TAB_H_INCLUDED  */

/* Copy the second part of user declarations.  */

#line 387 "soapcpp2_yacc.c" /* yacc.c:358  */

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
#define YYFINAL  3
/* YYLAST -- Last index in YYTABLE.  */
#define YYLAST   1110

/* YYNTOKENS -- Number of terminals.  */
#define YYNTOKENS  118
/* YYNNTS -- Number of nonterminals.  */
#define YYNNTS  62
/* YYNRULES -- Number of rules.  */
#define YYNRULES  241
/* YYNSTATES -- Number of states.  */
#define YYNSTATES  376

/* YYTRANSLATE[YYX] -- Symbol number corresponding to YYX as returned
   by yylex, with out-of-bounds checking.  */
#define YYUNDEFTOK  2
#define YYMAXUTOK   347

#define YYTRANSLATE(YYX)                                                \
  ((unsigned int) (YYX) <= YYMAXUTOK ? yytranslate[YYX] : YYUNDEFTOK)

/* YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to TOKEN-NUM
   as returned by yylex, without out-of-bounds checking.  */
static const yytype_uint8 yytranslate[] =
{
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   112,     2,     2,   117,   103,    90,     2,
     114,   115,   101,    99,    72,   100,     2,   102,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,    85,   109,
      93,    73,    95,    84,   116,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,   110,     2,   111,    89,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,   107,    88,   108,   113,     2,     2,     2,
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
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    69,    70,    71,    74,    75,    76,
      77,    78,    79,    80,    81,    82,    83,    86,    87,    91,
      92,    94,    96,    97,    98,   104,   105,   106
};

#if YYDEBUG
  /* YYRLINE[YYN] -- Source line where rule number YYN was defined.  */
static const yytype_uint16 yyrline[] =
{
       0,   212,   212,   228,   242,   244,   246,   252,   254,   255,
     256,   263,   264,   266,   289,   292,   294,   296,   298,   300,
     302,   304,   307,   310,   313,   316,   319,   320,   321,   323,
     324,   325,   326,   329,   449,   465,   466,   468,   469,   470,
     471,   472,   473,   474,   475,   476,   477,   478,   479,   480,
     481,   482,   483,   484,   485,   486,   487,   488,   489,   490,
     491,   492,   493,   494,   495,   496,   497,   498,   499,   500,
     501,   502,   503,   504,   505,   516,   526,   543,   601,   603,
     604,   606,   607,   609,   688,   695,   710,   712,   715,   719,
     729,   776,   782,   786,   796,   843,   844,   845,   846,   847,
     848,   849,   850,   851,   852,   853,   854,   855,   856,   857,
     858,   859,   860,   861,   862,   870,   892,   903,   922,   925,
     938,   960,   979,   994,  1009,  1032,  1052,  1067,  1082,  1104,
    1123,  1142,  1150,  1158,  1176,  1185,  1190,  1195,  1200,  1206,
    1223,  1242,  1259,  1260,  1262,  1263,  1264,  1265,  1272,  1274,
    1281,  1289,  1295,  1296,  1298,  1306,  1307,  1308,  1309,  1310,
    1311,  1312,  1313,  1314,  1315,  1316,  1317,  1318,  1322,  1323,
    1325,  1326,  1328,  1329,  1331,  1332,  1334,  1335,  1342,  1347,
    1349,  1362,  1366,  1374,  1375,  1386,  1387,  1389,  1394,  1399,
    1404,  1409,  1415,  1416,  1418,  1419,  1420,  1429,  1430,  1433,
    1438,  1441,  1444,  1447,  1449,  1452,  1455,  1457,  1460,  1461,
    1462,  1463,  1464,  1465,  1466,  1467,  1468,  1469,  1470,  1471,
    1472,  1473,  1474,  1475,  1476,  1479,  1484,  1489,  1499,  1500,
    1507,  1511,  1516,  1519,  1520,  1527,  1531,  1535,  1539,  1543,
    1547,  1551
};
#endif

#if YYDEBUG || YYERROR_VERBOSE || 0
/* YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
   First, the terminals, then, starting at YYNTOKENS, nonterminals.  */
static const char *const yytname[] =
{
  "$end", "error", "$undefined", "PRAGMA", "AUTO", "DOUBLE", "INT",
  "STRUCT", "BREAK", "ELSE", "LONG", "SWITCH", "CASE", "ENUM", "REGISTER",
  "TYPEDEF", "CHAR", "EXTERN", "RETURN", "UNION", "CONST", "FLOAT",
  "SHORT", "UNSIGNED", "CONTINUE", "FOR", "SIGNED", "VOID", "DEFAULT",
  "GOTO", "SIZEOF", "VOLATILE", "DO", "IF", "STATIC", "WHILE", "CLASS",
  "PRIVATE", "PROTECTED", "PUBLIC", "VIRTUAL", "INLINE", "OPERATOR",
  "LLONG", "BOOL", "CFALSE", "CTRUE", "WCHAR", "TIME", "USING",
  "NAMESPACE", "ULLONG", "MUSTUNDERSTAND", "SIZE", "FRIEND", "TEMPLATE",
  "EXPLICIT", "TYPENAME", "RESTRICT", "null", "UCHAR", "USHORT", "UINT",
  "ULONG", "NONE", "ID", "LAB", "TYPE", "LNG", "DBL", "CHR", "STR", "','",
  "'='", "PA", "NA", "TA", "DA", "MA", "AA", "XA", "OA", "LA", "RA", "'?'",
  "':'", "OR", "AN", "'|'", "'^'", "'&'", "EQ", "NE", "'<'", "LE", "'>'",
  "GE", "LS", "RS", "'+'", "'-'", "'*'", "'/'", "'%'", "AR", "PP", "NN",
  "'{'", "'}'", "';'", "'['", "']'", "'!'", "'~'", "'('", "')'", "'@'",
  "'$'", "$accept", "prog", "s1", "exts", "exts1", "ext", "pragma",
  "decls", "t1", "t2", "t3", "t4", "t5", "dclrs", "dclr", "fdclr", "id",
  "name", "constr", "destr", "func", "fname", "fargso", "fargs", "farg",
  "arg", "texp", "spec", "tspec", "type", "struct", "class", "enum",
  "tname", "base", "s2", "s3", "s4", "s5", "s6", "store", "constobj",
  "abstract", "virtual", "ptrs", "array", "arrayck", "init", "tag",
  "occurs", "patt", "cint", "expr", "cexp", "qexp", "oexp", "obex", "aexp",
  "abex", "rexp", "lexp", "pexp", YY_NULLPTR
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
     315,   316,   317,   318,   319,   320,   321,   322,   323,   324,
     325,   326,    44,    61,   327,   328,   329,   330,   331,   332,
     333,   334,   335,   336,    63,    58,   337,   338,   124,    94,
      38,   339,   340,    60,   341,    62,   342,   343,   344,    43,
      45,    42,    47,    37,   345,   346,   347,   123,   125,    59,
      91,    93,    33,   126,    40,    41,    64,    36
};
# endif

#define YYPACT_NINF -333

#define yypact_value_is_default(Yystate) \
  (!!((Yystate) == (-333)))

#define YYTABLE_NINF -208

#define yytable_value_is_error(Yytable_value) \
  0

  /* YYPACT[STATE-NUM] -- Index in YYTABLE of the portion describing
     STATE-NUM.  */
static const yytype_int16 yypact[] =
{
    -333,    35,   -25,  -333,   -13,  -333,   213,   -40,   -54,  -333,
    -333,  -333,  -333,    16,  -333,    13,  -333,  -333,  -333,  -333,
    -333,    17,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
      20,   -16,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,    11,  -333,  -333,  -333,  -333,  -333,   -67,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,   -49,  -333,  -333,   -52,
     967,    -7,   -61,    -2,   967,     0,  -333,  -333,    18,    28,
      29,  -333,  -333,    49,    28,    29,    52,  -333,  -333,    54,
      28,    29,  -333,    64,    82,  -333,  -333,  -333,  -333,    55,
     967,  -333,  -333,  -333,    98,  -333,  -333,  -333,     8,  -333,
      23,  -333,  -333,    25,  -333,  -333,  -333,    58,   672,  -333,
     441,  -333,    86,   900,  -333,   441,  -333,  -333,   441,  -333,
    -333,    52,   126,  -333,   967,   967,  -333,  -333,  -333,  -333,
     786,   -81,  -333,  -333,  -333,   441,   157,    25,    25,    25,
    -333,   117,   441,   900,  -333,  -333,   116,   152,   153,   156,
     134,   555,   -44,  -333,   171,   137,   441,   138,   155,  -333,
     -24,  -333,  -333,  -333,   967,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,   140,  -333,  -333,
     133,  -333,   979,  -333,   159,   150,  -333,  -333,  -333,  -333,
    -333,   154,   171,  -333,  -333,  -333,  -333,  -333,   160,   327,
     900,   162,   164,  -333,   169,  -333,   227,   172,  -333,   166,
    -333,   207,  -333,  -333,  -333,   170,  -333,  -333,  -333,  -333,
    -333,  -333,  -333,  -333,   996,   996,   996,   996,   172,   996,
     996,   996,   191,    70,    79,   196,   222,   198,  -333,  -333,
     245,   240,  -333,   441,  -333,   204,   327,   327,   327,   327,
    -333,   171,  -333,  -333,    52,  -333,   293,   967,     9,   967,
    -333,  -333,  -333,  -333,  -333,  -333,  -333,   -56,  -333,   172,
     996,   996,   996,   996,   996,   996,   996,   996,   996,   996,
     996,   996,   996,   996,   996,   996,   996,   996,   996,   220,
    -333,   243,    22,   209,  -333,  -333,  -333,  -333,  -333,   210,
    -333,  -333,   246,  -333,  -333,   172,   206,   996,  -333,  -333,
     248,   242,   196,   198,   311,   424,    48,    85,    85,    31,
      31,    31,    31,   205,   205,    -8,    -8,  -333,  -333,  -333,
    -333,   996,  -333,  -333,     2,   257,   267,   251,  -333,  -333,
     270,  -333,   240,  -333,  -333,   996,  -333,  -333,  -333,  -333,
       2,  -333,   243,  -333,  -333,  -333
};

  /* YYDEFACT[STATE-NUM] -- Default reduction number in state STATE-NUM.
     Performed when YYTABLE does not specify something else to do.  Zero
     means the default is an error.  */
static const yytype_uint8 yydefact[] =
{
       3,     0,     6,     1,     0,     2,     0,     0,     0,    13,
     155,   106,   100,     0,   101,     0,   156,   160,    97,   159,
     166,     0,   162,   105,    99,   108,   107,    95,   169,   157,
       0,   161,   164,   102,    96,    98,   113,   103,   165,   104,
     163,     0,   158,   109,   110,   111,   112,   133,    21,    22,
     167,   168,     7,     9,    11,    12,     0,    78,    78,   176,
      88,     0,   118,     0,    88,     0,     6,    10,     0,   122,
     123,   149,   139,     0,   131,   132,     0,   149,   141,     0,
     126,   127,   150,     0,     0,    35,    36,   149,   140,     0,
       0,   176,     8,    29,     0,    30,    27,    78,     0,   161,
     133,    90,   149,     0,   149,   149,    89,     0,     0,   136,
       0,   138,     0,    88,   137,     0,   150,   135,     0,   142,
     143,     0,     0,   176,    92,    91,    31,    78,   154,    28,
       0,   179,   178,   177,    34,     0,     0,     0,     0,     0,
     147,   119,     0,    88,    76,     4,     0,     0,     0,     0,
       0,     0,     0,   151,   152,     0,     0,     0,     0,   134,
     179,    94,    93,    32,    79,    40,    41,    42,    43,    44,
      45,    46,    47,    48,    49,    50,    51,    52,    53,    54,
      55,    56,    57,    58,    59,    60,    61,    62,    63,    64,
      65,    66,    67,    68,    71,    69,    70,     0,    38,    39,
       0,    74,     0,   182,   185,     0,   148,   145,   144,   146,
     149,     0,   152,    20,    23,    24,    25,   120,     0,     0,
      88,   176,     0,   124,     0,   115,     0,   179,    86,     0,
      80,    81,   176,    72,    73,     0,   240,   241,   236,   234,
     235,   237,   238,   239,     0,     0,     0,     0,   179,     0,
       0,     0,     0,   200,     0,   203,     0,   206,   224,   232,
       0,   192,   121,     0,   116,     0,     0,     0,     0,     0,
      15,   152,   128,   125,     0,    87,   170,     0,    84,     0,
     230,   228,   227,   229,   181,   225,   226,     0,   198,   179,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
     193,   183,   187,     0,   129,    16,    17,    18,    19,     0,
     114,   171,   172,    82,    85,   179,     0,     0,   233,   180,
     201,     0,   202,   205,   208,   209,   210,   211,   212,   213,
     214,   215,   216,   217,   218,   219,   220,   221,   222,   223,
     186,     0,    33,   194,     0,     0,     0,   188,   117,   130,
       0,    77,   192,   231,   197,     0,   184,   191,   195,   196,
     189,   173,   183,   199,   190,    83
};

  /* YYPGOTO[NTERM-NUM].  */
static const yytype_int16 yypgoto[] =
{
    -333,  -333,  -333,  -333,   273,  -333,  -333,  -108,     7,   139,
    -333,  -333,  -333,    -5,   292,   296,   -15,  -333,  -333,  -333,
     -39,  -333,  -333,    75,  -333,  -333,  -126,   -11,  -113,   -88,
    -333,  -333,  -333,  -333,    37,   -33,   244,  -333,  -182,  -333,
     -85,  -333,  -333,  -333,  -114,  -152,    34,   -10,  -333,    -6,
    -333,  -332,  -259,  -199,  -333,  -333,  -333,    81,  -333,   -97,
     -77,  -333
};

  /* YYDEFGOTO[NTERM-NUM].  */
static const yytype_int16 yydefgoto[] =
{
      -1,     1,     2,     5,     6,    52,    53,   150,   151,    55,
     266,   267,   268,   152,   126,   127,    72,   134,    57,    58,
      93,    94,   229,   230,   231,   325,   122,    59,   123,    60,
      61,    62,    63,   121,   141,   110,   115,   220,   222,   164,
      64,   322,   361,    65,    98,   203,   204,   352,   261,   311,
     312,   357,   287,   288,   331,   253,   254,   255,   256,   257,
     258,   259
};

  /* YYTABLE[YYPACT[STATE-NUM]] -- What to do in state STATE-NUM.  If
     positive, shift that token.  If negative, reduce the rule whose
     number is the opposite.  If YYTABLE_NINF, syntax error.  */
static const yytype_int16 yytable[] =
{
      78,    56,   124,   252,   201,   125,    83,   155,   228,   160,
     157,   161,   162,    54,    73,    88,   327,    68,    79,    95,
     -26,    84,   367,    91,   103,     4,    90,   205,    91,   202,
     265,   330,   136,   -37,   211,     3,   124,   124,   374,   125,
     125,   227,   124,   218,   113,   125,   104,   -75,   224,   101,
     130,   232,     7,   106,   118,    67,   -26,   -26,   129,   328,
      92,   112,   137,   138,   139,   219,   132,    66,   364,   135,
     353,   142,   143,   131,   324,   275,   124,   133,    74,   125,
      75,    69,    80,    70,    81,    85,   202,    86,   163,   319,
     353,   119,   140,   306,   307,   308,   284,  -175,   132,   132,
     102,   355,   356,    56,    89,   105,   158,   354,   154,   133,
     133,   270,   120,   107,    76,    54,    90,    85,   278,    86,
      77,   355,   356,    71,    82,   144,   109,    87,   302,   303,
     304,   305,   306,   307,   308,   -35,   -36,   329,   212,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   366,   326,  -204,   313,  -204,   111,   315,   316,
     317,   318,   114,   290,   232,   291,   373,   280,   281,   282,
     283,   116,   285,   286,   207,   208,   209,   263,   298,   299,
     300,   301,   302,   303,   304,   305,   306,   307,   308,   124,
     117,   124,   125,   153,   125,   333,   334,   335,   336,   337,
     338,   339,   340,   341,   342,   343,   344,   345,   346,   347,
     348,   349,   128,    -5,     8,   271,     9,    10,    11,    12,
      13,   159,   206,    14,   210,   213,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    24,    25,   214,   215,    26,
      27,   216,   217,   221,    28,   223,   225,    29,   234,    30,
     226,   233,   260,    31,    32,   -88,    33,    34,   262,   320,
      35,    36,   264,   274,    37,    38,    39,    40,    41,    42,
    -153,    49,   272,    43,    44,    45,    46,   273,   -88,   277,
      47,   276,   202,  -207,   279,   -88,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   289,   -88,   304,   305,   306,   307,   308,   292,
     309,   310,   314,   321,   -88,   350,   351,   358,   359,   360,
     327,   363,   -88,    48,    49,   368,  -174,   365,   146,    50,
      51,    10,    11,    12,    13,   369,   370,    14,   371,   108,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    96,   323,    26,    27,    97,   372,   269,    28,   362,
     156,    29,   375,    30,   147,   148,   149,    31,    32,   -88,
      33,    34,   332,     0,    35,    36,     0,     0,    37,    38,
      39,    40,    41,    42,     0,     0,     0,    43,    44,    45,
      46,     0,   -88,     0,    47,     0,     0,     0,     0,   -88,
     294,   295,   296,   297,   298,   299,   300,   301,   302,   303,
     304,   305,   306,   307,   308,     0,     0,   -88,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   -88,     0,
       0,     0,     0,     0,     0,   -14,   -88,    48,   -14,     0,
    -174,     0,   146,    50,    51,    10,    11,    12,    13,     0,
       0,    14,     0,     0,    15,    16,    17,    18,    19,    20,
      21,    22,    23,    24,    25,     0,     0,    26,    27,     0,
       0,     0,    28,     0,     0,    29,     0,    30,   147,   148,
     149,    31,    32,   -88,    33,    34,     0,     0,    35,    36,
       0,     0,    37,    38,    39,    40,    41,    42,     0,     0,
       0,    43,    44,    45,    46,     0,   -88,     0,    47,     0,
       0,     0,     0,   -88,   295,   296,   297,   298,   299,   300,
     301,   302,   303,   304,   305,   306,   307,   308,     0,     0,
       0,   -88,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -88,     0,     0,     0,     0,     0,     0,   -14,
     -88,    48,     0,     0,  -174,     0,   146,    50,    51,    10,
      11,    12,    13,     0,     0,    14,     0,     0,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    24,    25,     0,
       0,    26,    27,     0,     0,     0,    28,     0,     0,    29,
       0,    30,   147,   148,   149,    31,    32,   -88,    33,    34,
       0,     0,    35,    36,     0,     0,    37,    38,    39,    40,
      41,    42,     0,     0,     0,    43,    44,    45,    46,     0,
     -88,     0,    47,     0,     0,     0,     0,   -88,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,   -88,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,   -88,     0,     0,     0,
       0,     0,     0,     0,   -88,    48,   -14,     0,  -174,     0,
       0,    50,    51,     8,     0,     9,    10,    11,    12,    13,
       0,     0,    14,     0,     0,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    24,    25,     0,     0,    26,    27,
       0,     0,     0,    28,     0,     0,    29,     0,    30,     0,
       0,     0,    31,    32,   -88,    33,    34,     0,     0,    35,
      36,     0,     0,    37,    38,    39,    40,    41,    42,     0,
       0,     0,    43,    44,    45,    46,     0,   -88,     0,    47,
       0,     0,     0,     0,   -88,     0,     0,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,   -88,     0,     0,     0,     0,     0,     0,     0,
       0,     0,     0,   -88,     0,     0,     0,     0,     0,     0,
     145,   -88,    48,    49,     0,  -174,     0,     0,    50,    51,
      10,    11,    12,    13,     0,     0,    14,     0,     0,    15,
      16,    17,    18,    19,    20,    21,    22,    23,    24,    25,
       0,     0,    26,    27,     0,     0,     0,    28,     0,     0,
      29,     0,    30,     0,     0,     0,    99,    32,     0,    33,
      34,     0,     0,    35,    36,     0,     0,    37,    38,    39,
      40,    41,    42,     0,     0,     0,    43,    44,    45,    46,
       0,     0,     0,   100,     0,     0,     0,     0,     0,   165,
     166,   167,   168,   169,   170,   171,   172,   173,   174,   175,
       0,     0,   176,   177,   178,   179,   180,   181,   182,   183,
     184,   185,   186,   187,   188,   189,   190,   191,   192,   193,
     194,   195,   196,     0,     0,     0,   197,     0,   198,   199,
     200,     0,    50,    51,    10,    11,    12,    13,     0,     0,
      14,     0,     0,    15,    16,    17,    18,    19,    20,    21,
      22,    23,    24,    25,     0,     0,    26,    27,     0,     0,
       0,    28,     0,     0,    29,     0,    30,     0,     0,     0,
      31,    32,     0,    33,    34,     0,     0,    35,    36,     0,
       0,    37,    38,    39,    40,    41,    42,     0,     0,     0,
      43,    44,    45,    46,     0,     0,     0,    47,     0,     0,
       0,    10,    11,    12,    13,     0,     0,    14,     0,     0,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,     0,     0,    26,    27,     0,     0,     0,    28,     0,
       0,    29,     0,    30,     0,     0,     0,    99,    32,   235,
      33,    34,     0,  -174,    35,    36,    50,    51,    37,    38,
      39,    40,    41,    42,   236,   237,   235,    43,    44,    45,
      46,     0,     0,     0,   100,     0,     0,     0,   238,     0,
       0,   236,   237,     0,   239,     0,     0,   240,   241,   242,
     243,     0,     0,     0,     0,   238,     0,     0,     0,     0,
       0,   239,     0,     0,   240,   241,   242,   243,     0,   244,
       0,     0,     0,     0,     0,     0,     0,     0,   245,   246,
     247,     0,     0,    50,    51,     0,   244,     0,     0,     0,
     248,   249,   250,   251,     0,   245,   246,   247,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,   249,   250,
     251
};

static const yytype_int16 yycheck[] =
{
      15,     6,    90,   202,   130,    90,    21,   115,   160,   123,
     118,   124,   125,     6,     1,    30,    72,     1,     1,    58,
      72,     1,   354,    72,    85,    50,    93,   135,    72,   110,
     212,   290,     7,   114,   142,     0,   124,   125,   370,   124,
     125,    65,   130,   151,    77,   130,   107,   114,   156,    60,
      42,   164,    65,    64,    87,   109,   108,   109,    97,   115,
     109,    76,    37,    38,    39,   109,    90,   107,   327,   102,
      68,   104,   105,    65,    65,   227,   164,   101,    65,   164,
      67,    65,    65,    67,    67,    65,   110,    67,   127,   271,
      68,    36,    67,   101,   102,   103,   248,   113,    90,    90,
     107,    99,   100,   108,    93,   107,   121,    85,   113,   101,
     101,   219,    57,   113,   101,   108,    93,    65,   232,    67,
     107,    99,   100,   107,   107,    67,   108,   107,    97,    98,
      99,   100,   101,   102,   103,   107,   107,   289,   143,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   351,   279,    84,   263,    86,   108,   266,   267,
     268,   269,   108,    84,   277,    86,   365,   244,   245,   246,
     247,   107,   249,   250,   137,   138,   139,   210,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,   277,
     108,   279,   277,   107,   279,   292,   293,   294,   295,   296,
     297,   298,   299,   300,   301,   302,   303,   304,   305,   306,
     307,   308,   114,     0,     1,   220,     3,     4,     5,     6,
       7,    95,    65,    10,   107,   109,    13,    14,    15,    16,
      17,    18,    19,    20,    21,    22,    23,    85,    85,    26,
      27,    85,   108,    72,    31,   108,   108,    34,   115,    36,
      95,   111,    93,    40,    41,    42,    43,    44,   108,   274,
      47,    48,   108,    36,    51,    52,    53,    54,    55,    56,
     108,   111,   108,    60,    61,    62,    63,   108,    65,    72,
      67,   115,   110,    87,   114,    72,    88,    89,    90,    91,
      92,    93,    94,    95,    96,    97,    98,    99,   100,   101,
     102,   103,   111,    90,    99,   100,   101,   102,   103,    87,
      65,    71,   108,    20,   101,    95,    73,   108,   108,    73,
      72,   115,   109,   110,   111,    68,   113,    85,     1,   116,
     117,     4,     5,     6,     7,    68,    85,    10,    68,    66,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    59,   277,    26,    27,    59,   362,   218,    31,   325,
     116,    34,   372,    36,    37,    38,    39,    40,    41,    42,
      43,    44,   291,    -1,    47,    48,    -1,    -1,    51,    52,
      53,    54,    55,    56,    -1,    -1,    -1,    60,    61,    62,
      63,    -1,    65,    -1,    67,    -1,    -1,    -1,    -1,    72,
      89,    90,    91,    92,    93,    94,    95,    96,    97,    98,
      99,   100,   101,   102,   103,    -1,    -1,    90,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,
      -1,    -1,    -1,    -1,    -1,   108,   109,   110,   111,    -1,
     113,    -1,     1,   116,   117,     4,     5,     6,     7,    -1,
      -1,    10,    -1,    -1,    13,    14,    15,    16,    17,    18,
      19,    20,    21,    22,    23,    -1,    -1,    26,    27,    -1,
      -1,    -1,    31,    -1,    -1,    34,    -1,    36,    37,    38,
      39,    40,    41,    42,    43,    44,    -1,    -1,    47,    48,
      -1,    -1,    51,    52,    53,    54,    55,    56,    -1,    -1,
      -1,    60,    61,    62,    63,    -1,    65,    -1,    67,    -1,
      -1,    -1,    -1,    72,    90,    91,    92,    93,    94,    95,
      96,    97,    98,    99,   100,   101,   102,   103,    -1,    -1,
      -1,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,   108,
     109,   110,    -1,    -1,   113,    -1,     1,   116,   117,     4,
       5,     6,     7,    -1,    -1,    10,    -1,    -1,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    -1,
      -1,    26,    27,    -1,    -1,    -1,    31,    -1,    -1,    34,
      -1,    36,    37,    38,    39,    40,    41,    42,    43,    44,
      -1,    -1,    47,    48,    -1,    -1,    51,    52,    53,    54,
      55,    56,    -1,    -1,    -1,    60,    61,    62,    63,    -1,
      65,    -1,    67,    -1,    -1,    -1,    -1,    72,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    90,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,   101,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,   109,   110,   111,    -1,   113,    -1,
      -1,   116,   117,     1,    -1,     3,     4,     5,     6,     7,
      -1,    -1,    10,    -1,    -1,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    -1,    -1,    26,    27,
      -1,    -1,    -1,    31,    -1,    -1,    34,    -1,    36,    -1,
      -1,    -1,    40,    41,    42,    43,    44,    -1,    -1,    47,
      48,    -1,    -1,    51,    52,    53,    54,    55,    56,    -1,
      -1,    -1,    60,    61,    62,    63,    -1,    65,    -1,    67,
      -1,    -1,    -1,    -1,    72,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    90,    -1,    -1,    -1,    -1,    -1,    -1,    -1,
      -1,    -1,    -1,   101,    -1,    -1,    -1,    -1,    -1,    -1,
     108,   109,   110,   111,    -1,   113,    -1,    -1,   116,   117,
       4,     5,     6,     7,    -1,    -1,    10,    -1,    -1,    13,
      14,    15,    16,    17,    18,    19,    20,    21,    22,    23,
      -1,    -1,    26,    27,    -1,    -1,    -1,    31,    -1,    -1,
      34,    -1,    36,    -1,    -1,    -1,    40,    41,    -1,    43,
      44,    -1,    -1,    47,    48,    -1,    -1,    51,    52,    53,
      54,    55,    56,    -1,    -1,    -1,    60,    61,    62,    63,
      -1,    -1,    -1,    67,    -1,    -1,    -1,    -1,    -1,    73,
      74,    75,    76,    77,    78,    79,    80,    81,    82,    83,
      -1,    -1,    86,    87,    88,    89,    90,    91,    92,    93,
      94,    95,    96,    97,    98,    99,   100,   101,   102,   103,
     104,   105,   106,    -1,    -1,    -1,   110,    -1,   112,   113,
     114,    -1,   116,   117,     4,     5,     6,     7,    -1,    -1,
      10,    -1,    -1,    13,    14,    15,    16,    17,    18,    19,
      20,    21,    22,    23,    -1,    -1,    26,    27,    -1,    -1,
      -1,    31,    -1,    -1,    34,    -1,    36,    -1,    -1,    -1,
      40,    41,    -1,    43,    44,    -1,    -1,    47,    48,    -1,
      -1,    51,    52,    53,    54,    55,    56,    -1,    -1,    -1,
      60,    61,    62,    63,    -1,    -1,    -1,    67,    -1,    -1,
      -1,     4,     5,     6,     7,    -1,    -1,    10,    -1,    -1,
      13,    14,    15,    16,    17,    18,    19,    20,    21,    22,
      23,    -1,    -1,    26,    27,    -1,    -1,    -1,    31,    -1,
      -1,    34,    -1,    36,    -1,    -1,    -1,    40,    41,    30,
      43,    44,    -1,   113,    47,    48,   116,   117,    51,    52,
      53,    54,    55,    56,    45,    46,    30,    60,    61,    62,
      63,    -1,    -1,    -1,    67,    -1,    -1,    -1,    59,    -1,
      -1,    45,    46,    -1,    65,    -1,    -1,    68,    69,    70,
      71,    -1,    -1,    -1,    -1,    59,    -1,    -1,    -1,    -1,
      -1,    65,    -1,    -1,    68,    69,    70,    71,    -1,    90,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,    99,   100,
     101,    -1,    -1,   116,   117,    -1,    90,    -1,    -1,    -1,
     111,   112,   113,   114,    -1,    99,   100,   101,    -1,    -1,
      -1,    -1,    -1,    -1,    -1,    -1,    -1,    -1,   112,   113,
     114
};

  /* YYSTOS[STATE-NUM] -- The (internal number of the) accessing
     symbol of state STATE-NUM.  */
static const yytype_uint8 yystos[] =
{
       0,   119,   120,     0,    50,   121,   122,    65,     1,     3,
       4,     5,     6,     7,    10,    13,    14,    15,    16,    17,
      18,    19,    20,    21,    22,    23,    26,    27,    31,    34,
      36,    40,    41,    43,    44,    47,    48,    51,    52,    53,
      54,    55,    56,    60,    61,    62,    63,    67,   110,   111,
     116,   117,   123,   124,   126,   127,   131,   136,   137,   145,
     147,   148,   149,   150,   158,   161,   107,   109,     1,    65,
      67,   107,   134,     1,    65,    67,   101,   107,   134,     1,
      65,    67,   107,   134,     1,    65,    67,   107,   134,    93,
      93,    72,   109,   138,   139,   138,   132,   133,   162,    40,
      67,   145,   107,    85,   107,   107,   145,   113,   122,   108,
     153,   108,   134,   153,   108,   154,   107,   108,   153,    36,
      57,   151,   144,   146,   147,   158,   132,   133,   114,   138,
      42,    65,    90,   101,   135,   153,     7,    37,    38,    39,
      67,   152,   153,   153,    67,   108,     1,    37,    38,    39,
     125,   126,   131,   107,   131,   125,   154,   125,   134,    95,
     162,   146,   146,   138,   157,    73,    74,    75,    76,    77,
      78,    79,    80,    81,    82,    83,    86,    87,    88,    89,
      90,    91,    92,    93,    94,    95,    96,    97,    98,    99,
     100,   101,   102,   103,   104,   105,   106,   110,   112,   113,
     114,   144,   110,   163,   164,   125,    65,   152,   152,   152,
     107,   125,   131,   109,    85,    85,    85,   108,   125,   109,
     155,    72,   156,   108,   125,   108,    95,    65,   163,   140,
     141,   142,   146,   111,   115,    30,    45,    46,    59,    65,
      68,    69,    70,    71,    90,    99,   100,   101,   111,   112,
     113,   114,   171,   173,   174,   175,   176,   177,   178,   179,
      93,   166,   108,   153,   108,   156,   128,   129,   130,   127,
     125,   131,   108,   108,    36,   163,   115,    72,   162,   114,
     178,   178,   178,   178,   163,   178,   178,   170,   171,   111,
      84,    86,    87,    88,    89,    90,    91,    92,    93,    94,
      95,    96,    97,    98,    99,   100,   101,   102,   103,    65,
      71,   167,   168,   125,   108,   125,   125,   125,   125,   156,
     134,    20,   159,   141,    65,   143,   144,    72,   115,   163,
     170,   172,   175,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
      95,    73,   165,    68,    85,    99,   100,   169,   108,   108,
      73,   160,   164,   115,   170,    85,   171,   169,    68,    68,
      85,    68,   167,   171,   169,   165
};

  /* YYR1[YYN] -- Symbol number of symbol that rule YYN derives.  */
static const yytype_uint8 yyr1[] =
{
       0,   118,   119,   120,   121,   121,   122,   122,   123,   123,
     123,   123,   123,   124,   125,   125,   125,   125,   125,   125,
     125,   126,   127,   128,   129,   130,   131,   131,   131,   131,
     131,   131,   131,   132,   133,   134,   134,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   135,   135,   135,   135,   135,
     135,   135,   135,   135,   135,   136,   137,   138,   139,   140,
     140,   141,   141,   142,   143,   143,   144,   144,   145,   145,
     145,   146,   146,   146,   146,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   147,
     147,   147,   147,   147,   147,   147,   147,   147,   147,   148,
     149,   150,   151,   151,   152,   152,   152,   152,   152,   153,
     154,   155,   156,   156,   157,   158,   158,   158,   158,   158,
     158,   158,   158,   158,   158,   158,   158,   158,   158,   158,
     159,   159,   160,   160,   161,   161,   162,   162,   162,   163,
     163,   163,   164,   165,   165,   166,   166,   167,   167,   167,
     167,   167,   168,   168,   169,   169,   169,   170,   170,   171,
     171,   172,   173,   173,   174,   175,   175,   176,   177,   177,
     177,   177,   177,   177,   177,   177,   177,   177,   177,   177,
     177,   177,   177,   177,   177,   178,   178,   178,   178,   178,
     178,   178,   178,   179,   179,   179,   179,   179,   179,   179,
     179,   179
};

  /* YYR2[YYN] -- Number of symbols on the right hand side of rule YYN.  */
static const yytype_uint8 yyr2[] =
{
       0,     2,     2,     0,     5,     1,     0,     2,     2,     1,
       2,     1,     1,     1,     0,     3,     4,     4,     4,     4,
       2,     1,     1,     0,     0,     0,     1,     2,     3,     2,
       2,     3,     4,     6,     2,     1,     1,     1,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     3,     3,     2,     1,     3,     7,     0,     0,
       1,     1,     3,     6,     0,     1,     3,     4,     0,     2,
       2,     1,     1,     2,     2,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     7,     5,     5,     7,     1,     3,
       5,     5,     2,     2,     5,     6,     2,     2,     6,     6,
       8,     2,     2,     1,     4,     3,     3,     3,     3,     2,
       2,     2,     1,     1,     2,     2,     2,     1,     2,     0,
       0,     0,     0,     1,     0,     1,     1,     1,     1,     1,
       1,     1,     1,     1,     1,     1,     1,     1,     1,     1,
       0,     1,     0,     2,     0,     1,     0,     2,     2,     0,
       4,     3,     1,     0,     2,     0,     3,     1,     2,     3,
       4,     3,     0,     1,     1,     2,     2,     3,     1,     5,
       1,     1,     3,     1,     1,     3,     1,     1,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,     3,     3,     1,     2,     2,     2,     2,     2,
       2,     4,     1,     3,     1,     1,     1,     1,     1,     1,
       1,     1
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
      yyerror (YY_("syntax error: cannot back up")); \
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
                  Type, Value); \
      YYFPRINTF (stderr, "\n");                                           \
    }                                                                     \
} while (0)


/*----------------------------------------.
| Print this symbol's value on YYOUTPUT.  |
`----------------------------------------*/

static void
yy_symbol_value_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  FILE *yyo = yyoutput;
  YYUSE (yyo);
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
yy_symbol_print (FILE *yyoutput, int yytype, YYSTYPE const * const yyvaluep)
{
  YYFPRINTF (yyoutput, "%s %s (",
             yytype < YYNTOKENS ? "token" : "nterm", yytname[yytype]);

  yy_symbol_value_print (yyoutput, yytype, yyvaluep);
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
yy_reduce_print (yytype_int16 *yyssp, YYSTYPE *yyvsp, int yyrule)
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
                                              );
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
yydestruct (const char *yymsg, int yytype, YYSTYPE *yyvaluep)
{
  YYUSE (yyvaluep);
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
yyparse (void)
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
#line 212 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (lflag)
    			  {	custom_header = 0;
    			  	custom_fault = 0;
			  }
			  else
			  {	add_header(sp->table);
			  	add_fault(sp->table);
			  }
			  compile(sp->table);
			  freetable(classtable);
			  freetable(enumtable);
			  freetable(typetable);
			  freetable(booltable);
			  freetable(templatetable);
			}
#line 1923 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 3:
#line 228 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { classtable = mktable((Table*)0);
			  enumtable = mktable((Table*)0);
			  typetable = mktable((Table*)0);
			  booltable = mktable((Table*)0);
			  templatetable = mktable((Table*)0);
			  p = enter(booltable, lookup("false"));
			  p->info.typ = mkint();
			  p->info.val.i = 0;
			  p = enter(booltable, lookup("true"));
			  p->info.typ = mkint();
			  p->info.val.i = 1;
			  mkscope(mktable(mktable((Table*)0)), 0);
			}
#line 1941 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 4:
#line 243 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { namespaceid = (yyvsp[-3].sym)->name; }
#line 1947 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 5:
#line 244 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 1953 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 6:
#line 246 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { add_soap();
			  if (!lflag)
			  {	add_qname();
			  	add_XML();
			  }
			}
#line 1964 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 7:
#line 252 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 1970 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 8:
#line 254 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 1976 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 9:
#line 255 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 1982 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 10:
#line 256 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("input before ; skipped");
			  while (sp > stack)
			  {	freetable(sp->table);
			  	exitscope();
			  }
			  yyerrok;
			}
#line 1994 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 11:
#line 263 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2000 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 12:
#line 264 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2006 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 13:
#line 266 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].s)[1] >= 'a' && (yyvsp[0].s)[1] <= 'z')
			  {	for (pp = &pragmas; *pp; pp = &(*pp)->next)
			          ;
				*pp = (Pragma*)emalloc(sizeof(Pragma));
				(*pp)->pragma = (char*)emalloc(strlen((yyvsp[0].s))+1);
				strcpy((*pp)->pragma, (yyvsp[0].s));
				(*pp)->next = NULL;
			  }
			  else if ((i = atoi((yyvsp[0].s)+2)) > 0)
				yylineno = i;
			  else
			  {	sprintf(errbuf, "directive '%s' ignored (use #import to import files and/or use option -i)", (yyvsp[0].s));
			  	semwarn(errbuf);
			  }
			}
#line 2026 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 14:
#line 289 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { transient &= ~6;
			  permission = 0;
			}
#line 2034 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 15:
#line 293 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2040 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 16:
#line 295 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2046 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 17:
#line 297 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2052 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 18:
#line 299 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2058 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 19:
#line 301 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2064 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 20:
#line 302 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("declaration expected"); yyerrok; }
#line 2070 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 21:
#line 304 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { transient |= 1;
			}
#line 2077 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 22:
#line 307 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { transient &= ~1;
			}
#line 2084 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 23:
#line 310 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { permission = Sprivate;
			}
#line 2091 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 24:
#line 313 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { permission = Sprotected;
			}
#line 2098 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 25:
#line 316 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { permission = 0;
			}
#line 2105 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 26:
#line 319 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2111 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 27:
#line 320 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2117 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 28:
#line 322 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2123 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 29:
#line 323 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2129 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 30:
#line 324 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2135 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 31:
#line 325 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2141 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 32:
#line 327 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2147 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 33:
#line 330 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (((yyvsp[-3].rec).sto & Stypedef) && sp->table->level == GLOBAL)
			  {	if (((yyvsp[-3].rec).typ->type != Tstruct && (yyvsp[-3].rec).typ->type != Tunion && (yyvsp[-3].rec).typ->type != Tenum) || strcmp((yyvsp[-4].sym)->name, (yyvsp[-3].rec).typ->id->name))
				{	p = enter(typetable, (yyvsp[-4].sym));
					p->info.typ = mksymtype((yyvsp[-3].rec).typ, (yyvsp[-4].sym));
			  		if ((yyvsp[-3].rec).sto & Sextern)
						p->info.typ->transient = -1;
					else
						p->info.typ->transient = (yyvsp[-3].rec).typ->transient;
			  		p->info.sto = (yyvsp[-3].rec).sto;
					p->info.typ->pattern = (yyvsp[-1].rec).pattern;
					if ((yyvsp[-1].rec).minOccurs != -1)
					{	p->info.typ->minLength = (yyvsp[-1].rec).minOccurs;
					}
					if ((yyvsp[-1].rec).maxOccurs > 1)
						p->info.typ->maxLength = (yyvsp[-1].rec).maxOccurs;
				}
				(yyvsp[-4].sym)->token = TYPE;
			  }
			  else
			  {	p = enter(sp->table, (yyvsp[-4].sym));
			  	p->info.typ = (yyvsp[-3].rec).typ;
			  	p->info.sto = ((yyvsp[-3].rec).sto | permission);
				if ((yyvsp[0].rec).hasval)
				{	p->info.hasval = True;
					switch ((yyvsp[-3].rec).typ->type)
					{	case Tchar:
						case Tuchar:
						case Tshort:
						case Tushort:
						case Tint:
						case Tuint:
						case Tlong:
						case Tulong:
						case Tllong:
						case Tullong:
						case Tenum:
						case Ttime:
							if ((yyvsp[0].rec).typ->type == Tint || (yyvsp[0].rec).typ->type == Tchar || (yyvsp[0].rec).typ->type == Tenum)
								sp->val = p->info.val.i = (yyvsp[0].rec).val.i;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
						case Tfloat:
						case Tdouble:
						case Tldouble:
							if ((yyvsp[0].rec).typ->type == Tfloat || (yyvsp[0].rec).typ->type == Tdouble || (yyvsp[0].rec).typ->type == Tldouble)
								p->info.val.r = (yyvsp[0].rec).val.r;
							else if ((yyvsp[0].rec).typ->type == Tint)
								p->info.val.r = (double)(yyvsp[0].rec).val.i;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
						default:
							if ((yyvsp[-3].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[-3].rec).typ->ref)->type == Tchar
							 && (yyvsp[0].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[0].rec).typ->ref)->type == Tchar)
								p->info.val.s = (yyvsp[0].rec).val.s;
							else if (bflag
							 && (yyvsp[-3].rec).typ->type == Tarray
							 && ((Tnode*)(yyvsp[-3].rec).typ->ref)->type == Tchar
							 && (yyvsp[0].rec).typ->type == Tpointer
							 && ((Tnode*)(yyvsp[0].rec).typ->ref)->type == Tchar)
							{	if ((yyvsp[-3].rec).typ->width / ((Tnode*)(yyvsp[-3].rec).typ->ref)->width - 1 < strlen((yyvsp[0].rec).val.s))
								{	semerror("char[] initialization constant too long");
									p->info.val.s = "";
								}

								else
									p->info.val.s = (yyvsp[0].rec).val.s;
							}
							else if ((yyvsp[-3].rec).typ->type == Tpointer
							      && ((Tnode*)(yyvsp[-3].rec).typ->ref)->id == lookup("std::string"))
							      	p->info.val.s = (yyvsp[0].rec).val.s;
							else if ((yyvsp[-3].rec).typ->id == lookup("std::string"))
							      	p->info.val.s = (yyvsp[0].rec).val.s;
							else if ((yyvsp[-3].rec).typ->type == Tpointer
							      && (yyvsp[0].rec).typ->type == Tint
							      && (yyvsp[0].rec).val.i == 0)
								p->info.val.i = 0;
							else
							{	semerror("type error in initialization constant");
								p->info.hasval = False;
							}
							break;
					}
				}
				else
					p->info.val.i = sp->val;
			        if ((yyvsp[-1].rec).minOccurs < 0)
			        {	if (((yyvsp[-3].rec).sto & Sattribute) || (yyvsp[-3].rec).typ->type == Tpointer || (yyvsp[-3].rec).typ->type == Ttemplate || !strncmp((yyvsp[-4].sym)->name, "__size", 6))
			        		p->info.minOccurs = 0;
			        	else
			        		p->info.minOccurs = 1;
				}
				else
					p->info.minOccurs = (yyvsp[-1].rec).minOccurs;
				p->info.maxOccurs = (yyvsp[-1].rec).maxOccurs;
				if (sp->mask)
					sp->val <<= 1;
				else
					sp->val++;
			  	p->info.offset = sp->offset;
				if ((yyvsp[-3].rec).sto & Sextern)
					p->level = GLOBAL;
				else if ((yyvsp[-3].rec).sto & Stypedef)
					;
			  	else if (sp->grow)
					sp->offset += p->info.typ->width;
				else if (p->info.typ->width > sp->offset)
					sp->offset = p->info.typ->width;
			  }
			  sp->entry = p;
			}
#line 2270 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 34:
#line 449 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].rec).sto & Stypedef)
			  {	sprintf(errbuf, "invalid typedef qualifier for '%s'", (yyvsp[0].sym)->name);
				semwarn(errbuf);
			  }
			  p = enter(sp->table, (yyvsp[0].sym));
			  p->info.typ = (yyvsp[-1].rec).typ;
			  p->info.sto = (yyvsp[-1].rec).sto;
			  p->info.hasval = False;
			  p->info.offset = sp->offset;
			  if (sp->grow)
				sp->offset += p->info.typ->width;
			  else if (p->info.typ->width > sp->offset)
				sp->offset = p->info.typ->width;
			  sp->entry = p;
			}
#line 2290 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 35:
#line 465 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = (yyvsp[0].sym); }
#line 2296 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 36:
#line 466 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = (yyvsp[0].sym); }
#line 2302 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 37:
#line 468 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = (yyvsp[0].sym); }
#line 2308 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 38:
#line 469 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator!"); }
#line 2314 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 39:
#line 470 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator~"); }
#line 2320 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 40:
#line 471 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator="); }
#line 2326 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 41:
#line 472 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator+="); }
#line 2332 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 42:
#line 473 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator-="); }
#line 2338 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 43:
#line 474 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator*="); }
#line 2344 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 44:
#line 475 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator/="); }
#line 2350 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 45:
#line 476 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator%="); }
#line 2356 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 46:
#line 477 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator&="); }
#line 2362 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 47:
#line 478 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator^="); }
#line 2368 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 48:
#line 479 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator|="); }
#line 2374 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 49:
#line 480 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator<<="); }
#line 2380 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 50:
#line 481 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator>>="); }
#line 2386 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 51:
#line 482 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator||"); }
#line 2392 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 52:
#line 483 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator&&"); }
#line 2398 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 53:
#line 484 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator|"); }
#line 2404 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 54:
#line 485 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator^"); }
#line 2410 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 55:
#line 486 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator&"); }
#line 2416 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 56:
#line 487 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator=="); }
#line 2422 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 57:
#line 488 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator!="); }
#line 2428 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 58:
#line 489 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator<"); }
#line 2434 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 59:
#line 490 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator<="); }
#line 2440 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 60:
#line 491 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator>"); }
#line 2446 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 61:
#line 492 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator>="); }
#line 2452 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 62:
#line 493 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator<<"); }
#line 2458 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 63:
#line 494 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator>>"); }
#line 2464 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 64:
#line 495 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator+"); }
#line 2470 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 65:
#line 496 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator-"); }
#line 2476 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 66:
#line 497 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator*"); }
#line 2482 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 67:
#line 498 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator/"); }
#line 2488 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 68:
#line 499 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator%"); }
#line 2494 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 69:
#line 500 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator++"); }
#line 2500 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 70:
#line 501 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator--"); }
#line 2506 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 71:
#line 502 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator->"); }
#line 2512 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 72:
#line 503 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator[]"); }
#line 2518 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 73:
#line 504 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sym) = lookup("operator()"); }
#line 2524 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 74:
#line 505 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { s1 = c_storage((yyvsp[0].rec).sto);
			  s2 = c_type((yyvsp[0].rec).typ);
			  s = (char*)emalloc(strlen(s1) + strlen(s2) + 10);
			  strcpy(s, "operator ");
			  strcat(s, s1);
			  strcat(s, s2);
			  (yyval.sym) = lookup(s);
			  if (!(yyval.sym))
				(yyval.sym) = install(s, ID);
			}
#line 2539 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 75:
#line 516 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (!(p = entry(classtable, (yyvsp[0].sym))))
			  	semerror("invalid constructor");
			  sp->entry = enter(sp->table, (yyvsp[0].sym));
			  sp->entry->info.typ = mknone();
			  sp->entry->info.sto = Snone;
			  sp->entry->info.offset = sp->offset;
			  sp->node.typ = mkvoid();
			  sp->node.sto = Snone;
			}
#line 2553 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 76:
#line 527 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (!(p = entry(classtable, (yyvsp[0].sym))))
			  	semerror("invalid destructor");
			  s = (char*)emalloc(strlen((yyvsp[0].sym)->name) + 2);
			  strcpy(s, "~");
			  strcat(s, (yyvsp[0].sym)->name);
			  sym = lookup(s);
			  if (!sym)
				sym = install(s, ID);
			  sp->entry = enter(sp->table, sym);
			  sp->entry->info.typ = mknone();
			  sp->entry->info.sto = (yyvsp[-2].sto);
			  sp->entry->info.offset = sp->offset;
			  sp->node.typ = mkvoid();
			  sp->node.sto = Snone;
			}
#line 2573 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 77:
#line 544 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[-6].e)->level == GLOBAL)
			  {	if (!((yyvsp[-6].e)->info.sto & Sextern) && sp->entry && sp->entry->info.typ->type == Tpointer && ((Tnode*)sp->entry->info.typ->ref)->type == Tchar)
			  	{	sprintf(errbuf, "last output parameter of remote method function prototype '%s' is a pointer to a char which will only return one byte: use char** instead to return a string", (yyvsp[-6].e)->sym->name);
					semwarn(errbuf);
				}
				if ((yyvsp[-6].e)->info.sto & Sextern)
				 	(yyvsp[-6].e)->info.typ = mkmethod((yyvsp[-6].e)->info.typ, sp->table);
			  	else if (sp->entry && (sp->entry->info.typ->type == Tpointer || sp->entry->info.typ->type == Treference || sp->entry->info.typ->type == Tarray || is_transient(sp->entry->info.typ)))
				{	if ((yyvsp[-6].e)->info.typ->type == Tint)
					{	sp->entry->info.sto = (Storage)((int)sp->entry->info.sto | (int)Sreturn);
						(yyvsp[-6].e)->info.typ = mkfun(sp->entry);
						(yyvsp[-6].e)->info.typ->id = (yyvsp[-6].e)->sym;
						if (!is_transient(sp->entry->info.typ))
						{	if (!is_response(sp->entry->info.typ))
							{	if (!is_XML(sp->entry->info.typ))
									add_response((yyvsp[-6].e), sp->entry);
							}
							else
								add_result(sp->entry->info.typ);
						}
					}
					else
					{	sprintf(errbuf, "return type of remote method function prototype '%s' must be integer", (yyvsp[-6].e)->sym->name);
						semerror(errbuf);
					}
				}
			  	else
			  	{	sprintf(errbuf, "last output parameter of remote method function prototype '%s' is a return parameter and must be a pointer or reference, or use %s(void) for no return parameter", (yyvsp[-6].e)->sym->name, (yyvsp[-6].e)->sym->name);
					semerror(errbuf);
			  	}
				if (!((yyvsp[-6].e)->info.sto & Sextern))
			  	{	unlinklast(sp->table);
			  		if ((p = entry(classtable, (yyvsp[-6].e)->sym)))
					{	if (p->info.typ->ref)
						{	sprintf(errbuf, "remote method name clash: struct/class '%s' already declared at line %d", (yyvsp[-6].e)->sym->name, p->lineno);
							semerror(errbuf);
						}
						else
						{	p->info.typ->ref = sp->table;
							p->info.typ->width = sp->offset;
						}
					}
			  		else
			  		{	p = enter(classtable, (yyvsp[-6].e)->sym);
						p->info.typ = mkstruct(sp->table, sp->offset);
						p->info.typ->id = (yyvsp[-6].e)->sym;
			  		}
			  	}
			  }
			  else if ((yyvsp[-6].e)->level == INTERNAL)
			  {	(yyvsp[-6].e)->info.typ = mkmethod((yyvsp[-6].e)->info.typ, sp->table);
				(yyvsp[-6].e)->info.sto = (Storage)((int)(yyvsp[-6].e)->info.sto | (int)(yyvsp[-1].sto) | (int)(yyvsp[0].sto));
			  	transient &= ~1;
			  }
			  exitscope();
			}
#line 2634 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 78:
#line 601 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = sp->entry; }
#line 2640 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 79:
#line 603 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2646 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 80:
#line 604 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2652 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 81:
#line 606 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2658 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 82:
#line 607 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 2664 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 83:
#line 610 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[-2].rec).sto & Stypedef)
			  	semwarn("typedef in function argument");
			  p = enter(sp->table, (yyvsp[-3].sym));
			  p->info.typ = (yyvsp[-2].rec).typ;
			  p->info.sto = (yyvsp[-2].rec).sto;
			  if ((yyvsp[-1].rec).minOccurs < 0)
			  {	if (((yyvsp[-2].rec).sto & Sattribute) || (yyvsp[-2].rec).typ->type == Tpointer)
			        	p->info.minOccurs = 0;
			       	else
			        	p->info.minOccurs = 1;
			  }
			  else
				p->info.minOccurs = (yyvsp[-1].rec).minOccurs;
			  p->info.maxOccurs = (yyvsp[-1].rec).maxOccurs;
			  if ((yyvsp[0].rec).hasval)
			  {	p->info.hasval = True;
				switch ((yyvsp[-2].rec).typ->type)
				{	case Tchar:
					case Tuchar:
					case Tshort:
					case Tushort:
					case Tint:
					case Tuint:
					case Tlong:
					case Tulong:
					case Tenum:
					case Ttime:
						if ((yyvsp[0].rec).typ->type == Tint || (yyvsp[0].rec).typ->type == Tchar || (yyvsp[0].rec).typ->type == Tenum)
							sp->val = p->info.val.i = (yyvsp[0].rec).val.i;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
					case Tfloat:
					case Tdouble:
					case Tldouble:
						if ((yyvsp[0].rec).typ->type == Tfloat || (yyvsp[0].rec).typ->type == Tdouble || (yyvsp[0].rec).typ->type == Tldouble)
							p->info.val.r = (yyvsp[0].rec).val.r;
						else if ((yyvsp[0].rec).typ->type == Tint)
							p->info.val.r = (double)(yyvsp[0].rec).val.i;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
					default:
						if ((yyvsp[-2].rec).typ->type == Tpointer
						 && ((Tnode*)(yyvsp[-2].rec).typ->ref)->type == Tchar
						 && (yyvsp[0].rec).typ->type == Tpointer
						 && ((Tnode*)(yyvsp[0].rec).typ->ref)->type == Tchar)
							p->info.val.s = (yyvsp[0].rec).val.s;
						else if ((yyvsp[-2].rec).typ->type == Tpointer
						      && ((Tnode*)(yyvsp[-2].rec).typ->ref)->id == lookup("std::string"))
						      	p->info.val.s = (yyvsp[0].rec).val.s;
						else if ((yyvsp[-2].rec).typ->id == lookup("std::string"))
						      	p->info.val.s = (yyvsp[0].rec).val.s;
						else if ((yyvsp[-2].rec).typ->type == Tpointer
						      && (yyvsp[0].rec).typ->type == Tint
						      && (yyvsp[0].rec).val.i == 0)
							p->info.val.i = 0;
						else
						{	semerror("type error in initialization constant");
							p->info.hasval = False;
						}
						break;
				}
			  }
			  p->info.offset = sp->offset;
			  if ((yyvsp[-2].rec).sto & Sextern)
				p->level = GLOBAL;
			  else if (sp->grow)
				sp->offset += p->info.typ->width;
			  else if (p->info.typ->width > sp->offset)
				sp->offset = p->info.typ->width;
			  sp->entry = p;
			}
#line 2746 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 84:
#line 688 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (sp->table->level != PARAM)
			    (yyval.sym) = gensymidx("param", (int)++sp->val);
			  else if (eflag)
				(yyval.sym) = gensymidx("_param", (int)++sp->val);
			  else
				(yyval.sym) = gensym("_param");
			}
#line 2758 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 85:
#line 695 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (vflag != 1 && *(yyvsp[0].sym)->name == '_' && sp->table->level == GLOBAL)
			  { sprintf(errbuf, "SOAP 1.2 does not support anonymous parameters '%s'", (yyvsp[0].sym)->name);
			    semwarn(errbuf);
			  }
			  (yyval.sym) = (yyvsp[0].sym);
			}
#line 2769 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 86:
#line 711 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 2775 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 87:
#line 713 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 2781 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 88:
#line 715 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkint();
			  (yyval.rec).sto = Snone;
			  sp->node = (yyval.rec);
			}
#line 2790 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 89:
#line 719 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = (yyvsp[0].rec).typ;
			  (yyval.rec).sto = (Storage)((int)(yyvsp[-1].sto) | (int)(yyvsp[0].rec).sto);
			  if (((yyval.rec).sto & Sattribute) && !is_primitive_or_string((yyvsp[0].rec).typ) && !is_stdstr((yyvsp[0].rec).typ) && !is_binary((yyvsp[0].rec).typ) && !is_external((yyvsp[0].rec).typ))
			  {	semwarn("invalid attribute type");
			  	(yyval.rec).sto &= ~Sattribute;
			  }
			  sp->node = (yyval.rec);
			  if ((yyvsp[-1].sto) & Sextern)
				transient = 0;
			}
#line 2805 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 90:
#line 729 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].typ)->type == Tint)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tshort:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tllong:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  default:	semwarn("illegal use of 'signed'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tuint)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = mkuchar(); break;
				  case Tshort:	(yyval.rec).typ = mkushort(); break;
				  case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkulong(); break;
				  case Tllong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'unsigned'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tlong)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkllong(); break;
				  case Tuint:	(yyval.rec).typ = mkulong(); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  case Tdouble:	(yyval.rec).typ = mkldouble(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tulong)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkullong(); break;
				  case Tuint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[0].rec).typ->type == Tint)
				(yyval.rec).typ = (yyvsp[-1].typ);
			  else
			  	semwarn("invalid type (missing ';' or type name used as non-type identifier?)");
			  (yyval.rec).sto = (yyvsp[0].rec).sto;
			  sp->node = (yyval.rec);
			}
#line 2856 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 91:
#line 776 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkint();
			  (yyval.rec).sto = (yyvsp[0].sto);
			  sp->node = (yyval.rec);
			  if ((yyvsp[0].sto) & Sextern)
				transient = 0;
			}
#line 2867 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 92:
#line 782 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = (yyvsp[0].typ);
			  (yyval.rec).sto = Snone;
			  sp->node = (yyval.rec);
			}
#line 2876 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 93:
#line 786 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = (yyvsp[0].rec).typ;
			  (yyval.rec).sto = (Storage)((int)(yyvsp[-1].sto) | (int)(yyvsp[0].rec).sto);
			  if (((yyval.rec).sto & Sattribute) && !is_primitive_or_string((yyvsp[0].rec).typ) && !is_stdstr((yyvsp[0].rec).typ) && !is_binary((yyvsp[0].rec).typ) && !is_external((yyvsp[0].rec).typ))
			  {	semwarn("invalid attribute type");
			  	(yyval.rec).sto &= ~Sattribute;
			  }
			  sp->node = (yyval.rec);
			  if ((yyvsp[-1].sto) & Sextern)
				transient = 0;
			}
#line 2891 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 94:
#line 796 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[-1].typ)->type == Tint)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tshort:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  case Tllong:	(yyval.rec).typ = (yyvsp[0].rec).typ; break;
				  default:	semwarn("illegal use of 'signed'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tuint)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tchar:	(yyval.rec).typ = mkuchar(); break;
				  case Tshort:	(yyval.rec).typ = mkushort(); break;
				  case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkulong(); break;
				  case Tllong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'unsigned'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tlong)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkllong(); break;
				  case Tuint:	(yyval.rec).typ = mkulong(); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  case Tdouble:	(yyval.rec).typ = mkldouble(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[-1].typ)->type == Tulong)
				switch ((yyvsp[0].rec).typ->type)
				{ case Tint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tlong:	(yyval.rec).typ = mkullong(); break;
				  case Tuint:	(yyval.rec).typ = (yyvsp[-1].typ); break;
				  case Tulong:	(yyval.rec).typ = mkullong(); break;
				  default:	semwarn("illegal use of 'long'");
						(yyval.rec).typ = (yyvsp[0].rec).typ;
				}
			  else if ((yyvsp[0].rec).typ->type == Tint)
				(yyval.rec).typ = (yyvsp[-1].typ);
			  else
			  	semwarn("invalid type");
			  (yyval.rec).sto = (yyvsp[0].rec).sto;
			  sp->node = (yyval.rec);
			}
#line 2942 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 95:
#line 843 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkvoid(); }
#line 2948 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 96:
#line 844 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkbool(); }
#line 2954 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 97:
#line 845 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkchar(); }
#line 2960 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 98:
#line 846 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkwchart(); }
#line 2966 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 99:
#line 847 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkshort(); }
#line 2972 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 100:
#line 848 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkint(); }
#line 2978 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 101:
#line 849 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mklong(); }
#line 2984 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 102:
#line 850 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkllong(); }
#line 2990 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 103:
#line 851 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkullong(); }
#line 2996 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 104:
#line 852 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkulong(); }
#line 3002 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 105:
#line 853 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkfloat(); }
#line 3008 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 106:
#line 854 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkdouble(); }
#line 3014 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 107:
#line 855 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkint(); }
#line 3020 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 108:
#line 856 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkuint(); }
#line 3026 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 109:
#line 857 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkuchar(); }
#line 3032 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 110:
#line 858 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkushort(); }
#line 3038 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 111:
#line 859 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkuint(); }
#line 3044 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 112:
#line 860 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mkulong(); }
#line 3050 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 113:
#line 861 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.typ) = mktimet(); }
#line 3056 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 114:
#line 863 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (!(p = entry(templatetable, (yyvsp[0].sym))))
			  {	p = enter(templatetable, (yyvsp[0].sym));
			  	p->info.typ = mktemplate(NULL, (yyvsp[0].sym));
			  	(yyvsp[0].sym)->token = TYPE;
			  }
			  (yyval.typ) = p->info.typ;
			}
#line 3068 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 115:
#line 871 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { sym = gensym("_Struct");
			  sprintf(errbuf, "anonymous class will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(classtable, sym)))
			  {	if (p->info.typ->ref || p->info.typ->type != Tclass)
				{	sprintf(errbuf, "class '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkclass((Table*)0, 0);
			  }
			  sym->token = TYPE;
			  sp->table->sym = sym;
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3094 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 116:
#line 893 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { p = reenter(classtable, (yyvsp[-4].e)->sym);
			  sp->table->sym = p->sym;
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = p->sym;
			  if (p->info.typ->base)
			  	sp->table->prev = (Table*)entry(classtable, p->info.typ->base)->info.typ->ref;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3109 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 117:
#line 904 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { p = reenter(classtable, (yyvsp[-6].e)->sym);
			  sp->table->sym = p->sym;
			  if (!(yyvsp[-4].e))
				semerror("invalid base class");
			  else
			  {	sp->table->prev = (Table*)(yyvsp[-4].e)->info.typ->ref;
				if (!sp->table->prev && !(yyvsp[-4].e)->info.typ->transient)
				{	sprintf(errbuf, "class '%s' has incomplete type", (yyvsp[-4].e)->sym->name);
					semerror(errbuf);
				}
			  	p->info.typ->base = (yyvsp[-4].e)->info.typ->id;
			  }
			  p->info.typ->ref = sp->table;
			  p->info.typ->width = sp->offset;
			  p->info.typ->id = p->sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3132 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 118:
#line 922 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyvsp[0].e)->info.typ->id = (yyvsp[0].e)->sym;
			  (yyval.typ) = (yyvsp[0].e)->info.typ;
			}
#line 3140 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 119:
#line 926 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (!(yyvsp[0].e))
				semerror("invalid base class");
			  else
			  {	if (!(yyvsp[0].e)->info.typ->ref && !(yyvsp[0].e)->info.typ->transient)
				{	sprintf(errbuf, "class '%s' has incomplete type", (yyvsp[0].e)->sym->name);
					semerror(errbuf);
				}
			  	(yyvsp[-2].e)->info.typ->base = (yyvsp[0].e)->info.typ->id;
			  }
			  (yyvsp[-2].e)->info.typ->id = (yyvsp[-2].e)->sym;
			  (yyval.typ) = (yyvsp[-2].e)->info.typ;
			}
#line 3157 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 120:
#line 939 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { sym = gensym("_Struct");
			  sprintf(errbuf, "anonymous struct will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(classtable, sym)))
			  {	if (p->info.typ->ref || p->info.typ->type != Tstruct)
				{	sprintf(errbuf, "struct '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkstruct(sp->table, sp->offset);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3183 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 121:
#line 961 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[-4].e)->sym)) && p->info.typ->ref)
			  {	if (is_mutable(p->info.typ))
			  	{	if (merge((Table*)p->info.typ->ref, sp->table))
					{	sprintf(errbuf, "member name clash in struct '%s' declared at line %d", (yyvsp[-4].e)->sym->name, p->lineno);
						semerror(errbuf);
					}
			  		p->info.typ->width += sp->offset;
				}
			  }
			  else
			  {	p = reenter(classtable, (yyvsp[-4].e)->sym);
			  	p->info.typ->ref = sp->table;
			  	p->info.typ->width = sp->offset;
			  	p->info.typ->id = p->sym;
			  }
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3206 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 122:
#line 979 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->type == Tstruct)
			  		(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'struct %s' redeclaration (line %d)", (yyvsp[0].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkstruct((Table*)0, 0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3226 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 123:
#line 994 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->type == Tstruct)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'struct %s' redeclaration (line %d)", (yyvsp[0].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkstruct((Table*)0, 0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3246 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 124:
#line 1010 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { sym = gensym("_Union");
			  sprintf(errbuf, "anonymous union will be named '%s'", sym->name);
			  semwarn(errbuf);
			  (yyval.typ) = mkunion(sp->table, sp->offset);
			  if ((p = entry(classtable, sym)))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "union or struct '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, sym);
				p->info.typ = mkunion(sp->table, sp->offset);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3273 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 125:
#line 1033 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[-4].sym))))
			  {	if (p->info.typ->ref || p->info.typ->type != Tunion)
			  	{	sprintf(errbuf, "union '%s' already declared at line %d", (yyvsp[-4].sym)->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p = reenter(classtable, (yyvsp[-4].sym));
					p->info.typ->ref = sp->table;
					p->info.typ->width = sp->offset;
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[-4].sym));
				p->info.typ = mkunion(sp->table, sp->offset);
			  }
			  p->info.typ->id = (yyvsp[-4].sym);
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3297 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 126:
#line 1052 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->type == Tunion)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'union %s' redeclaration (line %d)", (yyvsp[0].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkunion((Table*) 0, 0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3317 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 127:
#line 1067 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->type == Tunion)
					(yyval.typ) = p->info.typ;
			  	else
				{	sprintf(errbuf, "'union %s' redeclaration (line %d)", (yyvsp[0].sym)->name, p->lineno);
			  		semerror(errbuf);
			  		(yyval.typ) = mkint();
				}
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkunion((Table*) 0, 0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3337 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 128:
#line 1083 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { sym = gensym("_Enum");
			  sprintf(errbuf, "anonymous enum will be named '%s'", sym->name);
			  semwarn(errbuf);
			  if ((p = entry(enumtable, sym)))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 4; /* 4 = enum */
				}
			  }
			  else
			  {	p = enter(enumtable, sym);
				p->info.typ = mkenum(sp->table);
			  }
			  p->info.typ->id = sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3363 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 129:
#line 1105 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(enumtable, (yyvsp[-5].e)->sym)))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[-5].e)->sym->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 4; /* 4 = enum */
				}
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[-5].e)->sym);
				p->info.typ = mkenum(sp->table);
			  }
			  p->info.typ->id = (yyvsp[-5].e)->sym;
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3386 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 130:
#line 1124 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(enumtable, (yyvsp[-5].sym))))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[-5].sym)->name, p->lineno);
					semerror(errbuf);
				}
				else
				{	p->info.typ->ref = sp->table;
					p->info.typ->width = 8; /* 8 = mask */
				}
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[-5].sym));
				p->info.typ = mkmask(sp->table);
			  }
			  p->info.typ->id = (yyvsp[-5].sym);
			  (yyval.typ) = p->info.typ;
			  exitscope();
			}
#line 3409 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 131:
#line 1142 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(enumtable, (yyvsp[0].sym))))
			  	(yyval.typ) = p->info.typ;
			  else
			  {	p = enter(enumtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkenum((Table*)0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3422 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 132:
#line 1150 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(enumtable, (yyvsp[0].sym))))
				(yyval.typ) = p->info.typ;
			  else
			  {	p = enter(enumtable, (yyvsp[0].sym));
			  	(yyval.typ) = p->info.typ = mkenum((Table*)0);
				p->info.typ->id = (yyvsp[0].sym);
			  }
			}
#line 3435 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 133:
#line 1158 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(typetable, (yyvsp[0].sym))))
				(yyval.typ) = p->info.typ;
			  else if ((p = entry(classtable, (yyvsp[0].sym))))
			  	(yyval.typ) = p->info.typ;
			  else if ((p = entry(enumtable, (yyvsp[0].sym))))
			  	(yyval.typ) = p->info.typ;
			  else if ((yyvsp[0].sym) == lookup("std::string") || (yyvsp[0].sym) == lookup("std::wstring"))
			  {	p = enter(classtable, (yyvsp[0].sym));
				(yyval.typ) = p->info.typ = mkclass((Table*)0, 0);
			  	p->info.typ->id = (yyvsp[0].sym);
			  	p->info.typ->transient = -2;
			  }
			  else
			  {	sprintf(errbuf, "unknown type '%s'", (yyvsp[0].sym)->name);
				semerror(errbuf);
				(yyval.typ) = mkint();
			  }
			}
#line 3458 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 134:
#line 1177 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(templatetable, (yyvsp[-3].sym))))
				(yyval.typ) = mktemplate((yyvsp[-1].rec).typ, (yyvsp[-3].sym));
			  else
			  {	sprintf(errbuf, "invalid template '%s'", (yyvsp[-3].sym)->name);
				semerror(errbuf);
				(yyval.typ) = mkint();
			  }
			}
#line 3471 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 135:
#line 1186 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("malformed class definition (use spacing around ':' to separate derived : base)");
			  yyerrok;
			  (yyval.typ) = mkint();
			}
#line 3480 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 136:
#line 1191 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("malformed struct definition");
			  yyerrok;
			  (yyval.typ) = mkint();
			}
#line 3489 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 137:
#line 1196 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("malformed union definition");
			  yyerrok;
			  (yyval.typ) = mkint();
			}
#line 3498 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 138:
#line 1201 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { synerror("malformed enum definition");
			  yyerrok;
			  (yyval.typ) = mkint();
			}
#line 3507 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 139:
#line 1206 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->ref)
			   	{	if (!is_mutable(p->info.typ))
					{	sprintf(errbuf, "struct '%s' already declared at line %d", (yyvsp[0].sym)->name, p->lineno);
						semerror(errbuf);
					}
				}
				else
					p = reenter(classtable, (yyvsp[0].sym));
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
				p->info.typ = mkstruct((Table*)0, 0);
			  }
			  (yyval.e) = p;
			}
#line 3528 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 140:
#line 1223 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(classtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->ref)
			   	{	if (!is_mutable(p->info.typ))
					{	sprintf(errbuf, "class '%s' already declared at line %d", (yyvsp[0].sym)->name, p->lineno);
						semerror(errbuf);
					}
				}
				else
					p = reenter(classtable, (yyvsp[0].sym));
			  }
			  else
			  {	p = enter(classtable, (yyvsp[0].sym));
				p->info.typ = mkclass((Table*)0, 0);
				p->info.typ->id = p->sym;
			  }
			  (yyvsp[0].sym)->token = TYPE;
			  (yyval.e) = p;
			}
#line 3551 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 141:
#line 1242 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = entry(enumtable, (yyvsp[0].sym))))
			  {	if (p->info.typ->ref)
				{	sprintf(errbuf, "enum '%s' already declared at line %d", (yyvsp[0].sym)->name, p->lineno);
					semerror(errbuf);
				}
				/*
				else
					p = reenter(classtable, $2);
			  	*/
			  }
			  else
			  {	p = enter(enumtable, (yyvsp[0].sym));
				p->info.typ = mkenum(0);
			  }
			  (yyval.e) = p;
			}
#line 3572 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 142:
#line 1259 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 3578 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 143:
#line 1260 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 3584 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 144:
#line 1262 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = (yyvsp[0].e); }
#line 3590 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 145:
#line 1263 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = (yyvsp[0].e); }
#line 3596 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 146:
#line 1264 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = (yyvsp[0].e); }
#line 3602 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 147:
#line 1265 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = entry(classtable, (yyvsp[0].sym));
			  if (!(yyval.e))
			  {	p = entry(typetable, (yyvsp[0].sym));
			  	if (p && (p->info.typ->type == Tclass || p->info.typ->type == Tstruct))
					(yyval.e) = p;
			  }
			}
#line 3614 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 148:
#line 1272 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.e) = entry(classtable, (yyvsp[0].sym)); }
#line 3620 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 149:
#line 1274 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (transient == -2)
			  	transient = 0;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			}
#line 3631 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 150:
#line 1281 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (transient == -2)
			  	transient = 0;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->grow = False;
			}
#line 3643 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 151:
#line 1289 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->mask = True;
			  sp->val = 1;
			}
#line 3653 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 152:
#line 1295 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 3659 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 153:
#line 1296 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 3665 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 154:
#line 1298 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (sp->table->level == INTERNAL)
			  	transient |= 1;
			  permission = 0;
			  enterscope(mktable(NULL), 0);
			  sp->entry = NULL;
			  sp->table->level = PARAM;
			}
#line 3677 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 155:
#line 1306 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sauto; }
#line 3683 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 156:
#line 1307 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sregister; }
#line 3689 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 157:
#line 1308 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sstatic; }
#line 3695 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 158:
#line 1309 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sexplicit; }
#line 3701 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 159:
#line 1310 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sextern; transient = 1; }
#line 3707 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 160:
#line 1311 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Stypedef; }
#line 3713 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 161:
#line 1312 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Svirtual; }
#line 3719 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 162:
#line 1313 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sconst; }
#line 3725 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 163:
#line 1314 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sfriend; }
#line 3731 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 164:
#line 1315 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sinline; }
#line 3737 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 165:
#line 1316 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = SmustUnderstand; }
#line 3743 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 166:
#line 1317 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sreturn; }
#line 3749 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 167:
#line 1318 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sattribute;
			  if (eflag)
			   	semwarn("SOAP RPC encoding does not support XML attributes");
			}
#line 3758 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 168:
#line 1322 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sspecial; }
#line 3764 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 169:
#line 1323 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sextern; transient = -2; }
#line 3770 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 170:
#line 1325 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Snone; }
#line 3776 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 171:
#line 1326 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sconstobj; }
#line 3782 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 172:
#line 1328 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Snone; }
#line 3788 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 173:
#line 1329 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Sabstract; }
#line 3794 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 174:
#line 1331 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Snone; }
#line 3800 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 175:
#line 1332 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.sto) = Svirtual; }
#line 3806 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 176:
#line 1334 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = tmp = sp->node; }
#line 3812 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 177:
#line 1335 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { /* handle const pointers, such as const char* */
			  if (/*tmp.typ->type == Tchar &&*/ (tmp.sto & Sconst))
			  	tmp.sto = (tmp.sto & ~Sconst) | Sconstptr;
			  tmp.typ = mkpointer(tmp.typ);
			  tmp.typ->transient = transient;
			  (yyval.rec) = tmp;
			}
#line 3824 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 178:
#line 1342 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { tmp.typ = mkreference(tmp.typ);
			  tmp.typ->transient = transient;
			  (yyval.rec) = tmp;
			}
#line 3833 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 179:
#line 1347 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = tmp;	/* tmp is inherited */
			}
#line 3840 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 180:
#line 1350 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if (!bflag && (yyvsp[0].rec).typ->type == Tchar)
			  {	sprintf(errbuf, "char["SOAP_LONG_FORMAT"] will be serialized as an array of "SOAP_LONG_FORMAT" bytes: use soapcpp2 option -b to enable char[] string serialization or use char* for strings", (yyvsp[-2].rec).val.i, (yyvsp[-2].rec).val.i);
			  	semwarn(errbuf);
			  }
			  if ((yyvsp[-2].rec).hasval && (yyvsp[-2].rec).typ->type == Tint && (yyvsp[-2].rec).val.i > 0 && (yyvsp[0].rec).typ->width > 0)
				(yyval.rec).typ = mkarray((yyvsp[0].rec).typ, (int) (yyvsp[-2].rec).val.i * (yyvsp[0].rec).typ->width);
			  else
			  {	(yyval.rec).typ = mkarray((yyvsp[0].rec).typ, 0);
			  	semerror("undetermined array size");
			  }
			  (yyval.rec).sto = (yyvsp[0].rec).sto;
			}
#line 3857 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 181:
#line 1362 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkpointer((yyvsp[0].rec).typ); /* zero size array = pointer */
			  (yyval.rec).sto = (yyvsp[0].rec).sto;
			}
#line 3865 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 182:
#line 1366 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).typ->type == Tstruct || (yyvsp[0].rec).typ->type == Tclass)
				if (!(yyvsp[0].rec).typ->ref && !(yyvsp[0].rec).typ->transient && !((yyvsp[0].rec).sto & Stypedef))
			   	{	sprintf(errbuf, "struct/class '%s' has incomplete type", (yyvsp[0].rec).typ->id->name);
					semerror(errbuf);
				}
			  (yyval.rec) = (yyvsp[0].rec);
			}
#line 3877 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 183:
#line 1374 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).hasval = False; }
#line 3883 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 184:
#line 1375 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).hasval)
			  {	(yyval.rec).typ = (yyvsp[0].rec).typ;
				(yyval.rec).hasval = True;
				(yyval.rec).val = (yyvsp[0].rec).val;
			  }
			  else
			  {	(yyval.rec).hasval = False;
				semerror("initialization expression not constant");
			  }
			}
#line 3898 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 185:
#line 1386 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { }
#line 3904 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 186:
#line 1387 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { /* empty for now */ }
#line 3910 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 187:
#line 1390 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).minOccurs = -1;
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[0].s);
			}
#line 3919 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 188:
#line 1395 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).minOccurs = (long)(yyvsp[0].i);
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[-1].s);
			}
#line 3928 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 189:
#line 1400 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).minOccurs = (long)(yyvsp[-1].i);
			  (yyval.rec).maxOccurs = 1;
			  (yyval.rec).pattern = (yyvsp[-2].s);
			}
#line 3937 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 190:
#line 1405 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).minOccurs = (long)(yyvsp[-2].i);
			  (yyval.rec).maxOccurs = (long)(yyvsp[0].i);
			  (yyval.rec).pattern = (yyvsp[-3].s);
			}
#line 3946 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 191:
#line 1410 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).minOccurs = -1;
			  (yyval.rec).maxOccurs = (long)(yyvsp[0].i);
			  (yyval.rec).pattern = (yyvsp[-2].s);
			}
#line 3955 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 192:
#line 1415 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.s) = NULL; }
#line 3961 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 193:
#line 1416 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.s) = (yyvsp[0].s); }
#line 3967 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 194:
#line 1418 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3973 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 195:
#line 1419 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.i) = (yyvsp[0].i); }
#line 3979 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 196:
#line 1420 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.i) = -(yyvsp[0].i); }
#line 3985 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 197:
#line 1429 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 3991 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 198:
#line 1430 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 3997 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 199:
#line 1434 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = (yyvsp[-2].rec).typ;
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			}
#line 4006 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 201:
#line 1441 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4012 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 202:
#line 1444 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).hasval = False;
			  (yyval.rec).typ = mkint();
			}
#line 4020 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 203:
#line 1447 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4026 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 204:
#line 1449 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4032 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 205:
#line 1452 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).hasval = False;
			  (yyval.rec).typ = mkint();
			}
#line 4040 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 206:
#line 1455 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4046 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 207:
#line 1457 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4052 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 208:
#line 1460 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop("|", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4058 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 209:
#line 1461 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop("^", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4064 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 210:
#line 1462 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop("&", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4070 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 211:
#line 1463 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop("==", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4076 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 212:
#line 1464 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop("!=", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4082 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 213:
#line 1465 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop("<", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4088 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 214:
#line 1466 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop("<=", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4094 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 215:
#line 1467 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop(">", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4100 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 216:
#line 1468 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = relop(">=", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4106 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 217:
#line 1469 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop("<<", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4112 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 218:
#line 1470 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop(">>", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4118 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 219:
#line 1471 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = op("+", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4124 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 220:
#line 1472 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = op("-", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4130 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 221:
#line 1473 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = op("*", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4136 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 222:
#line 1474 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = op("/", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4142 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 223:
#line 1475 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = iop("%", (yyvsp[-2].rec), (yyvsp[0].rec)); }
#line 4148 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 224:
#line 1476 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4154 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 225:
#line 1479 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).hasval)
				(yyval.rec).val.i = !(yyvsp[0].rec).val.i;
			  (yyval.rec).typ = (yyvsp[0].rec).typ;
			  (yyval.rec).hasval = (yyvsp[0].rec).hasval;
			}
#line 4164 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 226:
#line 1484 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).hasval)
				(yyval.rec).val.i = ~(yyvsp[0].rec).val.i;
			  (yyval.rec).typ = (yyvsp[0].rec).typ;
			  (yyval.rec).hasval = (yyvsp[0].rec).hasval;
			}
#line 4174 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 227:
#line 1489 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).hasval) {
				if (integer((yyvsp[0].rec).typ))
					(yyval.rec).val.i = -(yyvsp[0].rec).val.i;
				else if (real((yyvsp[0].rec).typ))
					(yyval.rec).val.r = -(yyvsp[0].rec).val.r;
				else	typerror("string?");
			  }
			  (yyval.rec).typ = (yyvsp[0].rec).typ;
			  (yyval.rec).hasval = (yyvsp[0].rec).hasval;
			}
#line 4189 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 228:
#line 1499 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4195 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 229:
#line 1500 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((yyvsp[0].rec).typ->type == Tpointer) {
			  	(yyval.rec).typ = (Tnode*)(yyvsp[0].rec).typ->ref;
			  } else
			  	typerror("dereference of non-pointer type");
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			}
#line 4207 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 230:
#line 1507 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkpointer((yyvsp[0].rec).typ);
			  (yyval.rec).sto = Snone;
			  (yyval.rec).hasval = False;
			}
#line 4216 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 231:
#line 1512 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).hasval = True;
			  (yyval.rec).typ = mkint();
			  (yyval.rec).val.i = (yyvsp[-1].rec).typ->width;
			}
#line 4225 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 232:
#line 1516 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[0].rec); }
#line 4231 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 233:
#line 1519 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec) = (yyvsp[-1].rec); }
#line 4237 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 234:
#line 1520 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { if ((p = enumentry((yyvsp[0].sym))) == (Entry*) 0)
				p = undefined((yyvsp[0].sym));
			  else
			  	(yyval.rec).hasval = True;
			  (yyval.rec).typ = p->info.typ;
			  (yyval.rec).val = p->info.val;
			}
#line 4249 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 235:
#line 1527 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkint();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = (yyvsp[0].i);
			}
#line 4258 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 236:
#line 1531 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkint();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 0;
			}
#line 4267 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 237:
#line 1535 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkfloat();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.r = (yyvsp[0].r);
			}
#line 4276 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 238:
#line 1539 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkchar();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = (yyvsp[0].c);
			}
#line 4285 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 239:
#line 1543 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkstring();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.s = (yyvsp[0].s);
			}
#line 4294 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 240:
#line 1547 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkbool();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 0;
			}
#line 4303 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;

  case 241:
#line 1551 "soapcpp2_yacc.y" /* yacc.c:1646  */
    { (yyval.rec).typ = mkbool();
			  (yyval.rec).hasval = True;
			  (yyval.rec).val.i = 1;
			}
#line 4312 "soapcpp2_yacc.c" /* yacc.c:1646  */
    break;


#line 4316 "soapcpp2_yacc.c" /* yacc.c:1646  */
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
      yyerror (YY_("syntax error"));
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
        yyerror (yymsgp);
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
                  yystos[yystate], yyvsp);
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
  yyerror (YY_("memory exhausted"));
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
                  yytoken, &yylval);
    }
  /* Do not reclaim the symbols of the rule whose action triggered
     this YYABORT or YYACCEPT.  */
  YYPOPSTACK (yylen);
  YY_STACK_PRINT (yyss, yyssp);
  while (yyssp != yyss)
    {
      yydestruct ("Cleanup: popping",
                  yystos[*yyssp], yyvsp);
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
#line 1557 "soapcpp2_yacc.y" /* yacc.c:1906  */


/*
 * ???
 */
int
yywrap()
{	return 1;
}

/******************************************************************************\

	Support routines

\******************************************************************************/

static Node
op(const char *op, Node p, Node q)
{	Node	r;
	Tnode	*typ;
	r.typ = p.typ;
	r.sto = Snone;
	if (p.hasval && q.hasval) {
		if (integer(p.typ) && integer(q.typ))
			switch (op[0]) {
			case '|':	r.val.i = p.val.i |  q.val.i; break;
			case '^':	r.val.i = p.val.i ^  q.val.i; break;
			case '&':	r.val.i = p.val.i &  q.val.i; break;
			case '<':	r.val.i = p.val.i << q.val.i; break;
			case '>':	r.val.i = p.val.i >> q.val.i; break;
			case '+':	r.val.i = p.val.i +  q.val.i; break;
			case '-':	r.val.i = p.val.i -  q.val.i; break;
			case '*':	r.val.i = p.val.i *  q.val.i; break;
			case '/':	r.val.i = p.val.i /  q.val.i; break;
			case '%':	r.val.i = p.val.i %  q.val.i; break;
			default:	typerror(op);
			}
		else if (real(p.typ) && real(q.typ))
			switch (op[0]) {
			case '+':	r.val.r = p.val.r + q.val.r; break;
			case '-':	r.val.r = p.val.r - q.val.r; break;
			case '*':	r.val.r = p.val.r * q.val.r; break;
			case '/':	r.val.r = p.val.r / q.val.r; break;
			default:	typerror(op);
			}
		else	semerror("illegal constant operation");
		r.hasval = True;
	} else {
		typ = mgtype(p.typ, q.typ);
		r.hasval = False;
	}
	return r;
}

static Node
iop(const char *iop, Node p, Node q)
{	if (integer(p.typ) && integer(q.typ))
		return op(iop, p, q);
	typerror("integer operands only");
	return p;
}

static Node
relop(const char *op, Node p, Node q)
{	Node	r;
	Tnode	*typ;
	r.typ = mkint();
	r.sto = Snone;
	r.hasval = False;
	if (p.typ->type != Tpointer || p.typ != q.typ)
		typ = mgtype(p.typ, q.typ);
	return r;
}

/******************************************************************************\

	Scope management

\******************************************************************************/

/*
mkscope - initialize scope stack with a new table and offset
*/
static void
mkscope(Table *table, int offset)
{	sp = stack-1;
	enterscope(table, offset);
}

/*
enterscope - enter a new scope by pushing a new table and offset on the stack
*/
static void
enterscope(Table *table, int offset)
{	if (++sp == stack+MAXNEST)
		execerror("maximum scope depth exceeded");
	sp->table = table;
	sp->val = 0;
	sp->offset = offset;
	sp->grow = True;	/* by default, offset grows */
	sp->mask = False;
}

/*
exitscope - exit a scope by popping the table and offset from the stack
*/
static void
exitscope()
{	check(sp-- != stack, "exitscope() has no matching enterscope()");
}

/******************************************************************************\

	Undefined symbol

\******************************************************************************/

static Entry*
undefined(Symbol *sym)
{	Entry	*p;
	sprintf(errbuf, "undefined identifier '%s'", sym->name);
	semwarn(errbuf);
	p = enter(sp->table, sym);
	p->level = GLOBAL;
	p->info.typ = mkint();
	p->info.sto = Sextern;
	p->info.hasval = False;
	return p;
}

/*
mgtype - return most general type among two numerical types
*/
Tnode*
mgtype(Tnode *typ1, Tnode *typ2)
{	if (numeric(typ1) && numeric(typ2)) {
		if (typ1->type < typ2->type)
			return typ2;
	} else	typerror("non-numeric type");
	return typ1;
}

/******************************************************************************\

	Type checks

\******************************************************************************/

static int
integer(Tnode *typ)
{	switch (typ->type) {
	case Tchar:
	case Tshort:
	case Tint:
	case Tlong:	return True;
	default:	break;
	}
	return False;
}

static int
real(Tnode *typ)
{	switch (typ->type) {
	case Tfloat:
	case Tdouble:
	case Tldouble:	return True;
	default:	break;
	}
	return False;
}

static int
numeric(Tnode *typ)
{	return integer(typ) || real(typ);
}

static void
add_fault(Table *gt)
{ Table *t;
  Entry *p1, *p2, *p3, *p4;
  Symbol *s1, *s2, *s3, *s4;
  imported = NULL;
  s1 = lookup("SOAP_ENV__Code");
  p1 = entry(classtable, s1);
  if (!p1 || !p1->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p1)
    { p1 = enter(classtable, s1);
      p1->info.typ = mkstruct(t, 3*4);
      p1->info.typ->id = s1;
    }
    else
      p1->info.typ->ref = t;
    p2 = enter(t, lookup("SOAP_ENV__Value"));
    p2->info.typ = qname;
    p2->info.minOccurs = 0;
    p2 = enter(t, lookup("SOAP_ENV__Subcode"));
    p2->info.typ = mkpointer(p1->info.typ);
    p2->info.minOccurs = 0;
  }
  s2 = lookup("SOAP_ENV__Detail");
  p2 = entry(classtable, s2);
  if (!p2 || !p2->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p2)
    { p2 = enter(classtable, s2);
      p2->info.typ = mkstruct(t, 3*4);
      p2->info.typ->id = s2;
    }
    else
      p2->info.typ->ref = t;
    p3 = enter(t, lookup("__type"));
    p3->info.typ = mkint();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("fault"));
    p3->info.typ = mkpointer(mkvoid());
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("__any"));
    p3->info.typ = xml;
    p3->info.minOccurs = 0;
    custom_fault = 0;
  }
  s4 = lookup("SOAP_ENV__Reason");
  p4 = entry(classtable, s4);
  if (!p4 || !p4->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p4)
    { p4 = enter(classtable, s4);
      p4->info.typ = mkstruct(t, 4);
      p4->info.typ->id = s4;
    }
    else
      p4->info.typ->ref = t;
    p3 = enter(t, lookup("SOAP_ENV__Text"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
  }
  s3 = lookup("SOAP_ENV__Fault");
  p3 = entry(classtable, s3);
  if (!p3 || !p3->info.typ->ref)
  { t = mktable(NULL);
    if (!p3)
    { p3 = enter(classtable, s3);
      p3->info.typ = mkstruct(t, 9*4);
      p3->info.typ->id = s3;
    }
    else
      p3->info.typ->ref = t;
    p3 = enter(t, lookup("faultcode"));
    p3->info.typ = qname;
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("faultstring"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("faultactor"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("detail"));
    p3->info.typ = mkpointer(p2->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, s1);
    p3->info.typ = mkpointer(p1->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, s4);
    p3->info.typ = mkpointer(p4->info.typ);
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Node"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Role"));
    p3->info.typ = mkstring();
    p3->info.minOccurs = 0;
    p3 = enter(t, lookup("SOAP_ENV__Detail"));
    p3->info.typ = mkpointer(p2->info.typ);
    p3->info.minOccurs = 0;
  }
}

static void
add_soap()
{ Symbol *s = lookup("soap");
  p = enter(classtable, s);
  p->info.typ = mkstruct(NULL, 0);
  p->info.typ->transient = -2;
  p->info.typ->id = s;
}

static void
add_XML()
{ Symbol *s = lookup("_XML");
  p = enter(typetable, s);
  xml = p->info.typ = mksymtype(mkstring(), s);
  p->info.sto = Stypedef;
}

static void
add_qname()
{ Symbol *s = lookup("_QName");
  p = enter(typetable, s);
  qname = p->info.typ = mksymtype(mkstring(), s);
  p->info.sto = Stypedef;
}

static void
add_header(Table *gt)
{ Table *t;
  Entry *p;
  Symbol *s = lookup("SOAP_ENV__Header");
  imported = NULL;
  p = entry(classtable, s);
  if (!p || !p->info.typ->ref)
  { t = mktable((Table*)0);
    if (!p)
      p = enter(classtable, s);
    p->info.typ = mkstruct(t, 0);
    p->info.typ->id = s;
    custom_header = 0;
  }
}

static void
add_response(Entry *fun, Entry *ret)
{ Table *t;
  Entry *p, *q;
  Symbol *s;
  size_t n = strlen(fun->sym->name);
  char *r = (char*)emalloc(n+9);
  strcpy(r, fun->sym->name);
  strcat(r, "Response");
  if (!(s = lookup(r)))
    s = install(r, ID);
  free(r);
  t = mktable((Table*)0);
  q = enter(t, ret->sym);
  q->info = ret->info;
  if (q->info.typ->type == Treference)
    q->info.typ = (Tnode*)q->info.typ->ref;
  p = enter(classtable, s);
  p->info.typ = mkstruct(t, 4);
  p->info.typ->id = s;
  fun->info.typ->response = p;
}

static void
add_result(Tnode *typ)
{ Entry *p;
  if (!typ->ref || !((Tnode*)typ->ref)->ref)
  { semwarn("response struct/class must be declared before used in function prototype");
    return;
  }
  for (p = ((Table*)((Tnode*)typ->ref)->ref)->list; p; p = p->next)
    if (p->info.sto & Sreturn)
      return;
  for (p = ((Table*)((Tnode*)typ->ref)->ref)->list; p; p = p->next)
  { if (p->info.typ->type != Tfun && !(p->info.sto & Sattribute) && !is_transient(p->info.typ) && !(p->info.sto & (Sprivate|Sprotected)))
      p->info.sto = (Storage)((int)p->info.sto | (int)Sreturn);
      return;
  }
}

//						↓↓↓VULNERABLE LINES↓↓↓

// 2015,4;2015,10

// 2021,7;2021,14

// 2275,7;2275,14

// 2531,5;2531,11

// 2532,5;2532,11

// 2533,5;2533,11

// 2560,5;2560,11

// 2561,5;2561,11

// 2579,8;2579,15

// 2599,7;2599,14

// 2604,8;2604,15

// 2611,8;2611,15

// 2763,7;2763,14

// 3073,5;3073,12

// 3077,6;3077,13

// 3120,6;3120,13

// 3148,6;3148,13

// 3162,5;3162,12

// 3166,6;3166,13

// 3190,7;3190,14

// 3214,6;3214,13

// 3234,6;3234,13

// 3251,5;3251,12

// 3256,6;3256,13

// 3279,8;3279,15

// 3305,6;3305,13

// 3325,6;3325,13

// 3342,5;3342,12

// 3346,6;3346,13

// 3369,6;3369,13

// 3392,6;3392,13

// 3452,7;3452,14

// 3465,7;3465,14

// 3514,7;3514,14

// 3535,7;3535,14

// 3557,6;3557,13

// 3845,7;3845,14

// 3871,9;3871,16

// 4664,1;4664,8

// 4871,2;4871,8

// 4872,2;4872,8

