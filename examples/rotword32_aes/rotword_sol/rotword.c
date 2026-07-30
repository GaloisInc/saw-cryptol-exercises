#include <stdint.h>
/** Rotate a 32-bit word left by 8 bits. */
uint32_t rot_word(uint32_t w) {
    return (w << 8) | (w >> 24);
}
