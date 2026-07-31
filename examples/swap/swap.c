#include <stdlib.h>

/**
 * Swap the values pointed to by
 * x and y.
 *
 * What correctness properties does this
 * function assume?
 */
void swap(int* x, int* y) {
    int tmp = *x;
    *x = *y;
    *y = tmp;
}

/**
 * Swap the values at index i and j of
 * a contiguous "region" of ints.
 *
 * What correctness properties does this
 * function assume?
 */
void swapIJ(int* p, unsigned i, unsigned j) {
    int tmp = p[i];
    p[i] = p[j];
    p[j] = tmp;
}

int main() {

    // make some ints
    int x = 3;
    int y = 4;
    int* xp = &x;
    int* yp = &y;

    // use swap in a silly way.
    // is this _ok_?
    swap(xp, xp);

    // use swap hideously.
    // swap(xp, NULL);

    return 0;
}