#define __HEX2INT_C__

#include <stdint.h>
#include <stdio.h>

static int8_t
hex2dec(uint8_t hex)
{
#ifdef CONDITIONAL
    uint64_t v = (hex << 24) | (hex << 16) | ((hex - 1) << 8) | (hex - 1);
    v &= 0b011111000111111101101110011011110;
    v ^= 0b011001111110001111011111110111011;
    v += 0b000000001000000010000000100000001;
    v &= 0b100000001000000010000000100000000;
    if (v) return -1;
    return (hex & 0b1111) + 9 * ((hex & 0b01000000) >> 6);
#else
    uint64_t v = (hex << 24) | (hex << 16) | ((hex - 1) << 8) | (hex - 1);
    v &= 0b0'11111000'11111110'11011100'11011110;
    v ^= 0b0'11001111'11000111'10111111'10111011;
    v += 0b0'00000001'00000001'00000001'00000001;
    v &= 0b1'00000001'00000001'00000001'00000000;
    v = ((v | (v >> 8) | (v >> 16) | (v >> 24)) >> 1) ^ 0b10000000;
    return (hex & 0b1111) + 9 * ((hex & 0b01000000) >> 6) | v;
#endif
}

static const char *
hex2int(uint64_t* __restrict dst, const char *hex)
{
    const char *p;
    *dst = 0;
    for (p = hex; p; p++) {
        if (*dst & 0b11110000'00000000'00000000'00000000'00000000'00000000'00000000'00000000) break;
        int dec = hex2dec(*p);
        if (dec < 0) break;
        *dst = (*dst << 4) | hex2dec(*p);
    }
    return p;
}

int main(int argc, const char *argv[])
{
    if (argc > 1) {
        for (int i = 1; i < argc; i++) {
            uint64_t val;
            const char *p = hex2int(&val, argv[i]);
            fprintf (stdout, "%s: %llx (%llu) [%s]\n", argv[i], val, val, p);
        }
    } else {
        for (int i = 0; i < 256; i++) {
            int d = hex2dec(i);
            fprintf(stdout, "%02x '%c' %2d\n", i, ((i >= ' ') && (i < 0x7f)) ? i : ' ', d);
        }
    }
}