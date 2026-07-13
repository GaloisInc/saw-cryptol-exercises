#include <stdint.h>

uint8_t rotl(uint8_t x, uint8_t r) {
    x &= 127;
    r %= 7;
    return ((x << r) & 0x7F) | ((x >> (7 - r)) & 0x3F);
}

uint8_t rotl_needs_precond(uint8_t x, uint8_t r) {
    x &= 127;
    return ((x << r) & 0x7F) | ((x >> (7 - r)) & 0x3F);
}

int main() {
    return 0;
}