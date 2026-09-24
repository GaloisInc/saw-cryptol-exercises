#include <stdint.h>
#include <stdio.h>

/**
 * Increment with carry "big endian"
 */
void update_counter(uint64_t x[2]) {
    // add one!
    // do funky bit operations to calculate
    // if there is a carry bit in "constant time".
    uint64_t r = x[1] + 1;
    uint64_t is_nonzero = (r | (0 - r)) >> 63;
    uint64_t mask = 0 - is_nonzero;
    uint64_t carry = 1 ^ (mask & 1);
    x[1] = r;
    x[0] = x[0] + carry;
}

/**
 * A counter mode "context"
 *
 * This has a 256-bit key and
 * a 64 bit counter. The specification
 * has a 128 bit counter...we deviate
 * for ease of arithmetic, and practicality...
 * 64 * 128 bits is a truly massive amount
 * of data we assume we won't see in the
 * wild. We can specify this assumption
 * in our proof of equivalence.
 */
typedef struct Ctr {
    uint64_t key[4];
    uint64_t counter[2];
} Ctr;


void encryptR(uint64_t* k, uint64_t* plaintext);

int main() {
    return 0;
}