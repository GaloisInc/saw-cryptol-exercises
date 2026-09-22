#include <stdint.h>

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
    uint64_t counter;
} Ctr;

/**
 * Return the "current" counter,
 * and then update.
 *
 * Does not check for overflow.
 */
uint64_t update_counter(struct Ctr* ctr) {
    uint64_t c = ctr -> counter;
    ctr -> counter += 1;
    return c;
}

/**
 * Represent a 128 bit unsigned
 * integer as two 64 bit chunks.
 * We could manipulate pointers,
 * as we've seen before, but I'm
 * going to formulate things with
 * this struct to change things up.
 */
typedef struct U128 {
    uint64_t lo;
    uint64_t hi;
} U128;

/**
 * This "trivial" f doesn't utilize the
 * key and doesn't transform the block.
 * This would be a "terrible" choice
 * of function to use in CTR mode,
 * but whatever.
 */
U128 f(uint64_t key[4], uint64_t counter) {
    U128 transformed = {.lo = counter, .hi = 0};
    return transformed;
}

/**
 * Counter mode works by applying some
 * function to the counter, and XORing
 * that with the plaintext. Here, we make
 * a stub f to stand-in for "any" block
 * cipher. This is probably seeming pretty
 * funky right now, and it is a bit contrived.
 * We'll see its utility when we head over
 * to SAW.
 *
 * Notice that counter mode doesn't even
 * require that f be invertible. That's
 * pretty crazy! Although it would be
 * much slower than using AES, one can
 * feasibly use a keyed hash like SHA-256-HMAC
 * in a counter mode setting.
 */
U128 applyF(Ctr* ctx) {
    // in "real life", we use a function
    // like AES to encrypt the counter.
    // Here, we will just do something
    // insecure and contrived.
    uint64_t current = update_counter(ctx);
    return f(ctx -> key, current);
}

/**
 * Really, this interface consumes anything
 * that produces a U128 from the context.
 *
 * Here, we hardcode f.
 */
U128 encrypt(Ctr* ctx, U128 plaintext) {
    U128 ctrEnc = applyF(ctx);
    // here is the "meat" of CTR mode...
    // we just XOR the output with the plaintext.
    U128 ciphertext = {.lo = ctrEnc.lo ^ plaintext.lo, .hi = ctrEnc.hi ^ plaintext.hi};
    return ciphertext;
}

/**
 * Cute.
 */
U128 decrypt(Ctr* ctx, U128 ciphertext) {
    return encrypt(ctx, ciphertext);
}

/**
 * Encrypt a bunch of blocks!
 */
void ctrOneShot(uint64_t key[4], unsigned n, U128* blocks) {
    // we agree on a start counter...
    // for all intents and purposes, this is 0.
    Ctr ctx = {.counter = 0, .key = *key};
    for (unsigned i = 0; i < n; i++) {
        encrypt(&ctx, blocks[i]);
    }
}

/**
 * Here is a more "morally  and philisophically upstanding"
 * version of counter, that accepts a function pointer.
 * In this version, we dispense with the caveats of
 * in the function f. We really _could_ call this with
 * some one block AES (ECB mode, ish), say...
 */
U128 genericF(Ctr* ctx, U128 (*fcn)(uint64_t*, U128)) {
    uint64_t x = update_counter(ctx);
    // one tedious point...we pad
    // the counter so it fits the
    // type of fcn.
    U128 fcnInput = {.hi = 0, .lo = x};
    // now, we call "whatever" fcn is
    // to get the "thing" we shoud XOR
    // into the plaintext.
    return fcn(ctx -> key, fcnInput);
}

/**
 * Do one block of counter mode with whatever the cipher is.
 */
U128 ctrOne(U128 (*cipher)(uint64_t*, U128), Ctr* ctx, U128 plaintext) {
    U128 x = genericF(ctx, cipher);
    U128 ciphertext = {.hi = plaintext.hi ^ x.hi, .lo = plaintext.lo ^ x.lo};
    return ciphertext;
}

void ctrGenericOneShot(U128 (*cipher)(uint64_t*, U128), uint64_t key[4], unsigned n, U128* blocks) {
    Ctr ctx = {.key = *key, .counter = 0};
    for (unsigned i = 0; i < n; i++) {
        ctrOne(cipher, &ctx, blocks[i]);
    }
}

int main() {
    return 0;
}