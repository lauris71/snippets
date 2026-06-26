#define __CONSTANT_TIME_C__

#include <stdint.h>
#include <stdio.h>

/**
 * @brief Check if an 8-bit value is zero.
 * 
 * @param v The value to check
 * @return 0xff if v is zero, 0x00 otherwise
 */
uint8_t
ct_zero8(uint8_t v) {
    uint16_t x = v;
    return (uint8_t) (((x - 1) >> 8) & 0xff);
}

/**
 * @brief Check if an 64-bit value is zero
 * 
 * @param v The value to check
 * @return UINT64_MAX if v is zero, 0x00 otherwise
 */
uint64_t
ct_zero64(uint64_t v) {
    uint8_t x8 = ((v >> 56) | (v >> 48) | (v >> 40) | (v >> 32) | (v >> 24) | (v >> 16) | (v >> 8) | v) & 0xff;
    uint64_t z8 = ct_zero8(x8);
    return (z8 << 56) | (z8 << 48) | (z8 << 40) | (z8 << 32) | (z8 << 24) | (z8 << 16) | (z8 << 8) | z8;
}

/**
 * @brief Check if two 8-bit values are equal
 * 
 * @param a a value
 * @param b a value
 * @return 0xff if a == b, 0x00 otherwise
 */
uint8_t
ct_eq8(uint8_t a, uint8_t b)
{
    return ct_zero8(a ^ b);
}

/**
 * @brief Check if two 32-bit values are equal
 * 
 * @param a The first value
 * @param b The second value
 * @return 0xff if a == b, 0x00 otherwise
 */
uint8_t
ct_eq32(uint32_t a, uint32_t b) {
    uint32_t x = a ^ b;
    uint8_t x8 = ((x >> 24) | (x >> 16) | (x >> 8) | x) & 0xff;
    return ct_zero8(x8);
}

/**
 * @brief Check if two 64-bit values are equal
 * 
 * @param a The first value
 * @param b The second value
 * @return 0xff if a == b, 0x00 otherwise
 */
uint8_t
ct_eq64(uint64_t a, uint64_t b) {
    uint64_t x = a ^ b;
    uint8_t x8 = ((x >> 56) | (x >> 48) | (x >> 40) | (x >> 32) | (x >> 24) | (x >> 16) | (x >> 8) | x) & 0xff;
    return ct_zero8(x8);
}

/**
 * @brief Check if a is greater than or equal to b
 * 
 * @param a The first value
 * @param b The second value
 * @return 0xff if a >= b, 0x00 otherwise
 */
uint8_t
ge_64(uint64_t a, uint64_t b) {
    /* (b - a - 1) wraps to a huge value when a >= b, putting 1 in the top bit */
    uint64_t top_bit = (b - a - 1u) >> 63;     /* 1 if a < b, 0 if a >= b */
    return (uint8_t) (top_bit * 0xFFu);
}

/**
 * @brief Compare two strings for equality in constant time.
 * 
 * It always iterates lhs_len steps so use lhs for public input and rhs for secret.
 * 
 * @param lhs The first string
 * @param lhs_len Length of the first (public) string
 * @param rhs The second string
 * @param rhs_len Length of the second (secret) string
 * @return 0xff if strings are equal, 0x00 otherwise
 */
uint8_t
ct_strncmp(const uint8_t *lhs, uint64_t lhs_len, const uint8_t *rhs, uint64_t rhs_len)
{
    /* We need at least one value at rhs */
    static const uint8_t rhs_p[] = {0};
    uint64_t replace = ct_zero64(rhs_len) & 1;
    uint64_t keep = replace ^ 1;
    /* Replace rhs with rhs_p if rhs_len == 0 */
    rhs = (const uint8_t *)  ((uint64_t) rhs * keep + (uint64_t) rhs_p * replace);

    /* 0xff if lengths are equal, 0x00 otherwise */
    uint8_t result = (uint8_t) (ct_eq64(lhs_len, rhs_len) & 0xff);
    
    for (uint64_t i = 0; i < lhs_len; i++) {
        /* Use rhs[0] if i >= rhs_len */
        /* This is safe because we have already put length equality to result */
        uint64_t lt = ge_64(i, rhs_len) ^ UINT64_MAX;
        uint64_t j = i & lt;
        result &= ct_eq8(lhs[i], rhs[j]);
    }

    /* Result is 0xff only if strings are equal */
    return ct_zero8(result ^ 0xff);
}

static void
compare(const char *lhs, uint64_t lhs_len, const char *rhs, uint64_t rhs_len)
{
    fprintf(stdout, "LHS: \"");
    for (uint64_t i = 0; i < lhs_len; i++) fprintf(stdout, "%c", lhs[i]);
    fprintf(stdout, "\"\n");
    fprintf(stdout, "RHS: \"");
    for (uint64_t i = 0; i < rhs_len; i++) fprintf(stdout, "%c", rhs[i]);
    fprintf(stdout, "\"\n");
    uint8_t result = ct_strncmp((const uint8_t *)lhs, lhs_len, (const uint8_t *)rhs, rhs_len);
    printf("Result: %02x\n", result);
}

int
main(void)
{
    const char *a = "HonoluluHonoluluHonoluluHonolulu";
    const char *b = "ShanghaiShanghaiShanghaiShanghai";

    compare(a, 32, a, 32);
    compare(a, 32, a, 31);
    compare(a, 32, a, 0);
    compare(a, 0, a, 0);
    compare(a, 31, a, 32);
    compare(a, 0, a, 32);

    compare(a, 32, b, 32);
    compare(a, 32, b, 31);
    compare(a, 32, b, 0);
    compare(a, 0, b, 32);
    compare(a, 0, b, 0);

    return 0;
}