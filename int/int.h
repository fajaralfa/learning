#include <stddef.h>
#include <stdbool.h>

#ifndef INT_TYPE
#define INT_TYPE

typedef enum {
    INT_ERR_NONE,
    INT_ERR_OVERFLOW,
    INT_ERR_UNDERFLOW,
    INT_ERR_INVALID_VALUE,
} Int_Err;

typedef struct Int {
    unsigned char val[2];
} Int;

Int_Err Int_init(Int* out, const char* num);
Int_Err Int_get(const Int *v, char* buf, size_t bufsize);
Int_Err Int_add(Int* target, const Int* n1, const Int* n2);

#endif