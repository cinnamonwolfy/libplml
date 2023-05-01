#pragma once

#include <pl32.h>

typedef enum plmlerror {
    PLML_SUCCESS,
    PLML_FILE,
    PLML_INVALID,
} plmlerror_t;

typedef enum plmltype {
    PLML_TYPE_STRING,
    PLML_TYPE_BOOL,
    PLML_TYPE_INT,
    PLML_TYPE_HEADER,
} plmltype_t;

typedef enum plmlaction {
    PLML_GET_NAME,
    PLML_GET_TYPE,
    PLML_GET_VALUE,
} plmlaction_t;

typedef struct plmltoken plmltoken_t;

void plMLError(string_t name, plmlerror_t errVal, plmt_t* mt);
plmltoken_t* plMLParse(string_t string, plmt_t* mt);
void plMLGetTokenAttrib(plmltoken_t* token, memptr_t ptr, plmlaction_t attribType);
void plMLFreeToken(plmltoken_t* token);
