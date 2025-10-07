#include "int.h"
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>

Int_Err Int_init(Int* out, const char* num) {
    if (!out || !num)
        return INT_ERR_INVALID_VALUE;

    out->val[0] = 0;
    out->val[1] = 0;

    while (*num) {
        char c = *num++;

        if (c < '0' || c > '9')
            return INT_ERR_INVALID_VALUE;

        char digit = c - '0';

        // Multiply current value by 10
        // We'll simulate 16-bit value with two unsigned chars
        unsigned char hi = out->val[0];
        unsigned char lo = out->val[1];

        // Multiply by 10: val = val * 10 = val * (8 + 2)
        // So we do: val << 3 (x8) and val << 1 (x2), then add them

        // First: shift left by 1 (val * 2)
        unsigned char lo_x2 = lo << 1;
        unsigned char hi_x2 = (hi << 1) | ((lo & 0x80) >> 7);

        // Then: shift left by 3 (val * 8)
        unsigned char lo_x8 = lo << 3;
        unsigned char hi_x8 = (hi << 3) | ((lo & 0xE0) >> 5);

        // Now add the two (x2 + x8) = x10
        unsigned char new_lo = lo_x2 + lo_x8;
        unsigned char carry = (new_lo < lo_x2) ? 1 : 0;

        unsigned char new_hi = hi_x2 + hi_x8 + carry;
        if (new_hi < hi_x2 || (new_hi == 0xFF && digit > 5)) // prevent overflow
            return INT_ERR_OVERFLOW;

        // Now add the digit to new_lo
        carry = 0;
        unsigned char final_lo = new_lo + digit;
        if (final_lo < new_lo) carry = 1;

        unsigned char final_hi = new_hi + carry;
        if (final_hi < new_hi)
            return INT_ERR_OVERFLOW;

        // Final overflow check: result must fit in 16 bits (max 65535)
        if (final_hi > 0xFF || (final_hi == 0xFF && final_lo > 0xFF))
            return INT_ERR_OVERFLOW;

        out->val[0] = final_hi;
        out->val[1] = final_lo;
    }

    return INT_ERR_NONE;
}

Int_Err Int_get(const Int* v, char* buf, size_t bufsize) {
    if (!v || !buf || bufsize == 0)
        return INT_ERR_INVALID_VALUE;

    // Copy bytes for manipulation (big-endian: val[0] MSB, val[1] LSB)
    unsigned char bytes[2] = { v->val[0], v->val[1] };

    char digits[5]; // max 5 digits for 65535
    int i = 0;

    // Special case: zero
    if (bytes[0] == 0 && bytes[1] == 0) {
        if (bufsize < 2) // '0' + '\0'
            return INT_ERR_INVALID_VALUE;
        buf[0] = '0';
        buf[1] = '\0';
        return INT_ERR_NONE;
    }

    // Repeated division by 10 (process MSB first)
    while (bytes[0] != 0 || bytes[1] != 0) {
        unsigned int remainder = 0;

        for (size_t j = 0; j < 2; j++) {
            unsigned int current = (remainder << 8) | bytes[j];
            bytes[j] = (unsigned char)(current / 10);
            remainder = current % 10;
        }

        if (i >= 5) // Overflow in digit array (shouldn't happen)
            return INT_ERR_OVERFLOW;

        digits[i++] = '0' + remainder;
    }

    // Check buffer size (digits + null terminator)
    if (bufsize <= i)
        return INT_ERR_INVALID_VALUE;

    // Reverse digits into output buffer
    for (int j = 0; j < i; j++) {
        buf[j] = digits[i - j - 1];
    }
    buf[i] = '\0';

    return INT_ERR_NONE;
}

