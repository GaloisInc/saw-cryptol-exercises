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
 * a 128-bit counter.
 */
typedef struct Ctr {
    uint64_t key[4];
    uint64_t counter[2];
} Ctr;


void encryptR(uint64_t* k, uint64_t* plaintext);

/**
 * Encrypt a single block in counter mode.
 */
void encryptCTROne(Ctr* context, uint64_t* plaintext) {
    uint64_t encrypted_counter[2] = {context -> counter[0], context -> counter[1]};
    // encrypt with the current counter.
    encryptR(context -> key, encrypted_counter);
    // update the context
    update_counter(context -> counter);

    // XOR with the plaintext
    plaintext[0] ^= encrypted_counter[0];
    plaintext[1] ^= encrypted_counter[1];
}

/**
 * Encrypt some n blocks in counter mode.
 */
void encryptCTR(Ctr* context, unsigned n, uint64_t* plaintext) {
    for (unsigned i = 0; i < n; i++) {
        encryptCTROne(context, plaintext);
        plaintext += 2;
    }
}

int main() {
    Ctr ctx = {.counter = {0}, .key = {0}};
    uint64_t pt[2] = {0};
    encryptCTROne(&ctx, pt);
    printf("%llu %llu\n", pt[0], pt[1]);
    printf("%llu %llu\n", ctx.counter[0], ctx.counter[1]);

    Ctr ctx1 = {.counter = {0xf6dc8b236b711d52, 0x672426f966dc9e74}, .key = {0x90bdf8f4bf4bb964, 0xb86444fbf46c4914, 0x84059a91ae5e68e0, 0x21ffd423664b32cc}};
    uint64_t pt1[2] = {0x54c492564ac6e1a4, 0x67c666c8808992ed};
    encryptCTROne(&ctx1, pt1);
    printf("%llu %llu\n", pt1[0], pt1[1]);
    printf("%llu %llu\n", ctx1.counter[0], ctx1.counter[1]);

    return 0;
}