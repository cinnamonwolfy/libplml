#include <pl32.h>
#define PLML_FILE 1
#define PLML_INVALID 2

#define PLML_TYPE_STRING 8
#define PLML_TYPE_BOOL 9
#define PLML_TYPE_INT 10
#define PLML_TYPE_HEADER 11

#define PLML_GET_NAME 16
#define PLML_GET_TYPE 17
#define PLML_GET_VALUE 18

typedef struct plmltoken plmltoken_t;

void plMLError(string_t name, int errNum, plmt_t* mt);
plmltoken_t* plMLParse(string_t string, plmt_t* mt);
void plMLGetTokenAttrib(plmltoken_t* token, memptr_t ptr, int attribType);
void plMLFreeToken(plmltoken_t* token);
