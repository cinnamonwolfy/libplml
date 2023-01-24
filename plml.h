#include <pl32.h>
#define PLML_FILE 1
#define PLML_INVALID 2

#define PLML_TYPE_STRING 8
#define PLML_TYPE_BOOL 9
#define PLML_TYPE_INT 10
#define PLML_TYPE_HEADER 11

typedef struct plmltoken plmltoken_t;

plmltoken_t* plMLParse(string_t string, plmt_t* mt);
void plMLFreeToken(plmltoken_t* token);
