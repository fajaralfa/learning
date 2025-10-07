#include <stdio.h>
#include "int.h"

int main() {
    Int v1;
    Int_init(&v1, "1234");
    int v1valuesize = 5;
    char v1value[v1valuesize];
    Int_get(&v1, v1value, v1valuesize);
    printf("value: %s\n", v1value);
}
