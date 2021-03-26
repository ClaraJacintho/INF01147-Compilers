#define ERR_UNDECLARED       10
#define ERR_DECLARED         11
#define ERR_VARIABLE         20
#define ERR_VECTOR           21
#define ERR_FUNCTION         22
#define ERR_WRONG_TYPE       30
#define ERR_STRING_TO_X      31
#define ERR_CHAR_TO_X        32
#define ERR_STRING_MAX       33
#define ERR_STRING_VECTOR    34
#define ERR_MISSING_ARGS     40
#define ERR_EXCESS_ARGS      41
#define ERR_WRONG_TYPE_ARGS  42
#define ERR_FUNCTION_STRING  43
#define ERR_WRONG_PAR_INPUT  50
#define ERR_WRONG_PAR_OUTPUT 51
#define ERR_WRONG_PAR_RETURN 52
#define ERR_WRONG_PAR_SHIFT  53

char* get_type_name(type_t t);
void throw_declared_error(symbol_t* a, symbol_t* b);
void throw_undeclared_error(int line, char* id);
void throw_error(int error, symbol_t *a, symbol_t *b);
void throw_wrong_type_error(int line, char* key, type_t a, type_t b);
void throw_func_string_error(int line);
