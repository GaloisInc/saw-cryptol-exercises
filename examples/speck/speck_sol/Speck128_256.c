/**
 * A C implementation of Speck128_256.
 */

#include <stdint.h>

/**
 * Left circular shift by `j`.
 */
uint64_t Sj(uint64_t x, unsigned j) {
    return (x << j) | (x >> (64 - j));
}

/**
 * Right circular shift by `j`.
 */
uint64_t Sji(uint64_t x, unsigned j) {
    return (x >> j) | (x << (64 - j));
}

/**
 * Rotation by Speck constant "alpha".
 */
uint64_t Sa(uint64_t x) {
    return Sj(x, 8);
}

/**
 * Rotation by Speck constant "beta".
 */
uint64_t Sb(uint64_t x) {
    return Sj(x, 3);
}

/**
 * The inverse of rotation by alpha.
 */
uint64_t Sai(uint64_t x) {
    return Sji(x, 8);
}

/**
 * The inverse of rotation by beta.
 */
uint64_t Sbi(uint64_t x) {
    return Sji(x, 3);
}

/**
 * The Speck round function. The first
 * parameter is the round key. The second
 * parameter is the 4 word state array.
 */
void R(uint64_t k, uint64_t* p) {
    uint64_t x = p[0];
    uint64_t y = p[1];
    p[0] = (Sai(x) + y) ^ k;
    p[1] = Sb(y) ^ (Sai(x) + y) ^ k;
}

/**
 * A structure that expands the Speck
 * key "online" rather than pre-computing
 * it.
 */
typedef struct Expander {
    uint64_t l0;
    uint64_t l1;
    uint64_t l2;
    uint64_t k0;
} Expander;

/**
 * Initialize an expander with the words of a key in order.
 */
Expander initializeExpander(uint64_t* key) {
    Expander e = {.k0 = key[0], .l0 = key[1], .l1 = key[2], .l2 = key[3]};
    return e;
}

/**
 * Get the round key for the ith round.
 */
uint64_t Rk(Expander* e, uint64_t i) {
    // the round key is the current k
    uint64_t k0 = e -> k0;
    // compute the next round key.
    uint64_t l3 = (k0 + Sai(e -> l0)) ^ i;
    uint64_t k1 = Sb(k0) ^ l3;
    // reset the expander state accordingly.
    e->k0 = k1;
    e->l0 = e->l1;
    e->l1 = e->l2;
    e->l2 = l3;
    // return the round key.
    return k0;
}

/**
 * Compute the key schedule for a key.
 */
void keyExpansion(uint64_t* key, uint64_t* expanded_key) {
    Expander e = initializeExpander(key);
    for (unsigned i = 0; i < 34; i++) {
        expanded_key[i] = Rk(&e, i);
    }
}

/**
 * Compute one block of Speck encryption.
 * This computes the key schedule each block.
 * It also expects the space for the expanded key
 * to be pre-allocated.
 *
 * Note: the Speck Cryptol specification reverses the key.
 * This does not.
 */
void encrypt(uint64_t* key, uint64_t* plaintext, uint64_t* expk) {
    keyExpansion(key, expk);
    for (unsigned i = 0; i < 34; i++) {
        R(expk[i], plaintext);
    }
}

/**
 * Computes one block of Speck encryption.
 * This computes the key schedule each block
 * but does not require a pre-allocatoion for the
 * expanded key.
 *
 * This reverses the key like the Cryptol spec does.
 */
void encryptR(uint64_t* key, uint64_t* plaintext) {
    uint64_t reverse_key[4] = {key[3], key[2], key[1], key[0]};
    uint64_t expanded_key[34] = {0};
    encrypt(reverse_key, plaintext, expanded_key);
}

/**
 * Compute Speck with online key schedule generation.
 */
void encryptOnline(uint64_t* key, uint64_t* plaintext) {
    Expander e = initializeExpander(key);
    for (unsigned i = 0; i < 34; i++) {
        R(Rk(&e, i), plaintext);
    }
}

int main() {
    return 0;
}